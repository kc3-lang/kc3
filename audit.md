# KC3 v0.2

## libkc3 re-audit — 2026-10-18

**Baseline:** `295b496b6` (`audit`); tracked worktree clean at the start.
The earlier F01–F11 report and restored F12–F23 report were consulted to
avoid renumbering or presenting their documented fixes as new work.

**Original audit method:** source inspection, type/size arithmetic, ownership
tracing, and caller tracing. The original finding examples are static
derivations; no builds, tests, probes, sanitizers, or network sessions were
run during that review. OpenBSD manual pages were consulted for the
system-call contracts in F29/F31. F24–F25 were subsequently fixed and tested
using the existing `test/` infrastructure on 2026-10-18, with the F25
correction validated on 2026-10-19, as recorded below. F26 was subsequently
fixed and validated with focused and full-suite tests on 2026-10-19.
The F27 correction on 2026-10-19 preserves EOF completion and also addresses
F32; focused and full-suite tests pass.
F28 was subsequently fixed on 2026-10-19 with lifecycle, destructor,
securelevel, and thread-isolation regressions in the existing runner.
The initial unwind-protection addition was removed at the user's request;
focused release/debug tests and the full build/test suite pass after
the correction.
F29's POSIX socket-flush fallback has been corrected, with new regressions
registered in the existing test runner. Focused release/debug tests and
the full build/test suite pass.
F30's successful TLS flush now compacts its saved tail, with regressions
registered in the existing test runner. Focused release/debug tests and
the full build/test suite pass.
F31's FD/TLS flush exits now account for confirmed partial writes, with
regressions registered in the existing runner. Focused release/debug tests
and the full build/test suite pass.
F33's RPC and Base64 temporary buffers are now cleaned on ordinary returns,
with allocation-tracked and fault-injected regressions in the existing
runner. Focused release/debug validation and the full build/test suite
pass as detailed below.

This was a targeted fresh pass, **not an exhaustive audit of every line in
libkc3**. Main areas reviewed were buffer refill/UTF-8 and output adapters;
string operations, Base32/Base64 and shell escaping; array construction and
selected parsing/evaluation paths; environment teardown and Facts guards;
and RPC temporary-buffer ownership. Selected struct, frame, tuple, Cow,
queue, process-execution, and marshalling paths were also inspected. The
inventory contains 473 C/header files, including generated files; that is
not a reviewed-file count. All arithmetic operators/generated numeric
implementations, all parser branches, cross-thread ownership, every
marshalled type, and platform-specific backends remain incompletely covered.

For this review, High includes native memory-safety defects and a demonstrated
shell-escaping contract bypass with conditional command-injection impact.
Medium covers availability, data integrity, lifecycle/guard defects, and
accumulating leaks. Low covers ordinary functional failure. These priorities
do not imply that a deployed service exposes the affected path remotely.

### New finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F24 | High | UTF-8 decoder uses a stale pointer after refill compaction | Fixed; regression tests and full `make test` pass |
| F25 | High | Shell escaping returns an unquoted suffix after invalid UTF-8 | Fixed; regression tests and full `make test` pass |
| F26 | High | Base64 output-size arithmetic wraps before allocation | Fixed; regression tests and full `make test` pass |
| F27 | Medium | String substitution spins on malformed UTF-8 | Fixed with EOF handling preserved; regression tests and full `make test` pass |
| F28 | Medium | Environment cleanup permanently enables the teardown bypass | Fixed without unwind protection; regression tests and full `make test` pass |
| F29 | Medium | Socket flush fallback defeats SIGPIPE suppression | Fixed; regression tests and full `make test` pass |
| F30 | Medium | TLS flush does not move the saved unwritten tail | Fixed; regression tests and full `make test` pass |
| F31 | Medium | FD/TLS flush errors forget partial-write progress | Fixed; regression tests and full `make test` pass |
| F32 | Low | Substitution sizing omits an unmatched trailing suffix | Fixed with F27 EOF correction; regression tests and full `make test` pass |
| F33 | Medium | RPC and Base64 discard temporary buffers without freeing their locks | Fixed; regression tests and full `make test` pass |

### F24 — High: UTF-8 decoding uses a pointer invalidated by compaction

**Locations (audited baseline):** `libkc3/buf.c:633–728`,
`libkc3/buf.c:1505–1592`.

`buf_peek_character_utf8` first requests one byte and stores
`b = buf->ptr.p_pu8 + buf->rpos`. For a multibyte leading byte it then calls
`buf_refill` for two, three, or four bytes, but continues reading `b` without
recomputing it. That refill can call `buf_refill_compact`, moving unread
bytes and changing `rpos`.

A valid eight-byte streaming buffer can have `rpos = 7`, `wpos = 8`, with
the first byte of U+00A2 (`C2 A2`) at offset 7. With no older save pinning
the data, the second refill moves `C2` to offset 0 and receives `A2` at
offset 1. The decoder still reads `b[1]` at offset 8: one byte past the
buffer. Three- and four-byte sequences have the same defect. When the stale
address remains inside the allocation, it can instead decode unrelated
bytes or reject valid input.

**Impact / preconditions:** out-of-bounds reads and incorrect decoding of
otherwise valid streamed UTF-8. File/socket-backed buffers can reach this
state through normal consumption and a split character; forged buffer
metadata is not required. A read-only buffer, or a save that prevents this
compaction, does not trigger this particular path. This is not a claim of
arbitrary writes or a demonstrated remote exploit.

**Suggested remediation:** derive the character pointer from the current
buffer position after each potentially compacting refill, then validate and
decode only the bytes whose availability was established.

**Suggested regression:** extend `test/buf_test.c` using its refill-callback
pattern. Split two-, three-, and four-byte characters at the allocation end;
check both peek/read, returned character, consumed position, short input,
and saved-position cases. Ordinary contiguous-string UTF-8 tests miss this.

**Status:** fixed on 2026-10-18 with two regression cases registered in the
existing `test/buf_test.c` suite.

**Implementation:** after each successful two-, three-, or four-byte refill
and availability check, `buf_peek_character_utf8` refreshes `b` from the
current `buf->ptr` and `buf->rpos` before accessing the character bytes.
The production change is three pointer assignments. Character-read return
values, locking, and save/compaction semantics are otherwise unchanged.
`buf_read_character_utf8` receives the fix through its existing peek call.
No headers, comments, source manifests, or unrelated implementation paths
were changed; the tests extend an existing source file.

**Regression coverage:**

