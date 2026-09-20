# Git log performance plan

## Current profile

The main remaining cost is repository/ODB and commit-graph lifecycle, not
rendering, branch enumeration, threads, or process creation.

- Full HTTP request median: approximately 2.75 seconds.
- `Git.log` on a fresh repository: approximately 2.41 seconds.
- `Git.Repository.free`: approximately 0.22 seconds.
- Branch listing and HEAD lookup: below measurable milliseconds.
- Path log on an already initialized repository: approximately 0.68–0.90
  seconds.
- Exact commit lookup without graph initialization: approximately 5 ms.
- Template rendering baseline: approximately 1 ms.

The changed-path Bloom implementation is effective: it reduced the request
from approximately seven seconds to 2.7 seconds. The dominant remaining
problem is rebuilding its supporting graph state for every HTTP request.

## Easy improvements

### 1. Avoid graph initialization for unfiltered logs

When `path == ""`, do not initialize the commit graph merely to retrieve the
latest commits. This should substantially improve repository and unfiltered
log pages.

For `limit == 1`, `skip == 0`, and an empty path, resolve the branch directly
to its commit instead of starting a revision walk.

This does not improve the `/audio/csound` case because that request has a path.

### 2. Precompute the Bloom query

Compute the Bloom hashes for the requested path and all its prefixes once per
query. For `audio/csound`, this means precomputing hashes for `audio` and
`audio/csound` instead of recalculating them for every visited commit.

The per-commit Bloom operation should only perform filter bit tests.

### 3. Initialize libgit2 cache options once

Move `git_libgit2_opts(GIT_OPT_SET_CACHE_OBJECT_LIMIT, ...)` out of
`kc3_git_log` and into Git module initialization. This is a small cleanup and
is not expected to materially change the request time.

### 4. Cache author lookups

The controller currently memoizes author email lookups only for the duration
of one request. A bounded process cache or batch lookup could remove repeated
Facts queries, but this is a low-priority optimization because all non-Git
request work currently accounts for only a small part of the total time.

## Medium improvements

### 5. Reuse initialized ODBs

This is the highest-value next improvement for path-filtered logs.

The application currently opens and frees a repository for every request.
Opening the repository itself is cheap, but each fresh repository creates a
fresh ODB, attaches and initializes the commit graph, and destroys that state
again at the end of the request.

Keep a bounded cache of initialized ODBs keyed by repository path and attach
the cached ODB to each freshly opened repository. This keeps reference lookup
request-local and fresh while reusing expensive object database and
commit-graph state.

Requirements:

- Do not add threads or processes.
- Bound the cache by entry count and/or memory use.
- Use explicit reference ownership.
- Protect shared cache metadata with a mutex.
- Define graph refresh behavior when the commit-graph chain changes.
- Prefer per-worker ownership or carefully audited ODB sharing; do not share a
  mutable `git_repository` concurrently between requests.

Based on the warm-repository measurements, this could move the
`/audio/csound` request from approximately 2.75 seconds toward 0.8–1.0
seconds, before further scan-loop optimization.

### 6. Cache or memory-map the custom graph

The commit graph is currently loaded twice:

1. libgit2 opens and attaches the graph to its ODB.
2. The custom Bloom reader allocates and reads the same 15.1 MB graph file.

Cache or memory-map the custom parsed representation using the
content-addressed graph filename as its identity. When the graph chain points
to a new filename, create a new cached mapping and retire the old one after its
last user releases it.

This removes repeated allocation, file reads, parsing, and freeing. It does
not by itself remove libgit2's graph initialization cost.

### 7. Cache log results

Use a bounded cache keyed by:

`(repository identity, resolved tip OID, path, skip, limit)`

Cache the Git log data, not the rendered HTTP response, so authorization and
user presentation remain request-specific. Including the resolved tip OID
naturally invalidates results after branch updates or force-pushes.

This can make repeated URLs very fast, but will provide less benefit for
crawlers visiting many unique paths.

### 8. Reduce graph entry searches

For linear history, carry the current graph entry and its first-parent
position into the next iteration. If the next OID returned by the revision
walker matches that parent, use the entry directly instead of performing
another fanout lookup and binary search. Fall back to the normal lookup at
merges or whenever the predicted OID does not match.

Benchmark this after ODB reuse, because fixed graph initialization currently
hides most scan-loop improvements.

## Hard improvements

### 9. Traverse the commit graph directly

Replace `git_revwalk_next` for supported log queries with traversal based on
commit-graph parent positions. This would allow path history to use the custom
graph and Bloom filters without initializing libgit2's commit graph.

The implementation must preserve:

- `GIT_SORT_TIME` ordering.
- Merge traversal.
- Revision ranges and hidden commits.
- Fallback for commits absent from the graph.
- Correct behavior with shallow repositories, replacement objects, and stale
  graph files.

This has large potential but also the highest correctness risk.

### 10. Add changed-path Bloom support to libgit2

The cleanest long-term design is for libgit2's revision walker to consume the
commit graph and changed-path Bloom filters directly. This avoids maintaining
two graph readers and lets one graph representation serve both traversal and
path filtering. It likely requires an upstream contribution or maintaining a
local libgit2 patch.

## Recommended implementation order

For the path-filtered `/audio/csound` endpoint:

1. Reuse an initialized ODB and benchmark it.
2. Cache or memory-map the custom graph and benchmark it.
3. Precompute Bloom query hashes and benchmark the scan loop.
4. Reduce repeated graph entry searches.
5. Add a bounded log-result cache if production traffic has useful repetition.
6. Consider direct commit-graph traversal only if the remaining latency
   justifies its complexity.

Separately, implement the empty-path fast paths because they are simple and
should remove the same fixed graph cost from repository and unfiltered log
pages.

Each step should be committed and benchmarked independently. Keep a change
only when it improves the relevant benchmark without weakening correctness.
