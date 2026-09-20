# Git log optimization results

Optimizations are applied and measured one at a time. Each result uses the
same bare repository, `/home/git/IABSD.fr/ports.git`, unless otherwise noted.

## Easy task 1: skip commit-graph initialization for empty paths

Status: rejected and reverted

Baseline for `Git.log(repo, "master", "", 0, 10)` in fresh processes:

- 3.142 seconds
- 4.275 seconds
- 4.140 seconds

Change: initialize libgit2's commit graph only for path-filtered history.

Result after the change:

- 4.007 seconds
- 4.300 seconds
- 3.711 seconds

There was no measurable improvement. A cold revision walk and its ODB work
remain expensive even without the explicit graph initialization. The change
was reverted.

## Easy task 2: direct branch lookup for one unfiltered commit

Status: kept

Target query: `Git.log(repo, "master", "", 0, 1)`.

Baseline in fresh processes:

- 2.347 seconds
- 2.871 seconds
- 2.167 seconds

When the caller requests one commit with no path and no skip, resolve and peel
the revision directly instead of creating a revision walker.

Result after the change:

- 1.450 ms
- 1.483 ms
- 1.460 ms

The returned commit OID was compared with native Git for both `master` and
`master~1`; both matched. The optimization reduces this query from seconds to
approximately 1.5 ms and is kept.

## Easy task 3: precompute changed-path Bloom hashes

Status: rejected without a production-code change

Before implementing the optimization, a standalone benchmark ran the exact
Murmur implementation four times per commit for all 212,319 commits. Four
hashes per commit are an upper bound for `audio/csound`; filters that reject
the full path do less work.

Results:

- 1.791 ms
- 1.607 ms
- 1.596 ms
- 1.670 ms
- 1.649 ms

Removing at most approximately 1.7 ms from a 2.6-second operation is not worth
the additional query structure, allocation, and lifecycle code. No production
change was retained.

## Easy task 4: initialize libgit2 cache options once

Status: rejected without a production-code change

Measure the existing `git_libgit2_opts` call directly before changing its
lifecycle.

One million calls in a tight loop took:

- 5.279 ms
- 5.315 ms
- 5.299 ms
- 5.300 ms
- 5.652 ms

One call is effectively unmeasurable relative to the Git log operation. No
production change was made for speed. It was later moved to `Git.init` as a
thread-safety prerequisite for the shared ODB cache.

## Easy task 5: cache author lookups

Status: rejected without a production-code change

Measure `RepoController.log_add_authors` and its Facts queries before changing
cache scope or invalidation behavior.

One thousand executions of the same three-pattern Facts query took:

- 71.003 ms
- 70.446 ms
- 70.096 ms
- 71.501 ms
- 71.343 ms

One uncached author costs approximately 0.071 ms. Even ten distinct authors
would remain below 1 ms, while a wider cache would require synchronization and
invalidation when users change their email or slug. No production change was
made.

## Easy-task conclusion

Only the direct one-commit branch lookup produced a material, repeatable
improvement. The other proposed easy changes were rejected after measurement.
Further work should begin by profiling ODB and commit-graph lifecycle rather
than modifying the scan loop speculatively.

## Production task 6: cache initialized Git ODBs

Status: implemented, awaiting production measurement

The cache retains up to eight initialized ODBs keyed by objects-directory
path. Repository handles and reference databases remain request-local. Cache
entries are invalidated when the commit-graph or graph-chain file identity
changes. Eviction is synchronous and adds no worker threads or processes.