- `buf_character_utf8_refill` covers 160 combinations: two-, three-, and
  four-byte characters split at every internal boundary, varying refill
  chunk sizes, direct peek/read calls, no save, a save at the current
  position, an older save permitting partial compaction, and a pinned save
  preventing compaction. It checks the decoded value, positions, retained
  bytes, save restoration, and untouched storage beyond the buffer capacity.
- `buf_character_utf8_refill_error` covers 48 combinations of truncated
  sequences, invalid bytes at every continuation position, refill errors,
  and peek/read calls. It checks error/no-match results, unchanged output
  character, and no logical input consumption after compaction.
- Backing storage includes controlled padding beyond the logical buffer
  boundary. This makes stale reads fail deterministically without requiring
  a crash or an actual allocation over-read in the pre-fix test run.

**Validation:**

- Before the production fix,
  `. ./env && make -C test build && KC3_TEST=buf make -C test test_libkc3`
  failed both new cases: a valid split character returned 0 instead of 2,
  and an invalid continuation returned 2 instead of 0. The buffer suite
  reported 37 passed, 2 failed.
- `. ./env && make` passed after the fix.
- `. ./env && KC3_TEST=buf make -C test test_libkc3 test_libkc3_debug`
  passed all 39 buffer tests in both release and debug builds.
- `. ./env && make test` passed: 469 C tests, plus all IKC3, KC3S,
  event-poll, socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** ASan is disabled in this checkout's configuration and was not
run; no 32-bit run was performed. This fix addresses stale pointers across
refill compaction, not a complete Unicode-validity or parser audit.
F25–F33 are addressed below.

### F25 — High: malformed UTF-8 bypasses shell escaping

**Locations (audited baseline):** `libkc3/sh.c:48–64,80–92`,
`libkc3/str.c:1711–1774,1831–1841`, `lib/kc3/0.1/sh.kc3:15–19`.
Example consumer: `lib/kc3/0.1/kpkg.kc3:625–639`.

The first `sh_escape` pass reads characters while
`str_read_character_utf8(...) > 0`. Invalid UTF-8 returns zero, or a
negative value for a truncated sequence, without consuming the offending
bytes. The loop treats this like successful completion. If the decoded
prefix contained only allowed characters, `escape` remains false and the
function copies the **entire original string**, including the unchecked
suffix, without quotes or escaping.

For example, the bytes `61 FF 3B 69 64` (`a`, invalid byte, `;id`) return
unchanged. The semicolon is never examined. If a caller interpolates this
result into a shell command and the shell accepts the non-UTF-8 byte as
part of a word, the suffix is shell syntax rather than argument data.
The library actually uses this escape-then-`/bin/sh -c` pattern in Kpkg.
If a disallowed character precedes the malformed byte, the second pass
instead silently drops the malformed byte and the remainder of the input.

**Impact / preconditions:** conditional command injection wherever an
application relies on `Sh.escape` for attacker-controlled byte strings
passed to a shell. This does not affect direct argv execution without a
shell, and is not evidence of a separate privilege boundary in Kpkg, which
already runs package build scripts. No command or exploit was executed.

**Suggested remediation:** either implement byte-oriented shell quoting
with an explicit embedded-NUL policy, or reject malformed input before
returning any successful result. Do not interpret decoding failure as EOF
or return an unchecked suffix. Preserve complete valid input.

**Suggested regression:** extend `test/ikc3/sh.kc3` and/or the existing C
runner with invalid-leading/continuation bytes before shell metacharacters,
after an allowed prefix, and after a character requiring quotes; include
truncated sequences, embedded NUL, and valid multibyte text. Assert rejection
or safe complete escaping without needing to invoke a shell.

**Status:** fixed, with the corrected implementation validated on
2026-10-19. Seven regression cases in `test/sh_test.c` are registered in the
existing C runner; focused release/debug and full-suite tests pass.

**Implementation:** the earlier byte-wise attempt was reverted. It changed
Unicode policy membership and could insert backslashes between the bytes of
a multibyte character. `Sh.allowed` and `Sh.reserved` retain their original
character-set semantics; source traversal and output remain character-based.

The first pass now runs until all source bytes are consumed and rejects any
decode failure instead of treating it as EOF. Since the shared string
decoder also accepts some noncanonical sequences, `sh_escape` additionally
checks that the consumed length equals `character_utf8_size(s_c)` and rejects
surrogate code points. This rejects malformed/truncated sequences, overlong
encodings, surrogates, and out-of-range code points. Embedded NUL is also
rejected because it cannot be represented in a shell command argument.
These failures return NULL without changing the destination. Validation
covers the entire input before either the unquoted-copy or quoted-output
path can succeed. The emitting pass also explicitly rejects decode failure.

Valid non-NUL UTF-8 retains the existing quoting and escaping behavior,
including when the policy sets contain multibyte characters. The default
ASCII policy is unchanged. Validation is local to `sh_escape`; the shared
UTF-8 decoder was not changed. No header or code comments were added.
Root and test source manifests were generated with `. ./env && ./update_sources`.

**Regression coverage:**

- `sh_escape_valid`: 26 exact-output cases covering empty/safe strings,
  shell metacharacters, whitespace/newlines, quotes, backslash, and valid
  UTF-8, including encoding-length boundaries, both sides of the surrogate
  range, and U+10FFFF.
- `sh_escape_unicode`: one string containing every non-NUL Unicode scalar
  value (1,112,063 code points), compared byte-for-byte against its expected
  default-policy output. This includes valid noncharacters; it is exhaustive
  scalar-value coverage, not an enumeration of every possible string.
- `sh_escape_allowed`: five cases with two-, three-, and four-byte policy
  characters, including a distinct character composed of bytes that appear
  in other allowed characters. Membership is by whole character, not byte.
- `sh_escape_reserved`: eight cases checking one backslash per reserved
  character and no escaping of distinct characters sharing UTF-8 bytes.
  Policy tests restore the original module value before checking results.
- `sh_escape_malformed`: 36 rejected inputs with invalid leading and
  continuation bytes, truncated sequences, overlong encodings, surrogate
  encodings, and out-of-range code points. Inputs include allowed prefixes,
  prefixes already requiring quotes, and following shell metacharacters;
  each rejection checks unchanged destination metadata and contents.
- `sh_escape_bytes`: all 255 non-NUL byte values with three different
  prefixes and a suffix containing shell metacharacters (765 combinations).
  The 381 ASCII combinations check complete output, length, and terminating
  NUL; the 384 isolated non-ASCII combinations check rejection and unchanged
  destination.
