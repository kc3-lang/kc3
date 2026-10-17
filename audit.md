# KC3 audit log — continuation

- Review dates: 2026-10-12–2026-10-13.
- Baseline: `19a4221d1` (`audit`); tracked worktree clean at the start.
- Original audit method: source inspection and caller/ownership tracing. Primary
  dependency documentation/source was consulted to verify API contracts.
  No builds, tests, malformed-input probes, GUI applications, or network
  replication sessions were run for this pass.
- Remediation: F12–F23 implementations and regression tests added on
  2026-10-13–2026-10-17 using the existing test infrastructure.
  No commits were created by the agent.
- Status: F12–F23 fixed within the scope documented below.
  The original audit was source-only; runtime checks below concern F12–F23.

Severity is a remediation priority, not a claim of remote exploitability.
High denotes a concrete native memory-safety defect; Medium denotes data loss,
incorrect synchronization, hangs, process termination, or accumulating leaks.
Exposure and preconditions are stated separately for each finding.

## Coverage

This pass extends the earlier audit into previously omitted areas. It is still
a targeted review, not a claim that every file or concurrency schedule is safe.

| Area | Work performed | Limits |
| --- | --- | --- |
| Facts database | Traced add/remove/replace, transaction start/end/rollback, logging, cursor ownership, reconnect synchronization, and their language wrappers | Not a complete proof of every index, persistence format, or distributed execution |
| Concurrency | Read recursive rwlock/mutex wrappers, thread creation/join, forked environment ownership, and connection lifecycle; F15 adds focused rwlock concurrency tests; F23 adds allocation-tracked environment/thread teardown and pinned-reader tests | No race detector or exhaustive lock-order analysis; raw stacktrace pointers require external owner-lifetime synchronization |
| Graphics/image | Reviewed mesh allocation/fill, bitmap text, image load/resize/encode, GL sprite and Cairo sprite paths; F16–F20 add headless mesh/text tests, real font rasterization, PNG/JPEG decoding checks, resize CLI cleanup checks, and failure-path tests | No GPU execution; platform window backends and all drawing/math routines remain incomplete |
| GUI bindings | Traced GTK signal ABI and selected object/text/action ownership; F21 adds headless GObject signal emission, KC3 callback evaluation, rejection, and ownership tests | GTK language binding is under `lib/disabled`; no GUI execution or complete binding audit |
| Compiler/cache | Read `to_c/to_c.c`, its CLI, `kc3c/kc3c.c`, and selected `env_load`/cache-loading paths; F22 adds generator progress, EOF, unsupported-input, and CLI I/O/cleanup tests | C generation is explicitly unfinished; generated-program correctness is not established |
| Dependencies | Recorded checked-out submodule revisions; inspected selected libtommath/AVIR boundaries and verified FreeType, libpng, libjpeg and GObject contracts | Not a complete third-party source audit or a current-vulnerability/advisory inventory |

Selected checked-out dependencies: libtommath `ea788234b525`, AVIR
`c5f2fbda013e`, linenoise `d771957055b6`, fonts `31cc58b1cc11`, SMTP
`87d913607d05`, and ucd2c `e6938e305b9d`. A revision listing is not a security
clearance. SMTP/ucd2c internals and the full font corpus were not audited here.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F12 | Medium | Facts rollback has no undo records; language error paths end the transaction | Fixed; regression tests pass |
| F13 | Medium | Facts persistence reports success despite write failures | Fixed; regression tests pass |
| F14 | Medium | Reconnect synchronization misses deletions of older facts | Fixed within documented scope; regression tests pass |
| F15 | Medium | Recursive rwlock owner tracking has an unsynchronized data race | Fixed within documented scope; concurrency tests pass |
| F16 | High | GL mesh counts wrap before allocation; fill loops overrun the allocation | Fixed within documented scope; regression tests pass |
| F17 | High | GL text rasterization writes outside advance-sized image storage | Fixed within documented scope; regression tests pass |
| F18 | Medium | PNG string output reaches a non-progressing fixed-buffer write loop | Fixed within documented scope; regression tests pass |
| F19 | Medium | Malformed JPEG input invokes the dependency's process-exit handler | Fixed within documented scope; regression tests pass |
| F20 | High | Image resize CLI error paths free uninitialized/already-freed storage | Fixed within documented scope; regression tests pass |
| F21 | Medium | Generic GTK signal callback does not match parameterized signal ABIs | Fixed by restricting signatures; regression tests pass; binding remains optional/disabled |
| F22 | Medium | C generator retries non-matching input without progress and hides parser errors | Fixed within documented scope; regression tests pass; generation remains unfinished |
| F23 | Medium | Forked environments leak their allocated stacktrace object | Fixed within documented ownership scope; regression tests pass; borrowed-pointer lifetime limits apply |

## Findings

### F12 — Medium: transactions do not roll back their mutations

**Locations (audited baseline):** `libkc3/facts_transaction.c:70–111,120–137`,
`libkc3/facts.c:417–533,2132–2239`, `libkc3/env.c:1752–1765`.

`facts_transaction_start` initializes `transaction->log` to NULL. Neither
the add nor remove paths append undo records; searching the transaction-log
references finds initialization, traversal, and deletion, but no population.
`facts_transaction_rollback` consequently traverses an empty log and ends
the transaction without reversing mutations. For example, start a transaction,
add a previously absent triple, and call rollback: the add has already changed
the indexes, and rollback has nothing with which to remove it.

The public `Facts.with_transaction` path additionally calls
`facts_transaction_end`, not rollback, when evaluating its body fails or
unwinds. `env_module_load` does call rollback on failure (`env.c:2998–3007`),
but is affected by the empty undo log. Partially loaded definitions and the
module's loading marker can therefore survive a failed load.

**Impact/exposure:** database atomicity and error recovery disagree with
`doc/1_KC3/1.25_Facts.en.md`. This does not require concurrency, a malformed
serialized object, or a malicious network peer. Disk logging and broadcasts
also happen as mutations are applied, not after an outer transaction commits.

**Suggested remediation:** define commit/rollback behavior for nested
transactions, retain owned undo data before changing indexes, and roll back
evaluation failures. Coordinate log/replication publication with commit.
Simply changing the language wrapper to call rollback is insufficient.

**Original check plan:** add/remove/replace followed by rollback;
inner commit followed by outer rollback; failing module load followed by
retry; and persistence/replication visibility of an aborted transaction.

**Remediation, 2026-10-14:** add/remove paths now reserve undo records
before changing indexes. Records retain references to all three fact tags
and preserve original fact IDs. Rollback applies inverse local operations
without logging, broadcasting, or recording more undo actions. Inner commit
merges its undo records into the parent; outer rollback also unwinds any
still-open inner transactions. Cleanup clears transferred/freed log links.

Outermost commit publishes queued persistence records, hooks, broadcasts,
and preallocated replication removal records in mutation order. Aborted
transactions publish none of these. `Facts.with_transaction` rolls back
evaluation failures and nonlocal exits, keeping its mutable transaction
state on the heap across `setjmp`/`longjmp`. Normal `false` results commit.
Replacement rolls back if its final add fails; transaction-start failures
are checked by replacement/wildcard removal. ID-exhaustion failure releases
the transaction write lock.

**Scope/limits:** transactions on replicas forwarding to a remote master
are rejected before mutation: the existing protocol has no distributed
transaction operation. This includes replacement/wildcard removal paths
that internally start a transaction. Publication at commit is still a
sequence of records, not crash-atomic persistence or atomic remote replay.
Reported publication errors leave the in-memory commit applied. F13's
unchecked I/O failures and F14's reconnect deletion cursor are addressed
separately below.
Arbitrary body side effects, loader cache/state outside the facts database,
and recovery from allocation failure during undo are not made transactional.

**Regression tests:** eleven cases added to the existing `facts_test.c`
and `env_test.c` runners, with two module-load fixtures under
`test/facts_transaction/`. The first six new Facts cases failed against
the original code (22 passed / 6 failed in the focused baseline).