- `sh_escape_nul`: six placements of NUL, including after malformed bytes,
  checking rejection and unchanged destination metadata/content.
- These tests call the C API and compare byte strings. They do not execute
  the input or escaped output as shell commands.

**Validation:**

- Against the superseded byte-wise implementation, the expanded focused
  suite reported three passes and four failures. The allowed-policy case
  returned two unquoted bytes instead of four quoted bytes; the reserved
  case returned six bytes instead of five by escaping individual UTF-8
  bytes. Malformed-input and isolated-byte rejection also failed.
- `. ./env && make -C libkc3 all && KC3_TEST=sh make -C test test_libkc3 test_libkc3_debug`
  passed after the correction: all seven cases in release and debug builds.
- `. ./env && make` passed.
- `. ./env && make test` passed: 476 C tests and all IKC3, KC3S, event-poll,
  socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites. This includes
  the unchanged existing `test/ikc3/sh.kc3` compatibility fixture.
- `git diff --check` passed. No commit was made.

**Limits:** malformed byte strings are deliberately rejected, not supported
as non-UTF-8 filenames. Preserving Unicode policy semantics does not make
unsafe custom policies or arbitrary shell command construction safe. No
locale-dependent shell execution, multi-GiB allocation, sanitizer, or 32-bit
testing was performed. ASan remains disabled in this checkout. F26–F33 are
addressed below.

### F26 — High: Base64 allocation size wraps before the output loop

**Locations:** `libkc3/base64.c:82–125`, `libkc3/types.h:695–699`,
`libkc3/str.c:459–468`, `lib/kc3/0.1/base64.kc3:17`.

`s_str.size` is `u32`. In
`dest_size = ((str->size + 2) / 3) * 4`, the right-hand arithmetic is
performed in the 32-bit unsigned type before assignment to `uw`.
Declaring `dest_size` wide does not prevent this overflow. The allocation
helper also accepts only a `u32` size, while the output index and loop
continue across the full input.

For a valid input containing `3 * 2^30` bytes, the mathematical output size
is `2^32`. The expression wraps to zero. `str_init_alloc(&tmp, 0)` succeeds
with a one-byte allocation for the terminator, then the first input group
writes four bytes through `d`. Subsequent groups keep writing beyond it.

**Impact / preconditions:** heap buffer overflow when encoding a sufficiently
large, genuinely backed string, most realistically on a 64-bit host. This
requires multi-GiB input and is not a small malformed-packet trigger. The
input length itself fits the existing `u32` string representation.

**Suggested remediation:** check the expansion against the representable
string/allocation limit before arithmetic or allocation, including the
terminating byte. Widening only the local variable or intermediate product
is insufficient if the result is then narrowed at `str_init_alloc`.

**Suggested regression:** add boundary checks to the existing C runner for
the largest accepted input length and the first rejected padded output
size, plus ordinary empty/one-/two-/three-byte encoding. Test a checked-size
helper or rejection before input access rather than requiring a multi-GiB
allocation in the normal suite.

**Status:** fixed on 2026-10-19, with three regression cases in
`test/base64_test.c`, registered in the existing C runner. Focused release
and debug tests and the full suite pass.

**Implementation:** `base64_encode` rejects input lengths greater than
`(U32_MAX - 1) / 4 * 3` before evaluating the expansion, allocating output,
or reading input bytes. The largest accepted length is 3,221,225,469 bytes
(`0xBFFFFFFD`); its padded output is 4,294,967,292 bytes (`0xFFFFFFFC`),
with a 4,294,967,293-byte allocation including the terminator. The next
input length needs 4,294,967,296 output bytes and is rejected with NULL,
leaving the destination unchanged.

Within the accepted range, both `str->size + 2` and the multiplied result
fit `u32`. The conversion at `str_init_alloc` cannot truncate, and its
`size + 1` cannot wrap. This does not depend on widening `uw`. Empty input
and ordinary Base64 output are unchanged. No public API, headers, or code
comments were added. Root/test source manifests were generated with
`. ./env && ./update_sources`.

**Regression coverage:**

- `base64_encode_vectors`: 15 exact-output examples using the linked public
  API, covering empty input, every padding class, multiple groups, embedded
  NUL, high-bit bytes, and both `+` and `/`. Checks include output length,
  the nonempty output terminator, the existing empty-string representation,
  and unchanged source metadata.
- `base64_encode_size_boundaries`: 13 allocation-size checks, including
  lengths one through six and the three final padding groups below the
  limit. The largest accepted input requests exactly `0xFFFFFFFC` bytes;
  the allocation size plus terminator fits `u32`. Simulated allocation
  failure leaves the destination unchanged.
- `base64_encode_size_overflow`: eight lengths from the first rejected
  size through `U32_MAX`, including both multiplication-wrap and
  addition-wrap boundaries. Each returns NULL without calling the output
  allocator or changing the destination.
- Size tests use the existing suite's source-inclusion/allocation-hook
  pattern. They compile the production implementation under test-local
  names and replace `str_init_alloc` with a recording stub that returns
  NULL. The source pointer is NULL, so no input access is possible on the
  successful test paths. No multi-GiB allocation, overflow write, standalone
  probe, or external command execution is needed.

**Validation:**

- Before the production fix, the focused suite reported two passes and one
  failure: ordinary vectors and accepted sizes passed, while the first
  oversized input reached the allocator instead of being rejected.
- `. ./env && make -C libkc3 all && KC3_TEST=base64 make -C test test_libkc3 test_libkc3_debug`
  passed after the fix: all three cases in both release and debug builds.
- `. ./env && make` passed.
- `. ./env && make test` passed: 479 C tests and all IKC3, KC3S, event-poll,
  socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** large-size tests verify the checked boundary and allocation
request without allocating the full buffers or encoding multi-GiB input.
No sanitizer or 32-bit runtime testing was performed. Base64 decoding,
Base32 behavior, and temporary-buffer cleanup were unchanged by F26.
F27–F33 are addressed below.

### F27 — Medium: substitution makes no progress on invalid UTF-8

**Locations:** `libkc3/str.c:1405–1428`, `libkc3/buf.c:633–735,907–928`,
`lib/kc3/0.1/str.facts:45–47`.

`str_init_subst_size`, called before every substitution, advances by a
matched search string or by one decoded UTF-8 character. On a non-match it
rejects only a negative character-read result. A malformed leading byte
such as `FF` produces zero, leaves `in.rpos` unchanged, and adds zero to
the calculated size. The same loop condition and byte are then retried
forever.

A one-byte source `FF`, search string `"a"`, and replacement `"b"` suffice.
The search fails, the UTF-8 read returns zero, and the size pass never
returns. This is distinct from the stale-pointer/streaming issue in F24:
the substitution buffer is read-only and no compaction is needed.

**Impact / preconditions:** unbounded CPU use when `Str.subst` receives
malformed bytes not matched by the search string. Whether this is remotely
reachable depends on application use of substitution on external data.

**Suggested remediation:** define whether substitution is byte-based or
UTF-8-only, and ensure every successful iteration consumes input. Reject
zero/negative decoding results while input remains, or consume bytes under
the chosen byte-oriented policy. Apply the same policy to the emitting pass.

**Suggested regression:** add malformed-leading/continuation and truncated
sequences to `test/str_test.c`, using the existing suite's bounded child-test
pattern for potentially hanging cases. Check both matched malformed bytes
and unmatched malformed bytes, valid text, and the empty-search policy.

**Status:** corrected on 2026-10-19, with seven regression cases added to the
existing `test/str_test.c` suite. Focused release/debug tests and full-suite
validation pass.

**Implementation:** the initial fix incorrectly collapsed zero and negative
character-read results into the same error path. In these fixed input
buffers, a negative return indicates EOF/insufficient remaining bytes,
including an incomplete final UTF-8 character; it must allow normal
completion. Both passes now break on a negative read result. The size pass
includes all remaining bytes, and the emitting pass copies that same tail
unchanged. EOF does not itself cause substitution to fail.

A zero result still means no character was consumed. The size pass cleans
its input buffer and returns -1, and the emitting pass uses error cleanup
and returns NULL. This prevents the F27 retry loop without changing the
EOF contract. Constructor rejection leaves the destination unchanged.
Empty searches retain their explicit rejection policy, independently of EOF.

Substitution still tries a literal search match first and otherwise advances
by one UTF-8 character. It was not converted to byte-by-byte scanning, and
matched malformed byte sequences can still be replaced. Valid UTF-8 search
and replacement behavior, embedded NUL, and empty replacements are preserved.
No public API, headers, code comments, source files, or manifests were added.

**Regression coverage:** all seven cases use the existing bounded-child
pattern (`fork`, a three-second alarm, and `waitpid`) so a reintroduced loop
fails the test instead of hanging the runner.

- `str_init_subst`: 15 exact-size/output cases covering empty input,
  no match, expansion, contraction, deletion, repeated/complete matches,
  newlines, NUL, and two-/three-/four-byte UTF-8. Searching for a continuation
  byte does not start matching inside an unmatched valid character.
- `str_init_subst_eof`: 15 successful completion cases, including empty
  input, a match at EOF, complete UTF-8, every incomplete two-/three-/four-byte
  ending, and tails after ASCII, UTF-8, or substituted prefixes. Checks both
  the computed size and exact preservation of the unconsumed tail.
- `str_init_subst_tail`: 12 cases covering F32's no-match and matched-prefix
  examples, source shorter than search, empty source, deletion, expansion,
  complete/repeated matches, UTF-8, and raw NUL/malformed trailing bytes.
- `str_init_subst_matched_bytes`: eight cases preserving replacement or
  deletion of complete malformed-byte matches, including truncated sequences,
  and existing raw malformed-byte replacement output.
- `str_init_subst_size_malformed`: 16 no-progress examples plus all
  128 high-bit byte values followed by three ASCII bytes, ensuring enough
  input to distinguish zero-result decoding failures from EOF. Covers
  invalid leading and
  continuation bytes, failures after ASCII/UTF-8 prefixes, and failures after
  successful matches. Every case must promptly return -1.
- `str_init_subst_malformed`: the same 144 inputs through the constructor,
  checking NULL and unchanged destination metadata/content independently of
  direct size-function assertions.
- `str_init_subst_empty_search`: five empty, ASCII, UTF-8, malformed, and
  truncated sources, checking rejection by both APIs and unchanged destination.

**Validation:**

- Before the production fix, valid and matched-byte cases passed. The
  malformed sizing, malformed constructor, and empty-search cases failed
  under their child-process deadlines; the focused run took about nine
  seconds and reported 13 passes and three failures.
- Before the EOF correction, the expanded focused suite reported 16 passes
  and two failures: an incomplete final character returned size -1 instead
  of 1, and the F32 no-match example returned size 2 instead of 3. Tests that
  previously required rejection of EOF tails were corrected to require
  successful, byte-preserving completion.
- `. ./env && make -C libkc3 all && KC3_TEST=str make -C test test_libkc3 test_libkc3_debug`
  passed after the correction: all seven new cases in both builds, with 18
  total release tests and 21 total debug tests passing.
- `. ./env && make` passed.
- `. ./env && make test` passed: 486 C tests and all IKC3, KC3S, event-poll,
  socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** this fixes progress and error handling, not strict validation of
all source/search/replacement bytes. Shared decoder acceptance rules and
literal matching remain unchanged. Unconsumed EOF/short-suffix bytes are
preserved, not validated or discarded. The same tail-count correction fixes
F32 as documented below. No sanitizer, large-size arithmetic, or 32-bit
runtime testing was performed. F28–F31 and F33 are addressed below.

### F28 — Medium: teardown mode survives after a nested environment is gone

**Status:** fixed on 2026-10-19 without unwind protection; focused
release/debug regressions and the full build/test suite pass.

**Locations (audited baseline):** `libkc3/env.c:498–499,522–529,569–575,1847–1857,2154–2169`,
`libkc3/facts.c:2115–2120,2155–2160,2335–2340`,
`libkc3/data.c:403–411`.

Before the fix, `env_cleaning` held a process-global static boolean. Passing
true set it permanently; passing false only read it. `env_clean` set this
flag when cleaning the current environment, but never restored it when
returning to `parent_env`. `env_init`/`env_global_set` explicitly support installing an
environment over the current one, and the existing environment tests use
local init/clean cycles.

Consequently, initializing and cleaning a nested environment leaves the
surviving parent in permanent teardown mode. Afterwards the
`!env_cleaning(false)` condition is false in the Facts removal guards, so
they no longer reject modifications to the environment database at
`securelevel > 1`. Separately, `data_clean` returns before custom struct
cleanup whenever this flag is set, suppressing normal destruction in the
still-running environment.