Coverage includes add and explicit-ID add rollback, duplicate/no-op
mutations, owned tag lifetime after removal, restoration of original IDs
and all four indexes, replacement and wildcard-removal rollback, inner
commit/outer rollback, inner rollback/outer commit, rollback of still-open
inner transactions, repeated cleanup, and successful commit. Persistence
tests reopen the database after both rollback and commit. A buffered
connection exercises the real broadcast writer without a separate server;
aborted transactions produce no wire bytes, log hooks, persistent records,
or replication removal records. Environment tests cover evaluation failure,
nonlocal exit, normal false-result commit, and repeated failed module loads
followed by successful retry. Remote-master rejection covers explicit
transactions and the implicit replacement/wildcard-removal paths; release
tests also check ID exhaustion and lock release.

**Validation:** all commands source `./env` first.

- `make`: passed, including release and debug builds.
- Existing focused runner (`KC3_TEST='facts env'`): 44/44 release,
  43/43 debug.
- `make test`: 473/473 across 12 suites, including 348 core tests.
- `make test_debug`: 465/465 across 11 suites, including 343 core tests.
- `git diff --check`: passed. No new source comments or commits.

No standalone probe results are used as validation. Allocation-failure
injection and crash/reconnect atomicity are not covered by these tests.

### F13 — Medium: persistence failures can be acknowledged as success

**Locations (audited baseline):** `libkc3/marshall.c:2134–2165`,
`libkc3/facts.c:393–405,508–520,1320–1401,2158–2166`,
`libkc3/buf_file.c:168–201`.

`marshall_to_buf` checks the header and payload transfers but ignores its
final `buf_flush(out)` result. It then resets the marshalled chunk and returns
a positive byte count. A record that fits in the output buffer can therefore
be reported written when the actual `fwrite`/`fflush` fails.

Even failures detected earlier are mishandled further up: the two add paths
test `! facts_log_add(...)`, so the documented negative error result is
truthy and does not enter cleanup. The remove path ignores the result of
`facts_log_remove` altogether.

**Impact/exposure:** disk-full, file-size-limit, or other storage failures
can leave successful in-memory mutations without the corresponding durable
log record. A restart can lose acknowledged additions or restore deleted
facts. This is an I/O failure-handling issue, not a claim that the API promises
power-loss durability through `fsync`. Some earlier error paths assert in
debug builds; the unchecked final flush is not protected by those assertions.

**Suggested remediation:** propagate final flush failure before resetting
the record, test signed results with `< 0`, and define mutation/log ordering
and the database's state after a partial write. Do not blindly retry a partly
written record as though nothing reached the output.

**Original check plan:** fail only the final flush of a small record;
fail a mid-record flush; test add and remove results; reopen the log and
compare its state with acknowledged operations.

**Remediation, 2026-10-14:** based on `20a1ac5c8`
(`astra audit findings #12`). The existing `marshall`, `buf_file`, and
`facts` runners now contain seven additional regression cases. All seven
failed against the baseline (60 passed / 7 failed in the focused run).

The implementation checks final flush and serialization results, handles
negative log returns in both add paths and removal, and latches failed logs
and FILE-backed writers. Failed logs reject subsequent writes/transaction
starts, and close does not retry pending KC3 buffer contents. Commit
publication stops at its first failure, without issuing that operation's
hook/broadcast or publishing later records. Ordinary I/O errors return
failure in debug builds as well as release builds.

Nontransactional failed additions are removed from memory and failed
removals leave their fact intact. As documented under F12, publication
failure does not roll back an already committed in-memory transaction.
Partial writes and failures between primary/after-dump log writes leave
the failed operation's durable outcome uncertain. Recovery requires
checking the log and rebuilding a fresh database, not retrying a partially
consumed marshalling record. This does not add `fsync`, automatic tail
repair, or crash-atomic/multi-file transactions.

**Focused validation:** 67/67 release and 64/64 debug tests pass using
`KC3_TEST='marshall facts buf_file'` with the existing core test runner.
The cases cover final/header/heap/payload flush errors without resetting
the failed marshalling chunk; a real `RLIMIT_FSIZE` error followed by limit
restoration/`clearerr`; add, explicit-ID add, and remove failure; callbacks
that write a real 20-byte prefix before failing; primary and secondary-log
errors for both record types; rejection of subsequent writes; absence of
close-time retries; stopping commit hooks/broadcasts/removal records; and
reopening a database after a failed write that emitted no bytes.

**Full validation:** all commands source `./env` first.

- `make`: passed, including release and debug builds.
- `make test`: 480/480 across 12 suites, including 355 core tests.
- `make test_debug`: 472/472 across 11 suites, including 350 core tests.
- `git diff --check`: passed. No new source comments or commits.

No standalone probes are used. Allocation-failure injection and
crash/reconnect atomicity are not covered by these tests.

### F14 — Medium: reconnect synchronization misses old-fact deletions

**Locations:** `libkc3/facts.c:2158–2182`, `libkc3/fact.c:166–181`,
`libkc3/facts_connection.c:404–438`.

Removal-log entries copy the removed fact with `fact_w_init_fact`, retaining
its original insertion ID. Reconnect synchronization sends a removal only if
that old fact ID is at least the peer's `remote_next_id`.

Consider a peer that has received fact ID 1 and advanced to next ID 2. The
peer disconnects; the master deletes fact 1 with removal tracking enabled.
The tombstone still contains ID 1, so synchronization filters it out against
2. If on-disk logging is disabled, removal does not advance `next_id` either,
and the earlier `facts->next_id <= remote_next_id` check can skip the entire
sync. Enabling disk logging does not fix the tombstone's old ID.

**Impact/exposure:** an authenticated replication peer can retain stale
facts after reconnecting. Deletion convergence fails during an ordinary
outage; no hostile input is necessary. This example assumes `server_count`
is configured to retain removals and the disconnected connection is cleaned
up so a replacement connection can be established.

**Suggested remediation:** give deletion events their own monotonically
advancing replication sequence, separate from a fact's creation ID, and
track peer acknowledgements against that sequence. Review tombstone retention
and reconnect cursors together.

**Original check plan:** disconnect a caught-up peer, delete an existing
fact, reconnect, and require convergence, both with and without disk logging.

**Remediation, 2026-10-14:** based on `170819802`
(`facts: error handling`). Retained deletions now carry a separate event ID;
replicated removals allocate an ID even without a disk log. Transaction
mutations also receive distinct IDs rather than sharing their transaction's
start ID, so a partially received commit has an unambiguous resume cursor.
Undo records keep original fact IDs. Binary log replay preserves stored
insertion IDs and consumes the removal record's event ID.

Reconnect replay merges retained removals and surviving additions in
ascending event order. The receiving path advances its cursor only after
successful application, including an already-absent deletion, and rejects
overflowing IDs. Failed writes or application stop the stream before later
events can skip the failed record. Snapshot replay and registration for
live broadcasts share the database write lock. Failed persistence logs and
peers whose cursor is ahead of the authority are rejected for sync.

Removal records are not pruned on send: without stable peer identities and
application acknowledgements, repeated reconnects by one peer cannot safely
stand in for acknowledgements from distinct peers. The records remain until
database cleanup, with memory proportional to retained deletions. This does
not implement bounded retention or durable acknowledgement tracking.

**Compatibility and scope:** the handshake now advertises protocol version
2. Upgrade all peers together and reinitialize legacy replication state
from an authoritative snapshot with distinct insertion IDs; shared legacy
transaction IDs are not automatically migrated. Configure nonzero
`server_count` before replicated writes. The fix covers reconnect within
the authoritative database's lifetime, without independent offline writes
on replicas. Tombstones are not persisted across restart/compaction;
history loss requires snapshot-based reinitialization. Leader failover,
conflict resolution, and crash-atomic transactions remain outside scope.