**Impact / preconditions:** bypass of those specific Facts mutation guards
and skipped custom-type cleanup after a supported environment lifecycle.
No concurrent schedule or attacker-written native memory is needed. This
does not establish that securelevel is a complete sandbox or that an
untrusted network client can itself create a nested environment. It is
separate from F23's owned stacktrace fix.

**Suggested remediation:** make teardown state scoped to the environment
and cleanup operation, preserving/restoring enclosing state on every exit.
Guard exemptions should identify the database actually being destroyed;
unrelated live environments must not inherit them. Account for threading
and nested cleanup instead of just adding an unsynchronized global reset.

**Suggested regression:** extend `test/env_test.c`/Facts coverage with a
nested init/clean followed by a parent custom destructor and an attempted
protected database removal. Isolate irreversible securelevel changes in a
child process using the established test infrastructure.

**Implemented:**

- Replaced the process-global boolean with a thread-local pointer to the
  environment in teardown. `env_cleaning(false)` is true only when that
  environment is current. The existing public function signature and
  environment layout are unchanged.
- `env_clean` saves the enclosing teardown context and restores it before
  returning to the parent environment. A nested live environment therefore
  does not inherit teardown mode; cleaning it restores the enclosing
  cleanup's state. There is no unwind protection in `env_clean`; the
  initial addition and its regression were removed at the user's request.
- The three Facts removal guards retain their existing database-identity
  comparison. Their exemption now requires that the current environment is
  the one being cleaned; returning to a live parent no longer disables
  its guards. Legitimate environment deletion at securelevel 2 remains
  permitted.
- Added four cases to `test/env_test.c`: repeated parent/child/grandchild
  lifecycles; parent custom-data destruction after child teardown; a
  destructor that starts and cleans another environment while checking
  enclosing-state restoration and another thread's state; all three
  protected removal entry points plus permitted teardown at securelevel 2.
- Tests run in bounded child processes, isolate irreversible securelevel
  changes, and disable core dumps for expected guard aborts. No standalone
  probe, new source file, header change, or code comment was added.

**Validation:**

- Before the production fix, the new lifecycle check observed teardown
  mode remaining enabled and the protected-removal check observed that
  removal did not raise the expected `SIGABRT`.
- After removing unwind protection and its regression,
  `. ./env && KC3_TEST=env make -C test test_libkc3 test_libkc3_debug`
  passed: all 18 environment tests in each build, including the four
  retained regression cases.
- `. ./env && make` passed after the correction.
- `. ./env && make test` passed after the correction: 490 C tests and all
  IKC3, KC3S, event-poll, socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and
  crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** this scopes the existing teardown exception; it does not make
securelevel a complete sandbox or make arbitrary simultaneous destruction
of a shared environment safe. Cleanup callbacks must return normally;
this change adds no nonlocal-exit recovery to `env_clean`. No sanitizer,
thread-sanitizer, or 32-bit runtime testing was performed. F29–F31 and F33
are addressed below.

### F29 — Medium: the socket error fallback reintroduces SIGPIPE

**Status:** fixed on 2026-10-19; focused release/debug regressions and the
full build/test suite pass.

**Locations (audited baseline):** `libkc3/buf_fd.c:185–235`,
`libkc3/buf_rw.c:37–50`, `libkc3/socket_buf.c:133–163`.