**Regression tests:** eight cases in the existing `facts_test.c` runner
exercise the real replication serializer, decoder, sync, and apply paths
using buffers, not standalone probes. The initial six cases all failed
before the behavior changes (35 passed / 6 failed); extracting the existing
sync/apply functions made those paths callable by the test runner.
Coverage includes old-fact deletion with and without disk logging,
delete/re-add ordering, interrupted replay, two offline peers and discarded
transmissions, nested commit/rollback, write errors, absent-fact deletion,
cursor overflow, live broadcasts, and replica persistence failure. Existing
rollback/reopen tests now compare saved insertion IDs rather than assuming
every mutation shares a transaction ID.

**Validation:** all build/test commands source `./env` first.

- Existing focused release runner (`KC3_TEST=facts`): 43/43 passed.
- `make`: passed, including release and debug builds.
- `make test`: 488/488 across 12 suites, including 363 core tests.
- `make test_debug`: 480/480 across 11 suites, including 358 core tests.
- The existing TLS connection/authentication and acceptor tests passed
  with protocol version 2 in both suites.
- `git diff --check`: passed. No new source comments or commits.

No standalone probes are used. The new reconnect regressions use buffered
connections rather than a separate TLS server. Race-detector execution,
allocation-failure injection, crash/restart recovery, legacy-peer handshake
integration, and full multi-peer concurrency stress are not covered.

### F15 — Medium: rwlock recursion metadata races before locking

**Locations (audited baseline):** `libkc3/types.h:516–523`,
`libkc3/rwlock.c:75–90,135–175`.

`rwlock_r` and `rwlock_w` read the ordinary `pthread_t thread` field before
acquiring the native lock, to detect a recursive call by the writer.
The owning writer assigns that field after acquiring the write lock and
clears it before unlocking. A second thread's pre-lock read can run
concurrently with either store; no atomic access or separate synchronization
protects the metadata.

**Impact/exposure:** the wrapper itself has a C data race during valid
concurrent use. Acquiring the native lock later does not retroactively
protect an earlier read. The affected wrapper is used by buffers and facts,
so this is not limited to an explicitly unsafe caller. No particular
miscompilation, crash schedule, or exploit has been demonstrated.

**Suggested remediation:** implement race-free recursive ownership tracking
with a portable design, preserving nested writer/read behavior. Making the
field `volatile` is not a fix; nor should a metadata lock be held while
waiting for a native lock whose owner needs that metadata lock to release it.

**Original check plan:** concurrent reader/writer acquisition under a
race detector, recursive writes, and a read nested inside an owned write.

**Remediation, 2026-10-14:** a private, statically initialized pthread mutex
now protects all live accesses to the wrapper's writer identity and
recursion count. Ownership checks use `pthread_equal` only when the count
is positive; zero means no writer, without comparing an invalid or expired
thread ID. Recursive writes increment the count under the metadata mutex;
the final write release clears the count under the same mutex before
unlocking the native rwlock. Write release checks the calling owner, and
recursive acquisition rejects count exhaustion without signed overflow.

The metadata mutex is released before every potentially blocking native
rwlock acquisition and is never held across a caller's critical section.
The owning writer can therefore acquire it to recurse or release while
another thread waits for the native lock. Normal readers continue to use
the native shared lock; reads nested inside the owning writer remain no-ops.
The public lock layout and bindings are unchanged.

**Scope/tradeoff:** this is one process-private metadata mutex shared by
all wrapper instances, so ownership checks on unrelated locks briefly
serialize. No throughput benchmark was performed. Initialization must
finish before publication; cleanup still requires all users to have
finished. Read-to-write upgrade is not added, and a writer's nested reads
must be released before its final write unlock. This change is not an
audit of caller lock ordering, object lifetime, or arbitrary concurrent
access to the public struct fields.

**Regression tests:** five cases in `test/rwlock_test.c`, integrated into
the existing core runner, cover recursive writes and reads, writer-nested
reads, simultaneous readers excluding a writer, 100 recursive-writer
handoffs, independent locks, and six-thread mixed contention (10,000
iterations per worker). Worker checks use atomic coordination rather than
the runner's shared assertion counters. Each case runs in a forked child
with a 30-second alarm so a deadlock fails instead of hanging the suite.
`test/configure` now calls the existing pthread feature helper: previously
the tests linked pthread without defining `HAVE_PTHREAD`, silently skipping
guarded test bodies. Source manifests were regenerated with `./update_sources`.

**Validation:** all build/test commands source `./env` first.

- Existing focused runner (`KC3_TEST=rwlock`): 5/5 passed in both release
  and debug builds. Ten additional runs per build passed all 100 cases.
- `make`: passed, including release and debug builds, after regenerating
  source manifests with `./update_sources`.
- `make test`: 493/493 across 12 suites, including 368 core tests.
- `make test_debug`: 485/485 across 11 suites, including 363 core tests.
- `git diff --check` and whitespace checks of the new files: passed.
  No new source comments or commits.

These ordinary concurrency tests also passed against the original
implementation: they protect behavior but do not constitute a reproduction
or proof of race freedom. The race fix is supported by the
synchronization/source review. No usable installed ThreadSanitizer runtime
or Valgrind was found on this OpenBSD host, so no race-detector result is
claimed. Other platforms, exhaustive schedules, and performance benchmarks
were not tested. No standalone probes were added or run.

### F16 — High: graphics mesh allocation counts wrap in 32-bit arithmetic

**Locations (audited baseline):** `gl/gl_sphere.c:31–90`, `gl/gl_square.c:31–89`,
`gl/gl_box.c:31–70`, `gl/gl_object.c:19–31`.

Mesh dimensions are `u32`. Expressions such as
`(seg_u + 1) * (seg_v + 2)` and `2 * seg_u * (seg_v + 1)` are therefore
computed in 32-bit unsigned arithmetic before being passed to
`gl_object_allocate`'s wider `uw` parameters. The fill loops retain the
original dimensions and increment raw vertex/triangle pointers.

For a sphere with both dimensions 65536, the requested counts wrap from
4,295,163,906 to **196,610 vertices** and from 8,590,065,664 to **131,072
triangles**. The resulting allocations are only about 7.5 MiB combined with
the current 32-byte vertex and 12-byte triangle layouts. If initialization
and allocation succeed, the vertex loop writes past its allocation after
196,610 vertices, long before it finishes the requested mesh. This affects
64-bit builds as well; widening the function parameter is too late.

Square and box constructors use the same unchecked narrow arithmetic before
dimension-driven fill loops. This is not the earlier F01 deserialization
stride issue.

**Impact/exposure:** native heap overwrite for oversized mesh parameters.
A usable GL context and a caller supplying those parameters are required.
The inspected earth demo uses constants; no remote parameter path or working
exploit is asserted.

**Suggested remediation:** validate additions, products, byte sizes and
index representability before allocation; compute in a checked wide type;
use validated counts consistently in loops and unwind partial initialization.

**Original check plan:** the wrapped-count example, each dimension
boundary, zero/default dimensions, and normal small meshes. Reject oversized
requests before exercising any overflowing write.

**Remediation, 2026-10-14:** sphere, square, and box constructors now compute
counts in `u64` and validate them before creating GL buffers or allocating
mesh arrays. Addition bounds and intermediate-product bounds are checked
before subsequent arithmetic, including the box's three-face sum. A shared
`gl_object_size_valid` check requires positive counts, vertex counts within
the signed 32-bit point-draw range (and therefore the unsigned 32-bit index
range), triangle counts no larger than `S32_MAX / 3`, and both byte sizes
within `SW_MAX` for allocation and signed GL buffer-upload sizes. Values
are narrowed to `uw` only after validation. Accepted totals also bound all
existing `u32` loop counters, row strides, plane offsets, and vertex indices.

Reviewing every caller of `gl_object_allocate` found the same unchecked
arithmetic in `gl/gl_cylinder.c:24–58`. It receives the same preflight checks
and failure cleanup. Sphere/square/box minimum-dimension clamping is
unchanged; the cylinder now returns NULL for zero dimensions in both builds.
The box's final face-pair loop was also missing a triangle-pointer increment,
leaving part of the allocated triangle array unwritten. The increment is
restored so its filled triangle count matches the validated allocation.

The shared allocator checks counts itself, verifies the language-side array
element strides against the C vertex/triangle layouts before data allocation,
and builds both arrays in temporaries. Failure frees temporary metadata/data
and preserves existing object arrays; successful replacement frees the old
arrays. Constructors release GL buffers when array setup fails and leave the
destination unchanged. Failure to create the second GL buffer releases the
first instead of leaking it.

**Regression tests:** fourteen cases in `test/gl_mesh_test.c` use the existing
core runner, not a standalone probe or executable. The fixture compiles the
actual mesh/object implementation sources with namespaced GL test doubles,
uses the real KC3 type modules and array metadata routines, and guards mesh
data with sentinel bytes. Test configuration enables these cases when GLES2
headers/libraries are available; they ran on this host. Generated source
lists were refreshed with `./update_sources`.

Coverage includes the 65536-by-65536 wrap, maximum `u32` inputs, zero/default
dimensions, index/draw/byte limits, and each dimension's accepted/rejected
boundary. Large accepted boundary cases inject allocation failure before
allocating mesh data, while oversized cases must not reach GL initialization.
Small asymmetric meshes check counts, finite vertex positions, complete
triangle initialization, in-range indices, and allocation guards. Failure
injection covers either buffer creation, either array metadata/data allocation,
and either incompatible element stride, plus failed/successful replacement of
existing arrays. Cylinder checks cover counts and storage bounds, not complete
geometry generation.

**Baseline evidence:** before the production changes, all four oversized-mesh
rejection cases failed because they reached GL buffer creation. That fixture
deliberately failed the first buffer creation to avoid running an overflowing
fill. The second-buffer failure case also exposed the leaked first buffer.
The initial small-mesh checks lacked explicit GL type-module loading and
failed setup; these are not counted as defect reproductions. The fixture was
corrected before post-fix validation. No native heap overwrite was executed.

**Validation:** all build/test commands source `./env` first.

- Existing focused runner (`KC3_TEST=gl_mesh`): 14/14 passed in release
  and debug builds; final boundary additions also passed in the full suites.
- `make`: passed, including release and debug builds of the actual GL
  library and its consumers, after `./update_sources`.
- `make test`: 507/507 across 12 suites, including 382 core tests.
- `make test_debug`: 499/499 across 11 suites, including 377 core tests.
- `git diff --check` and whitespace checks of the new test source: passed.
  No new source comments or commits.

**Scope/limits:** these are headless CPU/count/cleanup tests, not real GL
driver/context, GPU upload, or rendering tests. No 32-bit executable or
sanitizer build was run. Bounds prevent integer wrap and undersized storage;
they do not impose a memory/time quota on otherwise representable meshes or
change the allocator's existing out-of-memory policy. The cylinder remains
unfinished (packed position writes, no completed triangle generation, and a
no-op render function). The box's unsigned negative-face normal calculation
and duplicated final-face `tex_coord_x` assignment are also unchanged;
other mesh shading/texture/rendering correctness is not certified by this
fix. F17–F23 remain separate work.

### F17 — High: bitmap text writes are not bounded by the allocated image

**Locations (audited baseline):** `gl/gl_text.c:98–168`.

The sizing pass derives image width from summed `horiAdvance` values. The
drawing pass instead writes every pixel in `glyph->bitmap.width` at `x + j`,
without checking against `data_w`. Its vertical offset is also computed
with unsigned arithmetic from `bitmap_top` and the scaled face ascent,
without a bounds check.