Before the fix, `buf_fd_open_w_flush` first used `send(..., MSG_NOSIGNAL)`,
but fell back to `write` after **every** negative result, not just the
non-socket case. For
a socket shut down for writing, the first call can return `EPIPE` while
suppressing `SIGPIPE`; the fallback repeats the operation without that
suppression. The suppression and error semantics are documented by
[OpenBSD send(2)](https://man.openbsd.org/send.2) and
[write(2)](https://man.openbsd.org/write.2). The default action of SIGPIPE
is process termination. [OpenBSD signal(3)](https://man.openbsd.org/signal.3)

**Impact / preconditions:** a disconnected peer can cause the host process
to terminate on a subsequent output flush when SIGPIPE has its default
disposition. `Socket.Buf` uses this adapter. Some Facts connection/acceptor
paths explicitly ignore SIGPIPE (`facts.c:256`,
`facts_connection.c:409,621`); those paths can mask the symptom, but generic
socket-buffer users and embedders cannot rely on having run them.

**Suggested remediation:** distinguish sockets from ordinary descriptors,
or limit the ordinary `write` fallback to an actual non-socket error.
Preserve signal suppression for socket retries and handle transient errors
without switching to an unsafe operation. Avoid changing process-wide
signal policy as an incidental library side effect.

**Suggested regression:** in the existing C runner, use an isolated child
with default SIGPIPE disposition and a closed peer socket. Flushing should
report failure, not terminate the child. Retain ordinary-file and successful
socket-write coverage.

**Implemented:**

- On POSIX, `buf_fd_open_w_flush` calls `write` only when `send` fails with
  `ENOTSOCK`. Socket failures no longer switch to a call without
  `MSG_NOSIGNAL`; they use the existing error path or, for
  `EAGAIN`/`EWOULDBLOCK`, the existing poll/retry path.
- Kept the Windows branch's existing fallback unchanged. No signal
  handler, signal mask, public API, or header was changed.
- Added `test/buf_fd_test.c` and registered its `buf_fd` target in the
  existing C runner. Closed-peer and write-shutdown cases run in bounded
  child processes with default, unblocked SIGPIPE. They require a negative
  flush result, retained buffer contents after the immediate failure, safe
  adapter close, and unchanged signal disposition/mask.
- Three compatibility cases verify binary output through a connected
  socket, a pipe, and a regular temporary file, including no extra output.
- Regenerated root/test source manifests with `./update_sources` and
  refreshed the test build configuration. No standalone probe or code
  comment was added.

**Validation:**

- Before the fix, both socket-failure regressions terminated with signal
  13 (`SIGPIPE`) in release and debug builds. The three compatibility
  cases passed in both builds.
- `. ./env && KC3_TEST=buf_fd make -C test test_libkc3 test_libkc3_debug`
  passed after the fix: all five cases in both release and debug builds.
- `. ./env && make` passed.
- `. ./env && make test` passed: 495 C tests and all IKC3, KC3S, event-poll,
  socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** the new failure regressions use local Unix stream socket pairs.
No native Windows, sanitizer, or 32-bit testing was performed. Ordinary
pipe errors retain `write`'s existing signal behavior. Interrupted-call
handling, poll timeouts, and partial-write accounting were not redesigned;
F30–F31 and F33 are addressed below.

### F30 — Medium: successful TLS flush loses a saved suffix

**Status:** fixed on 2026-10-19; focused release/debug regressions and the
full build/test suite pass.

**Locations (audited baseline):** `libkc3/tls_buf.c:127–175`,
`libkc3/buf_save.c:26–66`. Compare `libkc3/buf_fd.c:239–246`.

The TLS adapter flushes only `buf_save_min_wpos(buf)` bytes, respecting
saved write positions. Before the fix, on success it reduced `buf->wpos`
and the saved positions, but never moved the unwritten suffix to the start
of the buffer.
The FD adapter performs this required `memmove`.

For buffered `"AB"`, `wpos = 2`, and a save taken after `"A"` (`wpos = 1`),
the TLS flush sends `A` and changes the current write position to 1. The
remaining logical byte should be `B`, but offset 0 still contains `A`.
Releasing the save and flushing again sends `A` a second time and loses `B`.
This requires no short write or I/O error.

**Impact / preconditions:** silent output corruption when a TLS-backed
buffer flushes a nonempty prefix while retaining bytes after an active
save. A flush of the entire buffer with no saved tail is unaffected; this
report does not claim that every TLS protocol call uses this pattern.

**Suggested remediation:** compact the unsent suffix before adjusting
positions, preserving the same buffer-save invariant as the FD adapter.

**Suggested regression:** exercise the actual TLS flush adapter through the
existing C test runner with saves at the beginning, middle, and end of the
buffer, including nested saves. Assert the complete emitted byte sequence
and retained buffer contents, not only a successful return value.

**Implemented:**

- On a successful flush, `tls_buf_open_w_flush` moves the unwritten suffix
  to the front with `memmove` before reducing buffer and saved write
  positions, matching the FD adapter's existing successful-flush behavior.
  Empty/full flushes and their return values are unchanged. Error exits
  were unchanged by F30 and are addressed separately in F31 below.
- Added `test/tls_buf_test.c` and registered its `tls_buf` target in the
  existing C runner. It follows the existing source-inclusion testing
  pattern: the actual adapter implementation is compiled under test-local
  names, with only the libtls transport calls replaced. The write stub
  captures exact bytes and supplies deterministic successful short writes;
  it does not duplicate the flush algorithm or replace generic `buf_flush`.
- Four cases cover the original `AB` example; all eight save positions in
  a seven-byte binary payload with write chunks of one through seven bytes;
  nested saves, including restoring a saved write position; and empty/full
  flushes without saves. They assert emitted bytes, retained contents,
  buffer/save positions, save links, and no duplicate output on repeated
  flush or close.
- Regenerated root/test source manifests with `./update_sources` and
  refreshed the test build configuration. No standalone probe, header
  change, or code comment was added.

**Validation:**

- Before the fix, the saved-tail, boundary, and nested-save cases failed
  in release and debug builds. The original example retained `A` instead
  of `B`; the nested case retained `ABCD` instead of `CDEF`. The no-save
  case passed.
- `. ./env && KC3_TEST=tls_buf make -C test test_libkc3 test_libkc3_debug`
  passed after the fix: all four cases in both release and debug builds,
  including the 56 save-position/write-chunk combinations.
- `. ./env && make` passed.
- `. ./env && make test` passed: 499 C tests and all IKC3, KC3S, event-poll,
  socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No commit was made.

**Limits:** the new regressions test adapter bookkeeping with a deterministic
transport, not a new encrypted TLS exchange. They cover successful short
writes, not errors after partial progress. No sanitizer, native Windows, or
32-bit testing was performed. F31 and F33 are addressed below.

### F31 — Medium: partial flush errors leave sent bytes queued for replay

**Status:** fixed on 2026-10-19; focused release/debug regressions and the
full build/test suite pass.

**Locations (audited baseline):** `libkc3/buf_fd.c:40–47,207–246`,
`libkc3/tls_buf.c:28–37,148–173`.

Before the fix, both adapters tracked successfully written bytes only in a
local `bytes` variable until the whole permitted prefix was sent. A later
error or timeout returned immediately without removing that completed
prefix from the buffer or adjusting saved positions. Partial writes are
allowed and must be accounted for. [OpenBSD write(2)](https://man.openbsd.org/write.2)

For buffered `"ABCD"`, a first write of two bytes followed by a failure
leaves all four bytes queued even though `AB` was emitted. If the transport
becomes writable again, a later flush sends `ABCD`, yielding `ABABCD`.
For the FD adapter, a short nonblocking write followed by `EAGAIN` and a
poll timeout provides a concrete error path. `buf_fd_close` also calls
`buf_flush`, so an error followed by ordinary cleanup can attempt replay
without an explicit application-level retry. TLS has the same accounting
defect on its error/timeout exits; an expired deadline or fatal connection
may prevent any later retry from succeeding.

**Impact / preconditions:** duplicated output/protocol corruption after
partial progress plus a recoverable failure and subsequent flush. The
function does report the error; this is not a claim that it acknowledges
complete success. F13 repaired the FILE persistence path, not these two
adapters. F30 concerns successful TLS tail handling and is independent.

**Suggested remediation:** preserve partial progress on every return path,
compacting only bytes actually accepted by the transport and adjusting saves
consistently. Define whether a failed stream is retryable; close must not
silently replay data already emitted.

**Suggested regression:** add deterministic short-write/error/retry and
short-write/error/close cases to the existing C runner, covering the actual
FD/TLS adapters. Check output bytes and saved positions. A mock of only
generic `buf_write` does not cover adapter-local progress accounting.

**Implemented:**

- Both flush functions now route error/timeout exits after transport work
  through the same compaction and saved-position adjustment as success.
  They discard exactly the number of bytes confirmed by positive write
  returns, keeping the complete remaining suffix, including saved tails.
- Errors still return `-1`; successful and empty-flush return values are
  unchanged. Failures before any progress leave data and saves unchanged.
  A subsequent flush, including the close callback's flush, cannot replay
  a previously confirmed prefix from this buffer.
- No new stream-recovery policy is introduced: an FD may be retried if its
  failure is recoverable; this bookkeeping fix does not make a fatal TLS
  connection usable. After `TLS_WANT_*`, retries must preserve the pending
  bytes and length. Local libtls source (`tls_configure_ssl` in
  `/usr/src/lib/libtls/tls.c`) enables `SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER`,
  which permits the address change from compaction but not changed content
  or length. [OpenBSD SSL_write(3)](https://man.openbsd.org/SSL_write.3)
- Added three FD and three TLS regression cases to the existing
  `test/buf_fd_test.c` and `test/tls_buf_test.c`. Each compiles the actual
  adapter under test-local names with deterministic transport hooks; real
  FD socket/pipe/file tests and F30's successful TLS tests remain enabled.

**Regression coverage:** 64 FD and 80 TLS scenarios cross zero through
three confirmed one-byte writes with explicit retry or close, with and
without nested saves. They assert exact binary output, retained bytes,
buffer/save positions, error returns, successful retry counts, and saved
position restoration. FD scenarios cover socket errors, ordinary-file
fallback errors, and `EAGAIN` followed by poll timeout or failure. TLS
scenarios cover negative/zero write returns, deadline expiry, and both
`TLS_WANT_POLLIN` and `TLS_WANT_POLLOUT` with failed readiness waits. The
TLS hook also rejects retries with altered pending data or length.

**Validation:**

- Before the production fix, the new release regression cases failed in
  both adapters: after one confirmed byte, `wpos` remained 8 instead of 7.
  Existing cases passed (five FD and four TLS cases).
- Ran `./update_sources` and `. ./env && (cd test && ./configure)` to refresh
  the source-inclusion dependencies. No source manifests changed.
- `. ./env && make -C libkc3 all` passed.
- `. ./env && KC3_TEST=buf_fd make -C test test_libkc3 test_libkc3_debug`
  passed all eight FD cases in both builds.
- `. ./env && KC3_TEST=tls_buf make -C test test_libkc3 test_libkc3_debug`
  passed all seven TLS cases in both builds.
- `. ./env && make` passed.
- `. ./env && make test` passed: 505 C tests and all IKC3, KC3S,
  event-poll, socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` passed. No headers or code comments were added; no
  commit was made by the agent. `audit.md.orig` was left untouched.
- Logs: `/tmp/kc3-f31.LxWBCTR3/` (`red-fd.log`, `red-tls.log`,
  `focused-fd.log`, `focused-tls.log`, `libkc3.log`, `update-sources.log`,
  `test-configure.log`, `make.log`, `make-test.log`).

**Limits:** fault sequences are deterministic adapter-level tests, not a
new encrypted-wire fault test or proof of recoverability after fatal TLS
errors. Callers must not change a pending TLS write before retrying it;
timeouts are not automatically renewed. Only positively acknowledged
transport progress can be accounted for. No sanitizer, native Windows,
or 32-bit testing was performed. F33 is addressed below.

### F32 — Low: substitution sizing drops the unmatched trailing bytes

**Status:** fixed on 2026-10-19 as part of correcting F27's EOF handling;
rechecked after F31 with focused release/debug tests passing.

**Locations (audited baseline):** `libkc3/str.c:1365–1428`,
`libkc3/buf.c:2123–2148`.

Before the correction, the sizing loop stopped when fewer than
`search->size` bytes remained, then returned `result` without adding that
remaining suffix. The emitting pass tried to copy the suffix with
`buf_xfer`, but its fixed output buffer was sized using the incomplete total.

For source `"abc"`, search `"zz"`, replacement `"x"`, the size pass counts
`a` and `b` and returns 2, omitting `c`. The emitting pass fills those two
bytes, cannot transfer `c`, and returns NULL instead of the unchanged
`"abc"`. Likewise, replacing `"ab"` with `"c"` in `"abX"` should produce
`"cX"` but fails. This occurs with valid ASCII and is independent of F27.

**Impact / preconditions:** ordinary substitutions fail when a nonempty
short suffix remains, including many no-match cases. `buf_xfer` rejects
the insufficient space; this finding does not establish a buffer overflow.

**Suggested remediation:** include the trailing byte count in checked output
sizing and keep sizing/emission semantics identical.

**Suggested regression:** extend `test/str_test.c` with unmatched multi-byte-
length search strings, matches followed by a short suffix, source shorter
than search, empty source, deletion, and complete matches. Assert both
successful return and exact output length/content.

**Implemented with F27:**
`str_init_subst_size` now adds `in.wpos - in.rpos` before returning, matching
the tail copied by `str_init_subst`. This is necessary both when the decoder
reaches an incomplete final character and when the main loop ends because
the remaining suffix is shorter than the search string.

**Regression coverage / validation:** `str_init_subst_tail` adds 12 bounded
cases in the existing `test/str_test.c` runner, including both examples above,
source shorter than search, empty source, deletion/expansion, complete and
repeated matches, UTF-8, and raw trailing bytes. Both the size API and exact
constructor output are checked. The new case failed before the tail-count
correction and passes in release/debug builds. Full-suite validation is
recorded with F27 above.

**Recheck after F31:** confirmed the tail-count addition remains in
`libkc3/str.c:1433` and the 12 tail cases remain registered through
`str_init_subst_tail`. `. ./env && KC3_TEST=str make -C test test_libkc3 test_libkc3_debug`
passed all 18 release and 21 debug string tests, including the tail and EOF
regressions. Log: `/tmp/kc3-f32.fjnki4rC/str-tests.log`. No additional code
or test changes were needed; only this report was updated. The full suite
was not rerun for this documentation-only recheck; see F31 and F33 for
full-suite validation. No commit was made.

**Limits:** this corrects the omitted suffix, not all large-output size
arithmetic. No multi-GiB, sanitizer, or 32-bit runtime testing was performed.

### F33 — Medium: temporary buffers leak their allocated rwlocks

**Status:** fixed on 2026-10-19; focused regression tests and the full
build/test suite pass.

**Locations (audited baseline):** `libkc3/rpc.c:23–85`,
`libkc3/base64.c:39–79,82–127`,
`libkc3/buf.c:188–198,400–432,1105–1151`, `libkc3/rwlock.c:86–108`.

With `HAVE_PTHREAD`, `buf_init` and `buf_init_const` allocate a separate
rwlock even when the byte storage is borrowed or stack-backed. That lock
must be released with buffer cleanup; merely copying out the bytes does
not transfer or destroy it.

Before the fix, `rpc_request` initialized stack-backed error/output buffers,
but called `buf_clean` on neither buffer on any ordinary return path. Each
successful request therefore lost two allocated locks. Restoring `env->out` and
`env->err`, or cleaning the returned `RPC.Response`, does not own/free those
temporary locks. The response contains strings and the result, not buffers.

Both `base64_encode` (for nonempty input) and `base64_decode` similarly
initialized an input buffer and returned without cleaning it. Decode cleaned
its output buffer, but not its input. These were small-input, normal-success
leaks, unrelated to F26's large-input overflow.

**Impact / preconditions:** accumulating heap/pthread-resource leaks in
long-running users of these APIs with pthread support enabled. Repeated
ordinary requests/encoding operations suffice. No particular remote RPC
deployment or resource-exhaustion time has been established.

**Suggested remediation:** balance every successful temporary-buffer
initialization on all return paths, including failures, after restoring
borrowed environment pointers. Preserve ownership of stack bytes and
returned strings. Nonlocal evaluation exits in RPC need a separate unwind
review; this finding only establishes the ordinary-return leaks.

**Suggested regression:** use the existing C runner and allocation tracking
patterns from `test/env_fork_test.c` to repeat small encode/decode and benign
RPC requests, cleaning results and checking no temporary locks remain.
Include initialization/error exits and pthread-disabled builds where
supported. No leak measurement was run during the original source audit.

**Implemented:**

- Base64 encoding cleans its borrowed input buffer before returning the
  result. Decoding cleans its input buffer immediately after consuming it,
  before attempting the output-string copy, so both copy success and failure
  release the input lock. Neither cleanup owns or frees the input bytes.
- RPC checks both temporary-buffer initializations before redirecting
  `env->err` or `env->out`. A failed second initialization cleans the first
  buffer; a failed first initialization has no buffer to clean.
- Every later ordinary RPC return restores both environment pointers and
  cleans both temporary buffers. The response strings are independent
  copies, and existing tag cleanup and error/assertion behavior are retained.
  No unwind protection was added, and `env_clean` was not changed.

**Regression coverage:**

- Three new Base64 cases in `test/base64_test.c` use the existing allocation
  tracker in bounded child processes. Each encode/decode success case
  repeats five empty, padded, unpadded, and binary vectors 32 times,
  verifies exact output and borrowed input ownership, cleans the result,
  and requires zero remaining tracked allocations after each operation.
- The Base64 failure case injects output-buffer initialization failure and
  output-string copy failure 16 times each using the existing source-copy
  test hooks. Both failures leave the destination unchanged and no tracked
  allocations outstanding. F26's encode allocation-boundary tests remain.
- Added `test/rpc_test.c`, registered the `rpc` target in the same runner,
  and regenerated root/test manifests using `./update_sources`. After
  loading `RPC.Response` and warming the evaluated modules, the success
  case repeats 32 pairs of quiet requests and requests writing to both
  captured streams. It checks result/string contents after return,
  restored environment pointers, and zero tracked allocations after
  cleaning each response.
- RPC fault cases exercise both buffer-initialization failures, parsing
  failure, response-tag initialization failure, response-data allocation
  failure, and failure copying either captured stream. Each is repeated
  eight times. Checks cover unchanged destination tags, environment
  restoration before buffer cleanup, cleanup counts, and allocation balance.
  Faults are injected at these API boundaries, not through malformed input
  or process-wide memory exhaustion.
- All six Base64 cases pass in release/debug builds. RPC success and buffer
  initialization failures pass in both builds; the five later RPC failure
  paths are tested in release only because their existing debug assertions
  abort instead of returning. Assertions were not suppressed in the tests.

**Validation:**

- Before the production fix, all three new Base64 cases failed allocation
  balance checks. The RPC success case also leaked, the initialization-
  failure child did not exit normally, and the ordinary-error case observed
  zero buffer cleanups where two were required.
- `./update_sources` and `. ./env && (cd test && ./configure)` passed.
- `. ./env && make -C libkc3 all` passed.
- `. ./env && KC3_TEST=base64 make -C test test_libkc3 test_libkc3_debug`
  passed all six cases in each build.
- `. ./env && KC3_TEST=rpc make -C test test_libkc3 test_libkc3_debug`
  passed all three release cases and both enabled debug cases.
- `. ./env && make` passed.
- `. ./env && make test` passed: 511 C tests and all IKC3, KC3S,
  event-poll, socket, TLS, PDF, EKC3, JSON, Markdown, HTTP, and crypto suites.
- `git diff --check` and `git diff --cached --check` passed. No headers or
  code comments were added; no commit was made by the agent.
  `audit.md.orig` was left untouched.
- Logs: `/tmp/kc3-f33.VPRueylu/` (`red-base64.log`, `red-rpc.log`,
  `focused-base64.log`, `focused-rpc.log`, `libkc3.log`,
  `update-sources.log`, `test-configure.log`, `make.log`, `make-test.log`).

**Limits:** this closes the reported ordinary-return leaks, not nonlocal
RPC evaluation exits or all allocation-failure behavior inside buffer/lock
initializers. No pthread-disabled, sanitizer, native Windows, or 32-bit
build was run. Base64's existing decoding policy and RPC evaluation
semantics are unchanged. The rest of libkc3 has not been exhaustively
audited; completing these reported findings does not imply otherwise.

### Re-audit disposition and limits

- F24–F27 and F32 are fixed with regression coverage and validation recorded
  above, including full-suite validation of the F27 EOF correction/F32.
  F28–F31 and F33 were subsequently fixed as documented above. All findings
  F24–F33 in this report are now addressed within their documented scope.
- F12–F23 retain their previously documented completion scope. These
  full-suite runs are not exhaustive re-audits of those earlier fixes.
- Static examples establish the source-level failures described above;
  exploitability of a particular network service, resource thresholds in
  deployment, and platform-specific runtime behavior require separate work.
- The original review changed only `audit.md`. F24 remediation changed
  `libkc3/buf.c`, `test/buf_test.c`, and this report. F25 remediation changed
  `libkc3/sh.c`, added `test/sh_test.c`, registered it in the existing runner,
  regenerated the root/test source manifests, and updated this report.
  F26 remediation changed `libkc3/base64.c`, added `test/base64_test.c`,
  registered it in the same runner, regenerated the root/test source
  manifests, and updated this report.
  F27 remediation and its EOF/F32 correction changed only `libkc3/str.c`,
  `test/str_test.c`, and this report.
  F31 remediation changed only `libkc3/buf_fd.c`, `libkc3/tls_buf.c`,
  `test/buf_fd_test.c`, `test/tls_buf_test.c`, and this report.
  F33 remediation changed `libkc3/base64.c`, `libkc3/rpc.c`, and
  `test/base64_test.c`, added and registered `test/rpc_test.c`, regenerated
  root/test source manifests, and updated this report.
  No headers were changed and no commit was made by the agent.