Advance distance does not bound bitmap width. As a source-level example,
a glyph with a 4-pixel advance, 8-pixel bitmap width, and bitmap top equal to
the computed ascent gives `data_w = 4`. The first bitmap row is placed at
`data_y = data_h - 1`; its fifth pixel starts immediately beyond the allocated
image. These are different metrics, not a guarantee supplied by FreeType.
See the [FreeType glyph metrics contract](https://freetype.org/freetype2/docs/glyphs/glyphs-3.html).

**Impact/exposure:** out-of-bounds native writes when loaded glyph extents
exceed the advance/ascent-based assumptions. Font/text selection and the GL
text path are required. This example was derived from the writes, not measured
against a particular bundled font, and does not assert a font-library defect.

**Suggested remediation:** measure actual rendered glyph bounds, use signed
bearing-aware coordinates, validate allocation sizes/results, and bound each
pixel or row copy. Source-row addressing should honor bitmap pitch as well.

**Original check plan:** overhanging and zero-advance glyphs, accents
above nominal ascent, negative bearings, multiline strings, and allocation
failure, with guarded output storage or memory instrumentation.

**Remediation, 2026-10-15:** layout now uses signed coordinates and measures
rendered bitmap rectangles, including left/top bearings, in addition to pen
advances and nominal line boxes. Negative kerning and zero-advance glyphs no
longer become unsigned offsets. The canvas expands for overhangs and accents,
then translates the measured bounds into nonnegative texture coordinates.
Scaled FreeType size metrics replace the float-to-unsigned ascent/descent
conversion. Both passes share the same layout code, and drawing validates
each complete glyph rectangle against the allocated image before copying.
Changed bounds or errors during the second pass abort before texture upload.

Bitmap validation checks nonempty buffers, row pitch, source-offset arithmetic,
pixel format, and grayscale range. Row addressing handles padding and both
pitch signs, consistent with [FreeType's bitmap conversion implementation](https://raw.githubusercontent.com/freetype/freetype/master/src/base/ftbitmap.c).
Grayscale and monochrome bitmaps are supported; other nonempty formats are
rejected rather than interpreted as byte-per-pixel grayscale. Overlapping
glyph coverage is composited so transparent pixels do not erase previous ink.

Coordinate/span limits, the active GL maximum texture size, and the RGBA byte
product are checked before allocation. Allocation failure returns false;
temporary pixels are freed on all later failures. Whitespace remains a logical
line/advance extent; zero-sized logical textures upload a minimal transparent
image, clearing stale text without allocating zero bytes. FreeType, UTF-8
decoding, upload, and mipmap failures are reported; size fields are published
only after successful upload/mipmap completion. The string/buffer update
wrappers now propagate rasterization failure instead of returning success.
String replacement copies before releasing the old value; texture creation
and initialization-copy failures now return failure and release owned textures.

**Regression tests:** fourteen cases in `test/gl_text_test.c` run through the
existing core runner. The fixture compiles the actual implementation with
namespaced GL/FreeType test doubles, captures uploaded pixels, and guards the
allocation on both sides. Each case uses a child with a timeout so baseline
crashes or hangs become test failures. No standalone probe or executable was
added. Test configuration enables these cases with GLES2 and FreeType support;
source lists were regenerated with `./update_sources`.

The initial four cases failed against the original code (0 passed / 4 failed):
overhang and bearing cases changed allocation guards, zero advance terminated
the child abnormally, and negative pitch produced reversed/incorrect rows.
After remediation, those cases pass. Added coverage includes positive padded
pitch, negative kerning and overlap, multiline/space/empty text, monochrome
and reduced grayscale ranges, invalid bitmap metadata, signed-coordinate and
allocation-size limits, texture limits, invalid UTF-8, allocation/load/kerning
failures, changed second-pass glyph bounds, GL failures, and wrapper error
propagation. A real FreeType path renders bundled regular and italic Courier
Prime fonts at 12, 24, and 48 pixels with four strings (24 combinations),
including accents, a combining mark, multiline text, spaces, and empty text.
The final string-ownership case failed before its fix (13 passed / 1 failed)
and now covers copy failure, texture creation failure, and balanced cleanup.

**Validation:** focused release and debug runners each pass 14/14. The full
build passed. On 2026-10-16, `. ./env && make test` passed all 521 tests,
including the 14 F17 regressions. Earlier full-suite attempts failed because
TLS fixtures could not read their configured private key; the successful
rerun included all previously failing cases. No credential changes were made
by this remediation.

**Scope/limits:** GL calls are mocked; no real context, driver upload, visual
comparison, sanitizer, or 32-bit execution was performed. FreeType is still
responsible for the lifetime and actual allocation length of its returned
bitmap buffers. Concurrent mutation of a shared face is not made safe.
This remains a simple horizontal glyph/kerning renderer, not full text shaping.
GL failures after upload are reported but GPU contents are not rolled back.
Representability checks do not add a separate memory/time quota or change
the allocator's existing out-of-memory policy. F18–F23 remain separate work.

### F18 — Medium: PNG string encoding can enter a non-progressing write loop

**Locations (audited baseline):** `image/image.c:410–415,418–459`,
`libkc3/buf.c:230–236,1800–1860`.

`image_to_png_str` allocates an output buffer exactly as large as the raw
pixel data and installs no flush/growth callback. PNG headers and chunk
overhead can exceed that capacity. Its libpng callback calls `buf_write`
without inspecting the return value.

In `buf_write`, when a write fits the total capacity but not the remaining
space, the chunk loop can fill the buffer and then keep copying zero bytes.
`buf_flush` on a full buffer without a callback returns zero; the loop checks
only for a negative result and never advances `w`. A direct example is a
16-byte buffer already full followed by a one-byte write.

A 2x2 RGBA8 image supplies a concrete encoder path to this state: capacity
16, an 8-byte PNG signature, an 8-byte IHDR chunk header, then the 13-byte
IHDR payload. The callback stalls on that payload. The write sequence is
visible in [libpng's writer](https://raw.githubusercontent.com/pnggroup/libpng/libpng16/pngwutil.c).

**Impact/exposure:** a legitimate small image can hang its encoding thread,
and the generic writer has the same no-progress issue for other fixed-buffer
callers. This is distinct from F11's delimiter-copy flushing loop. Not every
capacity has the same failure mode; some take error paths instead.

**Suggested remediation:** make `buf_write` fail or otherwise terminate when
a flush cannot make space, keep buffer invariants on every branch, and provide
the PNG encoder with growable output whose failures are reported to libpng.

**Original check plan:** full/no-flush destination, zero-progress flush
callback, small PNG encodings including 2x2 RGBA, and incompressible images
whose encoded output is larger than their raw data.

**Remediation, 2026-10-16:** `buf_write` now checks lengths against `SW_MAX`
and validates buffer positions before doing subtraction-based capacity checks.
After flushing, it uses the actual available space, not a nonnegative return
value as evidence of progress. A full buffer with no space after flushing
returns failure. Each chunk advances the source position and appends at the
current write position, preserving bytes retained by a partial flush rather
than overwriting them. Negative flush results remain errors; zero-length
writes remain successful no-ops. The change also removes the oversized-write
branch that could advance `wpos` beyond capacity.

PNG string encoding starts with a small owned buffer and grows it geometrically
as libpng emits output. Output-length addition, growth, and raw row/image
arithmetic are checked. The encoder rejects invalid dimensions and layouts
outside its existing 8-bit, one-to-four-component format. The callback checks
the exact write count and reports allocation or short/failed writes through
`png_error`, reaching the encoder's cleanup handler. Custom output and error
handling follow the [libpng writing example](https://raw.githubusercontent.com/pnggroup/libpng/libpng16/example.c).

The mutable output buffer is heap-owned so its state remains valid after a
libpng `longjmp`. Rows are submitted directly, removing the separate row-pointer
allocation that could leak on a PNG error. Cleanup releases the output and
libpng objects on failure; `dest` is published only after successful completion.
On success, the exact encoded byte count and buffer ownership transfer into
the resulting string without another full-output copy.

**Regression tests:** fifteen cases in `test/buf_write_test.c` and
`test/image_test.c`, registered in the existing core runner. Test source lists
were regenerated with `./update_sources`; PNG tests are enabled when libpng
and libjpeg are detected. No standalone executable or probe was added.

- Generic buffer coverage includes full and zero-capacity fixed buffers,
  oversized writes, stalled callbacks reporting zero or positive results,
  partial flushes, error propagation, empty writes, and unrepresentable lengths.
  A 9,996-combination streaming matrix checks retained/output bytes and buffer
  guards across capacities, initial fill levels, write lengths, and drain sizes.
  A real file-backed buffer with an active saved position verifies a blocked
  flush terminates and the retained bytes can be flushed afterward.
- PNG coverage uses real libpng compression and decoding for 2x2 RGBA and
  grayscale, grayscale-alpha, RGB, and RGBA images at 1x1, 7x3, 32x32, and
  128x128. Deterministic high-entropy pixels produce outputs larger than the
  raw data, and decoded pixels must match byte for byte.
- Namespaced inclusion of the actual image implementation permits controlled
  allocation, short-write, and libpng failure injection without replacing
  production library symbols. Every output allocation and write callback in
  the large-image case is failed in turn. Header/row/end errors, libpng object
  creation failures, invalid image layouts, and output-length overflow are
  covered. Tracked buffers/allocations and libpng objects must be released,
  and the destination must remain unchanged. Child timeouts turn regressions
  into runner failures rather than hanging the suite.

**Validation:** all eight initial regression cases failed against the
original code, including timeouts for the full/zero-capacity buffers and the
2x2 PNG encoder. The final focused release and debug runners each pass 15/15.
With `. ./env` sourced, the root `make` completed, `make test` passed 536/536,
and `make test_debug` passed 528/528. Both full targets included all 15 new
F18 cases. `git diff --check` passed. Release-library links and the pre-test
`test/kc3.dump` were restored afterward; the generated dump and run logs were
retained under `/tmp/kc3-f18.jyR9Q6C5`. No commit was made.

**Scope/limits:** generic buffered writes are not transactional; a failure
can leave or have emitted an accepted prefix. Callbacks must still obey buffer
ownership/invariant contracts and return in finite time. Input image storage
must actually cover its declared dimensions; the struct has no allocation
length to verify. No sanitizer, 32-bit execution, exhaustive dependency audit,
or resource quota was added, and the allocator's existing out-of-memory policy
is unchanged. PNG file output, image decoding, and JPEG handling were not
changed by F18. F19–F23 remain separate work.

### F19 — Medium: malformed JPEG decoding terminates the host process

**Locations (audited baseline):** `image/image.c:101–151`,
`gl/gl_sprite.c:208–213`.

Both JPEG readers install `jpeg_std_error` and proceed to decode without
replacing `error_exit` or establishing a recovery path. The standard handler
destroys the JPEG object and calls `exit(EXIT_FAILURE)`; it does not return
NULL to KC3. This behavior is explicit in the
[libjpeg-turbo error-handler source](https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/main/src/jerror.c).

The image-file dispatcher only checks the leading JPEG signature before
entering that decoder. A file with that signature and a fatal JPEG structure
error can therefore terminate the embedding application, rather than fail
the image operation. Not every damaged JPEG is fatal; recoverable warnings
are a different path.

**Impact/exposure:** process-wide denial of service wherever these image APIs
are used on supplied files. `Image.new_resize_to_fill_file` reaches the reader
through its public binding. No specific unauthenticated web route was found
or exercised, and this is misuse of the dependency's default error policy,
not an asserted libjpeg vulnerability.

**Suggested remediation:** install a non-returning recovery handler using
the dependency's supported pattern, unwind decoder/file/image ownership, and
return an ordinary KC3 error. Apply it to both readers.

**Original check plan:** decode fatal malformed JPEGs in child processes
and verify the caller survives and receives failure; retain valid-image tests.

**Remediation, 2026-10-16:** `image_init_jpeg` installs its own non-returning
libjpeg error handler and establishes recovery before decoder creation. Fatal
errors now unwind to decoder cleanup and return NULL instead of terminating
the process. The decoder, error manager, jump buffer, and provisional image
are heap-owned so cleanup does not read automatic objects made indeterminate
by `longjmp`. This follows the error-recovery and object-lifetime requirements
described in the [libjpeg-turbo decompression example](https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/main/src/example.c).

Decoded pixels are retained privately until header processing, start,
scanlines, and finish all succeed. Failures destroy the decoder and release
temporary pixels/context without changing the caller's image. Width, height,
component count, and row/image multiplication are validated before allocating
KC3 pixel storage. Unexpected suspension/no-progress results return failure.
The supplied FILE remains owned by the caller, including after a JPEG error.

`gl_sprite_init_jpeg` now calls this shared decoder; the GL library already
links the image library, so no dependency was added. Its duplicate unprotected
libjpeg reader was removed. Decode failures close the opened file and release
the temporary path strings before returning, without uploading textures or
publishing a partial sprite. The image-file and sprite-file dispatchers
therefore reach the same protected decode path.

**Regression tests:** twelve additional cases in the existing
`test/image_test.c` core-runner fixture. The initial four tests reproduced the
process exit through the direct image reader, image dispatcher, direct sprite
reader, and sprite dispatcher. All four now return failure normally. The
seven F18 PNG cases remain enabled and passing. `./update_sources` was run;
no standalone executable or probe was added.

- Real libjpeg input includes an SOI/EOI stream with no image and a valid JPEG
  whose final EOI was replaced with a second SOI. The latter decodes all eight
  scanlines before reporting a fatal error; each of the four entry paths must
  release temporary pixel storage and preserve its destination.
- Fatal errors are injected before/after decoder creation, in header/start
  processing, on the first and a later scanline, and at finish. Allocation
  failure is injected for the context and pixel buffer. Header/start/scanline/
  finish suspension, invalid dimensions/components, and byte-count overflow
  are also covered. Repeated failures are followed by successful decodes in
  the same child process.
- Real baseline and progressive grayscale, RGB, and CMYK JPEGs are generated
  by libjpeg and decoded; dimensions/component layout and pixel values within
  JPEG rounding tolerance are checked. A missing EOI produces a warning but
  remains decodable, preserving libjpeg's existing nonfatal-warning policy.
- The RGB progressive sprite case checks captured RGBA pixels, vertical row
  order, alpha, dimensions, and successful cleanup. GL is mocked. File-open
  and path-search hooks use duplicated real temporary-file streams to track
  ownership; this is not a filesystem search or driver test. Tracked KC3
  allocations, files, strings, decoder objects, and textures must balance.
  Caller-owned streams must remain open after the direct image operation.

**Validation:** the baseline image target passed its seven PNG tests
and failed all four initial JPEG cases. The expanded release and debug image
targets each pass 19/19 (7 PNG + 12 JPEG). With `. ./env` sourced, the root
`make` completed, `make test` passed 548/548, and `make test_debug` passed
540/540. Both full targets included all twelve new JPEG cases.
`git diff --check` passed. Release-library links and the pre-test
`test/kc3.dump` were restored; run logs and the generated dump were retained
under `/tmp/kc3-f19.2aG8BPJQ`. Existing staged F18 work was preserved; no commit
was made by this remediation.

**Scope/limits:** fatal decoder errors are recovered; warnings are not promoted
to fatal errors. No libjpeg vulnerability audit, sanitizer, 32-bit execution,
or memory/time quota was added, and KC3's allocation-failure policy is unchanged.
No real GL context was used. Sprite geometry/color conversion and later GL
allocation/upload failure paths are not made safe by this decoder fix. PNG
decoding is unchanged. The resize CLI cleanup is addressed separately under
F20 below; F21–F23 are addressed separately below.

### F20 — High: image resize cleanup frees invalid storage on error paths

**Locations (audited baseline):** `image/resize/image_resize.c:20–27,43–65`,
`image/image.c:23–27`.

The CLI declares `src` and `dest` without initialization. If
`image_init_file(&src, ...)` fails, it immediately calls `image_clean(&dest)`
even though `dest` has never been initialized. That function reads
`dest.data` and conditionally passes the indeterminate pointer to `free`.
A readable file too short for the initial eight-byte header reaches this
failure path without depending on a decoder crash.

There is a separate fall-through in the same cleanup flow: failure of
`image_resize_to_fill` cleans both images but does not jump to `clean`.
Execution cleans `src` again and passes freed `dest` storage to the PNG writer.
`image_clean` does not reset the pointer, so this is a double-free/use-after-free
path, not an idempotent cleanup. Intermediate allocation failure is one way
the resize routine returns NULL after the two images have been allocated.

**Impact/exposure:** undefined native memory behavior on input/error handling
in the resize executable. This report does not establish that a network
service launches that utility or that an invalid free is exploitable.

**Suggested remediation:** zero-initialize owned objects, track ownership,
and use one terminal error-cleanup path with no continuation into image output.
Check dimension parsing and release an already-loaded source if destination
allocation fails.

**Remediation status (2026-10-17): fixed within the scope below.**

- `image/resize/image_resize.c` zero-initializes both images and routes every
  exit after successful KC3 initialization through one cleanup block. Each
  image is cleaned once; failures retain a nonzero status. Failed loading,
  destination allocation, or resizing cannot continue into PNG output.
  A loaded source is released when destination allocation fails, and usage
  errors also clean the initialized KC3 environment.
- Dimension arguments must parse completely, be nonzero, and fit `S32_MAX`.
  Existing decimal, binary, octal, and hexadecimal integer syntax is retained;
  trailing junk is rejected. The destination byte count is checked against
  `SW_MAX` before multiplication/allocation. This is a representability check,
  not a memory quota or a proof about intermediate resize geometry.
- `image_clean` itself is unchanged: callers still must not clean an owned
  image twice. No headers or core-library code were changed.

**Regression coverage:** ten cases added to the existing `test/image_test.c`
target; no standalone probe. The fixture includes the production CLI entry
point and existing namespaced image implementation in isolated child processes.
It uses real temporary input streams and JPEG decoding, allocation accounting,
and the production resize/crop control flow. KC3 initialization/cleanup and
file-access checks are hooked to avoid disturbing the test runner. AVIR is
stubbed; the output hook checks live pixel ownership and redirects successful
output to the production PNG string encoder for a libpng round trip.

- Initialization failure, wrong argument count, inaccessible input, and a
  readable five-byte input file.
- Failure of each of four KC3 allocations: JPEG context, source pixels,
  destination pixels, and the intermediate resize buffer. Each must return
  nonzero without leaks, invalid frees, or entering output.
- Resizer failure with both matching and differing aspect ratios, plus
  output failure after a successful resize. Cleanup counts and allocation,
  decoder, and file balances are checked.
- Empty, zero, signed, malformed, trailing-junk, and overflowing dimensions
  in either argument, plus destination byte-count overflow after loading.
- Successful same-aspect/crop paths and all four integer syntaxes, checking
  PNG dimensions and pixel contents as well as ownership cleanup.

**Validation:** the baseline failed seven of the ten new CLI cases, including
abnormal child termination on short input and an output call after failed
resizing. After the fix, release and debug image targets each pass 29/29.
`./update_sources` was run, and the test configuration was regenerated to
track the newly included CLI source. With `. ./env` sourced, root `make`
completed, `make test` passed 558/558, and `make test_debug` passed 550/550.
Both full suites included all ten new CLI cases. `git diff --check` passed.
Release-library links and the pre-test `test/kc3.dump` were restored; logs,
the generated dump, and the baseline crash dump were retained under
`/tmp/kc3-f20.EqYqLbBH`. No commit was made.

**Scope/limits:** the tests exercise the included CLI implementation, not a
separately launched resize executable or real AVIR resampling. No sanitizer,
32-bit execution, resource quota, atomic output replacement, or complete
PNG-decoder/file-writer audit was added. Generic image allocation arithmetic,
intermediate geometry calculations, AVIR internals, and KC3's allocation-failure
policy remain unchanged. F21–F23 are separate findings.

### F21 — Medium: the generic GTK signal trampoline has a fixed wrong ABI

**Locations (audited baseline):** `gtk4/signal.c:22–34,62–85`,
`lib/disabled/gtk4.kc3:26–27`.

`kc3_gtk4_signal_connect` accepts an arbitrary signal name but always connects
`void callback(GObject *object, s_tuple *tuple)`. That argument layout works
only for signals with no parameters and no return value. For example,
`notify` passes `(GObject *, GParamSpec *, user_data)`, as documented by
[GObject](https://docs.gtk.org/gobject/signal.Object.notify.html).
KC3 therefore reads the parameter-spec pointer as its tuple, while the real
tuple is in the third argument. The callback immediately dereferences the
misidentified object and can abort or perform invalid accesses. Return-valued
signals are also not represented by this `void` trampoline.

**Impact/exposure:** normal parameterized signal emission is unsafe if this
optional binding is enabled. The language module is currently under
`lib/disabled`, and the root default GTK build lines are commented out; this
is not presented as an enabled interpreter or HTTP attack path.

**Suggested remediation:** either validate and restrict supported signal
signatures or use signature-aware GClosure marshalling with correct parameter,
result, and callback-data ownership. A function-pointer cast does not adapt
the calling convention.

**Remediation status (2026-10-17): fixed by restricting supported signatures.**

- `gtk4/signal.c` resolves the signal on the actual object type and queries
  its parameter count, return type, and flags before allocating callback
  state. Only signals with zero parameters and `G_TYPE_NONE` return type can
  connect. Details are accepted only for signals with `G_SIGNAL_DETAILED`.
  This preserves the existing KC3 callback contract `(object, data)` rather
  than introducing argument/result marshalling. The queried metadata describes
  the callback ABI in [GObject's SignalQuery contract](https://docs.gtk.org/gobject/struct.SignalQuery.html).
- Parameterized signals, including `notify::property`, and return-valued
  signals produce a diagnostic and leave no handler installed. Unknown
  signals, invalid arguments, empty names, and embedded NULs are rejected.
  Accepted names are copied from their counted KC3 string to terminated
  storage before passing them to GObject; the copy's size is bounded before
  allocation. The public C/KC3 `Void` API is unchanged, so rejection is reported
  through diagnostics, not a new return status or handler ID.
- Successful connections use `g_signal_connect_data` with a correctly typed
  destroy notifier that deletes the owned tuple, callable reference/copy,
  and data tag. GObject releases that state after disconnection when it is no
  longer in use, including object destruction, per its
  [connection-data lifetime contract](https://docs.gtk.org/gobject/func.signal_connect_data.html).
  Name storage and already-constructed tuple fields are released on constructor
  failures; a failed copy is not installed as an owned tuple field.
- No headers, core-library code, language declarations, or default GTK
  enablement were changed.

**Regression coverage:** `test/gtk4_signal_test.c` adds eleven cases to the
existing C runner. `test/configure` detects GTK4 optionally; the cases run
when that dependency is present. Root and test manifests were regenerated
using their `./update_sources` scripts. Tests include the production signal
implementation with narrow ownership/failure-injection hooks, real GObject
signal connections/emissions, and real KC3 function evaluation. They run in
isolated children with a timeout and fatal GLib warnings/criticals; no display,
GTK initialization, GUI application, or standalone probe is used.

- A no-parameter void signal delivers the expected object and copied user
  data; explicit disconnection stops later calls and releases ownership.
- A supported detailed signal invokes only its matching detail.
- Real `notify::value` emission with a `GParamSpec`, an integer-parameter
  signal, and a Boolean-returning signal must not enter the KC3 callback.
- Unknown names, unsupported details, empty/embedded-NUL names, counted
  non-terminated input, null arguments, a void callable, and the name-size
  bound are checked.
- Injected name, tuple, callable-copy, and data-copy failures leave no handler,
  owned tuple/name, or extra callable reference.
- Object destruction, reference/copy ownership modes, and disconnection
  during an active callback are checked. The latter keeps the tuple alive
  during evaluation and releases it once the emission returns.

**Validation:** the baseline reproduced abnormal child termination for
`notify` and integer-parameter emission, invocation of an incompatible
return-valued callback, and retained tuples on disconnect/destruction.
The initial baseline run failed and reached the runner's error limit.
After the fix, the focused release and debug targets each pass 11/11.
The optional GTK library builds successfully in both modes with `. ./env`
sourced. Root `make` completed, `make test` passed 569/569, and
`make test_debug` passed 561/561; both full suites included all eleven new
cases. The existing optional-library configuration probes emitted nonfatal
linker diagnostics; neither build nor test failed. Whitespace checks passed
for the code and audit log. Release-library links and the pre-test
`test/kc3.dump` were restored. Logs, the generated dump, and the baseline
crash dump are retained under `/tmp/kc3-f21.tCRuaMZK`. During validation the
code was committed externally as `867dfe25d`; the agent created no commit
and left that commit untouched.

**Scope/limits:** this is a signature restriction, not a general GValue/KC3
marshaller. Parameterized and return-valued signals remain unsupported.
Callbacks and their teardown still require an appropriate live KC3 environment;
cross-thread dispatch and objects outliving KC3 shutdown were not made safe.
Existing abort behavior on callback evaluation/argument-allocation failure is
unchanged. No real GTK widgets, sanitizer, 32-bit run, or complete GTK binding
audit was added; independent simple-action and other binding paths are outside
this finding. F22–F23 are addressed below.

### F22 — Medium: C generation stalls on no-match input and masks parse failure

**Locations (audited baseline):** `to_c/to_c.c:25–48,114–120`,
`to_c/kc3_to_c/kc3_to_c.c:39–49`, `libkc3/buf_parse.c:4787–4833`.

The generator's outer loop does `continue` when `buf_parse_tag` returns zero.
The parser's no-match path restores its read position. If non-whitespace,
non-comment input remains that is not a tag, the next iteration sees the
same bytes and retries indefinitely. An unmatched closing delimiter is a
candidate regression input; no executable reproduction was run during the
original source-only audit.

When parsing instead returns a negative error, the loop breaks and returns
the accumulated nonnegative output count. The CLI treats that as success.
Thus distinguishing a malformed input from ordinary EOF is also missing.

**Impact/exposure:** malformed input to the separate C generator can hang
or produce a successful exit with incomplete output. This is not a claim
about `kc3c`'s cache compiler: `to_c_defmodule_tag` explicitly says generation
is not implemented, and the component is not part of the normal root build.
The unfinished status does not make its parser control flow safe.

**Status:** fixed on 2026-10-17 with regression tests in the existing C runner.
The C generator remains an optional, unfinished component.

**Remediation:**

- `to_c_buf` explicitly distinguishes EOF from refill/UTF-8 failure while
  consuming top-level whitespace and line comments, including a comment with
  no final newline. A remaining token must parse successfully and advance
  the read position. The progress check uses a buffer save so compaction does
  not invalidate the comparison. Zero/no-match and negative parser results
  now fail instead of retrying or returning accumulated output as success.
- Unsupported top-level tags/calls and unimplemented module-body generation
  return failure. The accepted `defmodule` call must have exactly two proper
  arguments. Zero-result output writes are also failures; the successful
  byte count now includes both module-guard writes, and the outer accumulated
  count is checked for overflow.
- `to_c_file` checks the input stream's error flag and close result. This
  catches stdio read errors even though the shared FILE refill callback
  returns zero for both EOF and read failure.
- The existing `to_c_close` declaration now has an implementation that checks
  both output flushes, stream error flags, and closes. The CLI requires it
  to succeed before exiting successfully. Cleanup zeros the context so the
  existing partial-open/error paths can safely clean it again. Replacing the
  module guard also releases its previous string.
- `test/to_c_test.c` is registered in `test/libkc3_test.c`; source manifests
  were regenerated with `./update_sources`. The optional component's test
  target now runs these regressions through `test/Makefile`, replacing the
  old HTTP-generation command that expected success from unimplemented
  module-body generation.

**Regression coverage:** thirteen cases exercise the real generator and
parser, with CLI `main` included in the existing runner and narrow hooks for
runtime initialization and file/failure boundaries. Temporary files supply
input/output storage; real FILE streams opened in the wrong mode exercise
read/write error flags. Close failures and parser/write return values are
injected. Each case runs in a child with a three-second alarm so non-progress
becomes a test failure, not a stuck suite.

- Empty input, whitespace, EOF comments, valid empty modules, exact generated
  bytes, and the combined C/header byte count.
- Unmatched closing delimiters, a truncated string, invalid/truncated UTF-8,
  unsupported input, and malformed trailing input after generated output.
- Parser no-match, negative failure, and positive-without-progress results;
  one-byte streaming refills ending in either EOF or error.
- Zero and negative results at each of the nine generation writes, actual
  input read errors, successful CLI cleanup, invalid-input CLI status,
  failures flushing either output, each input/output close failure, and
  partial output-open failure without double cleanup.

**Validation:** the baseline run reproduced child timeouts for EOF comments,
unmatched input, and malformed trailing input, plus falsely successful
truncated/unsupported input and refill/write failures. It also detected an
output-count mismatch (164 reported versus 170 bytes written). That run
reached the runner's error limit before executing every case. After the fix,
all thirteen focused cases pass in release and debug modes. The optional
`to_c` library and CLI build successfully in both modes with `. ./env`
sourced. Root `make` completed, `make -C to_c test` passed 13/13,
`make test` passed 582/582, and `make test_debug` passed 574/574. Both full
suites included all thirteen new cases. Optional-library configuration probes
emitted nonfatal linker diagnostics; neither build nor test failed.
Whitespace checks passed. Release-library links, the pre-test `test/kc3.dump`,
and the two tracked optional CLI binaries were restored after validation.
Logs, built binaries, the generated dump, and a crash dump from the corrected
debug regression fixture are retained under `/tmp/kc3-f22.OlRcgDq1`.
No commit was made by the agent.

**Scope/limits:** real module-body compilation is still unimplemented; only
the existing empty-module output skeleton is supported, and this fix does
not certify generated-program correctness. The root default build and
`kc3c` cache compiler are unchanged. Output replacement is not atomic: failed
runs can leave created, truncated, or partially written output files, but
return failure. Shared core parser assertions on other malformed syntax,
buffer-size limits, and broader parser/UTF-8/allocation behavior are outside
this fix. No sanitizer or 32-bit run was performed. F23 is addressed below.

### F23 — Medium: every forked environment loses its stacktrace allocation

**Locations (audited baseline):** `libkc3/env_fork.c:24–44,96–106`,
`libkc3/stacktrace.c:26–45`, `libkc3/kc3.c:2065–2069,2128–2134`.

`env_fork_init` allocates a new independent stacktrace with `stacktrace_new`.
`env_fork_clean` releases frames, operators and mutexes, but leaves the
stacktrace untouched. `env_fork_delete` then frees the environment containing
the sole owning pointer. `stacktrace_delete` exists, but no cleanup caller was
found. This is not a borrowed reference to the parent's stacktrace.

The regular `Thread.new` worker deletes its forked environment on completion,
so repeated successful thread creation/join accumulates one leaked stacktrace
allocation per fork. Replication connections and acceptor workers also use
this lifecycle. The root environment separately allocates a stacktrace in
`env.c:2142`; its cleanup should be checked in the same remediation.

**Impact/exposure:** persistent resource leakage in long-running applications
that create workers or reconnect peers. No exceptional condition is needed.
No allocation-tracking or memory-growth measurement was performed during the
original source-only audit.

**Status:** fixed on 2026-10-17 for owned environment stacktraces, with ten
regression cases in the existing C test runner.

**Remediation:**

- `env_fork_clean` deletes its owned stacktrace after cleaning the fork's
  frames and operators, then clears the pointer. It uses `stacktrace_delete`,
  preserving the existing empty-stack assertion and wait for pinned readers.
  Shared parent stacktraces and other borrowed parent resources are untouched.
- Fork initialization failures now use a common partial-cleanup path. It
  tolerates a missing operator table and releases initialized mutexes as well
  as frames/operators; the destination remains unchanged on failure.
- `env_clean` also deletes and clears the root/local environment's stacktrace
  after its other owned resources. Cleaning the default environment clears
  the thread-local current-environment pointer instead of leaving it pointing
  at the freed default environment.
- Root initialization's earlier return-without-cleanup paths now unwind.
  Before symbol/integer initialization, argument/stacktrace cleanup restores
  the parent environment and frees an owned default environment directly.
  Later failures use `env_clean`, which now tolerates missing facts,
  operators, argument strings, module path, and I/O buffers. FILE-buffer
  adapter initialization and facts allocation failures are checked too.
- `test/env_fork_test.c` is registered in `test/libkc3_test.c`; source
  manifests were regenerated with `./update_sources`. No headers or
  stacktrace atomic/read-pin implementation were changed.

**Regression coverage:** tests run in forked children with a thirty-second
alarm and use the existing `memleak` tracker after warming shared state.
The normal lifecycle cases call the linked production functions. Fault and
reader scheduling tests include the production fork/stacktrace sources with
renamed symbols and narrow test-only hooks; no standalone probes were added.

- Thirty-two stack-allocated fork init/clean cycles and thirty-two heap fork
  new/delete cycles return the tracked allocation count to zero, preserve the
  parent, and create independent empty stacktraces.
- Thirty-two real `kc3_thread_new`/join cycles returning a value and eight
  cycles with an evaluation error leave no tracked allocations. This checks
  the normal and failed-evaluation cleanup paths, not just direct destructors.
- Three root/local init/clean cycles verify each known stacktrace allocation
  is removed and the parent current environment is restored. This assertion
  is scoped to the stacktrace, not all root/global allocations.
- A real early argument-initialization failure (`argv[0]` equal to `/`),
  partial-environment cleanup, default-environment cleanup, and early failure
  while creating a default environment check pointer/resource cleanup.
- Injected environment allocation, frame-copy, operator-copy, read-time-frame,
  and stacktrace-allocation failures leave no tracked allocations or initialized
  fork mutexes; failed initialization does not overwrite the destination.
- With a reader pinned, a test-only scheduling hook observes deletion inside
  the existing reader-wait loop. Teardown must remain incomplete until the
  reader unpins, then join successfully and release all tracked resources.

**Validation:** all eight initial cases failed against the baseline. The
thread test found one retained allocation after its first joined worker;
fork/root cleanup retained the stacktrace pointer, partial/early-failure
checks terminated abnormally, default cleanup retained its current pointer,
fork-init failure left three mutexes initialized, and teardown did not reach
the reader wait. After the fix, the final ten focused cases pass in both
release and debug modes. The two additional cases cover failed thread
evaluation and default-environment initialization failure. Root `make`
completed, `make test` passed 592/592, and `make test_debug` passed 584/584.
Both full suites included all ten new cases and the existing stacktrace pin,
formatting, and stress tests. Optional-library configuration probes emitted
nonfatal linker diagnostics; neither build nor test failed. Whitespace checks
passed for the code and audit log. Release-library links and the pre-test
`test/kc3.dump` were restored; logs, the generated dump, and the baseline crash
dump are retained under `/tmp/kc3-f23.Dyj6cWSr`. No commit was made.

**Scope/limits:** teardown requires the owner to have unwound its calls and
stopped publishing the stacktrace to new readers. The existing reader count
protects already-pinned reads; it is not reference counting for saved raw
`Stacktrace.ptr()` values, which become invalid when their environment is
destroyed. In particular, `HTTPd.thread_stacktrace` stores borrowed pointers
in its facts database; those entries are not durable handles and must not be
dereferenced after worker exit. This remediation does not add registry
unpublication, managed handles, or a late-reader acquisition protocol.
Allocation tracking is scoped to these lifecycle operations, not a whole-
process leak audit or RSS measurement. General allocator/assert abort policy,
other root/global resource ownership, worker cancellation, and replication
shutdown races are not certified here. No sanitizer or 32-bit run was added.

## Validation and remaining work

- Every numbered finding has a concrete source path and stated preconditions.
  Original finding examples and schedules are static derivations;
  remediation tests and their results are identified separately above.
- External references establish dependency contracts only; they do not show
  that a particular installed binary or service was exploited.
- F12–F23 remediation and runtime validation are recorded above; closure is
  limited to each finding's documented scope and does not remove its limits.
- Full third-party source/advisory review, complete native window backends,
  all GTK bindings, generated-code correctness, and exhaustive database/
  concurrency analysis remain unfinished. This report must not be read as
  closing every coverage gap from the first audit.
- F12–F23 production/test code and Facts documentation were edited.
  No prior audit commit was reverted and no commit was made by the agent.
