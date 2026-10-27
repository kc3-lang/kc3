# KC3 audit log — F40 onward

## Focused libkc3 re-audit — 2026-10-19

**Baseline:** `887793afb` (`audit`); the worktree was clean at the start.
That commit archived the preceding report by removing it from the tree.
The F34–F39 report is retained at `9d8ccf607:audit.md`; its code fixes are
in `e2889a60a`. Earlier reports remain at `ea2356129:audit.md` (F01–F11),
`ba9ee2cc3:audit.md` (F12–F23), and `88db53900:audit.md` (F24–F33).
This pass does not renumber or reopen those findings wholesale.

**Original audit method:** source inspection, ownership/arithmetic analysis, and caller
tracing. All examples below are source-derived, not runtime reproductions.
During that audit, no build, test, interpreter expression, standalone probe,
sanitizer, or fault-injection experiment was run. Only this report was
written; no code, headers, tests, or source manifests were changed.

**Remediation update:** F40–F46 are fixed as described below, with
validation and scope limits recorded for each finding.
No regression tests, headers,
source manifests, or code comments were added or changed. No commit was made.

**Scope:** selected random-string and splitting paths; struct field updates
and their cleanup; complex division; Time arithmetic and Timespec casts;
map/list ownership; and the relevant evaluator, FFI, and language bindings.
Queue, hash-table iteration, Cow/tuple constructors, array copying, and
struct-type handling were also inspected selectively. Repository docs and
tests, bundled LibTomMath source, and local OpenBSD libc/kernel source were
consulted where needed. This is not an exhaustive audit of `libkc3`.

High denotes a native memory-safety defect under the stated preconditions.
Medium denotes undefined arithmetic, incorrect results, availability, or
resource-lifecycle failure without an established memory-corruption path.
Neither label establishes remote exploitability or a deployed service's
exposure. Completion is recorded in the status table and remediation notes;
the original descriptions and source locations refer to the audit baseline.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F40 | High | `Str.random` omits the required NUL terminator | Fixed; no new regression tests |
| F41 | High | Failed struct-field replacement leaves freed data for cleanup | Fixed; no new regression tests |
| F42 | Medium | Complex division loses owning arithmetic temporaries | Fixed; no new regression tests |
| F43 | Medium | Time addition misses the exact one-second nanosecond carry | Fixed; no new regression tests |
| F44 | Medium | Floating Timespec conversion overflows before taking the remainder | Fixed; no new regression tests |
| F45 | Medium | Random Base64 generation left-shifts signed random bytes | Fixed; no new regression tests |
| F46 | Medium | Multi-separator splitting reports successful matches as zero | Fixed; build passes; full test suite green (user-confirmed) |

## Findings

### F40 — High: random strings have no allocated terminator

**Locations:** `libkc3/str.c:1171–1185,421–429`,
`lib/kc3/0.1/str.facts:53–55`, `libkc3/time.c:218–233`,
`doc/1_KC3/1.20_Str.en.md:3–5`.

`str_init_random` allocates exactly the requested number of bytes, fills
all of them with `arc4random_buf`, and publishes that allocation as an
`s_str`. It neither reserves nor writes a trailing NUL. `str_init` merely
stores the pointer and size; it does not append a terminator. Initial
zero-filling in `alloc` does not help because the random generator replaces
every allocated byte.

This violates the documented NUL-termination contract of KC3 strings.
For a one-byte result whose random byte is nonzero, there is no NUL anywhere
inside the allocation. A C-string consumer therefore reads beyond it.

**Reachability / impact:** the public binding is `Str.random`. A concrete
source-derived consumer chain is `Time.from_str(Str.random(1))`:
`time_init_str` accepts the nonempty, non-NULL storage and passes it to
`sscanf`. The inspected local implementation at
`/usr/src/lib/libc/stdio/sscanf.c:54` calls `strlen(str)` before parsing.
The evaluator passes the nested call's result directly as a `Str` argument
(`env_eval.c:378–409`, `cfn.c:113–125`, `tag.c:1639–1643`), without a
copy that would add a terminator. This establishes an out-of-bounds read
path for ordinary small input, not an arbitrary-write or proven disclosure
primitive. An interior random NUL may mask the bug on a particular call.

This is distinct from F37: the date parser's empty-storage guard is present,
but this producer supplies a nonempty, unterminated allocation.

**Remediation direction:** validate the requested length against the string
representation and allocation limits, reserve an additional byte, randomize
only the requested payload, and keep the terminator outside the counted
data. Preserve binary payloads, including random interior NULs.

**Completed remediation:** `str_init_random` now uses `str_init_alloc`
to allocate a temporary string with a zero-filled trailing byte, then
randomizes exactly `tmp.size` payload bytes. The terminator remains outside
the counted data, and interior random NULs are preserved. Only a successful
initialization publishes the temporary to the destination.

The existing signed-word conversion is retained, with its type name and
diagnostic corrected to `Sw`. Negative converted lengths and converted
lengths at or above `U32_MAX` are rejected before allocation. The upper
bound prevents narrowing or overflow in the string allocator's `size + 1`
expression. Zero length now produces an allocated, NUL-terminated empty
string instead of requesting a zero-byte allocation. This does not change
or harden the generic tag-to-`Sw` conversion rules themselves.

### F41 — High: struct update cleans the old field before replacement succeeds

**Locations:** `libkc3/struct.c:604–649,150–203`,
`libkc3/pstruct.c:138–150`, `libkc3/map.c:56–67,137–160,202–220`,
`libkc3/data.c:240–326,763–813`, `libkc3/kc3.c:1477–1530`.

`struct_set` calls `data_clean(type_sym, data)` on the existing field before
calling `data_init_copy` for its replacement. Some field cleaners free
their allocations without clearing their pointers/counts. If replacement
copying then fails without publishing a new value, the struct retains
metadata pointing to freed storage.

A nonempty `Map` field provides a concrete failure sequence:

1. `Struct.put` successfully creates its temporary copy of the input struct.
2. `struct_set` frees that temporary's old map keys and values through
   `map_clean`; the field still contains their old pointers and count.
3. Allocation of the replacement map fails. `map_init_copy` returns NULL
   without overwriting the destination field.
4. `pstruct_init_put` calls `pstruct_clean` on the temporary struct.
   `struct_clean` cleans the map again, reading freed tags and freeing
   the already-freed key/value allocations.

**Reachability / impact:** use-after-free/double-free during a failed
language-level struct update. `Struct.put_multiple` also deletes its
temporary when `struct_set` fails. Both bindings are in
`lib/kc3/0.1/struct.kc3:17–20`. The example requires a nonempty owning field
and a failure during replacement, after the initial struct copy succeeds;
it is not a normal-success crash or a demonstrated remotely inducible OOM.
Debug allocation/assertion handling may abort before the second cleanup;
the returned-error path matters in release builds.

**Remediation direction:** construct the replacement in separate temporary
storage first, then clean and replace the old value only after success.
Cover inline composite types and their ownership rules; clearing one
specific field representation alone would not make replacement atomic.
Leave the old field valid on failure and account for possible source aliasing.

**Completed remediation:** `struct_set` now resolves the field's storage
size with `sym_type_size`, allocates separate temporary storage, and runs
`data_init_copy` into that temporary before cleaning the old field. The
size follows the existing struct-layout rules, including inline struct
fields; it is not restricted to the size of a tag's payload. Zero-sized
fields succeed after type validation without requesting a zero-byte
allocation.

If size lookup, temporary allocation, or replacement copying fails, the
old field remains untouched. In particular, the failed Map replacement
above can now be followed by ordinary cleanup of the caller's temporary
struct without revisiting freed old map storage. The temporary buffer is
freed on a returned copy failure; it is not passed to `data_clean` as though
initialization had succeeded.

After a successful copy, the setter cleans the old field, moves the
initialized representation into its slot with `memcpy`, and frees only
the temporary buffer, transferring ownership to the field. All reads from
the replacement source finish before the old field is cleaned, including
when the source aliases that field. Existing copy/refcount semantics are
retained through `data_init_copy`.

**Boundary:** this fixes the setter's premature cleanup, not every
lower-level copy failure path. Those initializers still own partial-copy
rollback. For example, `struct_type_copy_data` does not unwind earlier
copied fields when a later field fails; that existing resource-leak path
is not repaired by this ordering change. No fault-injection coverage is
claimed.

### F42 — Medium: successful complex division leaks intermediate values

**Locations:** `libkc3/complex.c:82–118,291–302`,
`libkc3/tag_div.c:31–35`, `libkc3/tag_mul.c:396–402`,
`libkc3/ratio.c:463–486`, `libkc3/integer.c:219–235,254–265`.

`complex_div` creates nine temporary tags for products, sums, differences,
and the denominator. It copies only `tmp.x` and `tmp.y` to the result and
returns without cleaning any of the nine intermediates.

Those temporaries are not always immediate scalar values. For rational
components, the multiplication/addition/subtraction paths retain Ratio
tags whose numerators and denominators own LibTomMath allocations.
`ratio_simplify` creates independent result integers rather than moving
ownership out of its inputs. Bundled `libtommath/bn_mp_init.c` confirms
that integer initialization allocates digit storage.

**Reachability / impact:** ordinary successful complex division with
owning numeric components leaks storage on every call, even if the caller
cleans both operands and the returned result. A source-derived example is
`(1/2 +i 1/3) / (1/4 +i 1/5)`. `tag_div` dispatches complex operands to
`complex_new_div`, which calls this function. Large Integer components can
also make the intermediates own memory. No failure injection or malformed
data is needed; repeated operations can accumulate unbounded leaks.

**Remediation direction:** clean every owning intermediate on success and
failure, check the arithmetic calls before using their outputs, and publish
the final complex value only after both result components are ready.
This finding is about division's temporaries, not a claim that all complex
operations or their error paths have been exhaustively audited.

**Completed remediation (2026-10-20):** `complex_div` now checks all eleven
arithmetic calls in their original order, short-circuiting on a returned
failure. A shared cleanup sequence releases all nine zero-initialized
intermediate tags on both success and returned failure, including their
owning Integer and Ratio storage.

The result remains local in `tmp` until both components have been
computed successfully. On failure, `complex_clean(&tmp)` also releases
any result components already initialized, and the destination is left
untouched. On success, only the result is transferred to the destination;
its storage is independent of the intermediates being cleaned. The
division formula and arithmetic dispatch remain unchanged.

The new checks handle failures reported by the arithmetic helpers; they
do not repair failures those helpers currently fail to propagate or their
internal cleanup paths. No broader arithmetic hardening is claimed.

### F43 — Medium: Time addition leaves exactly one billion nanoseconds unnormalized

**Locations:** `libkc3/time.c:144–163`,
`libkc3/tag_add.c:1095–1098`, `libkc3/tag_init.c:760–769`,
`libkc3/compare.c:1789–1813`, `libkc3/kc3.c:1390–1396`,
`libkc3/timespec.c:88–94`.

`time_init_add` carries nanoseconds only while the sum is strictly greater
than `1000000000`, rather than greater than or equal to that value.
Adding two valid values `{tv_sec = 0, tv_nsec = 500000000}` therefore
produces `{0, 1000000000}` instead of `{1, 0}`. There is no integer overflow
or invalid input in this example.

**Reachability / impact:** normal Time `+` uses this implementation.
`compare_time` compares seconds and nanoseconds separately, so the result
compares less than the equivalent canonical `{1, 0}`. Passing it to `sleep`
copies the invalid nanosecond field unchanged into a Timespec. The inspected
OpenBSD kernel rejects it with `EINVAL` before sleeping
(`/usr/src/sys/kern/kern_time.c:297–298`, with `timespecisvalid` in
`/usr/include/sys/time.h:115–116`); `kc3_sleep` ignores that error.
The defect therefore affects equality/ordering and can turn a one-second
delay into an immediate return.

**Remediation direction:** carry at the exact boundary as well as above
it, preserving the normalized range `0 <= tv_nsec < 1000000000`.
This specific finding does not establish that all signed-overflow and
noncanonical-Time cases in the surrounding APIs are handled correctly.

**Completed remediation (2026-10-20):** `time_init_add` now carries while
`tmp.tv_nsec >= 1000000000`, including the exact one-second boundary.
The source-derived half-second-plus-half-second example therefore produces
`{tv_sec = 1, tv_nsec = 0}`. The negative-nanosecond borrowing loop,
addition order, and temporary-result publication are unchanged. This
fix is limited to the carry boundary; it does not add signed-overflow
checks or change the surrounding Time APIs.

### F44 — Medium: floating Timespec casts overflow before remainder reduction

**Locations:** `libkc3/timespec.c:18–27,38–60`,
`libkc3/kc3.c:1390–1396`, `lib/kc3/0.1/init.kc3:319`,
`libkc3/types.h:293`.

The shared floating conversion macro assigns `src * 1000000000` to
`tmp.tv_nsec` and only then takes `% 1000000000`. The intermediate thus
contains the entire duration in nanoseconds, not just its fractional part.
`s_timespec` is the system `struct timespec`; the local system definition
uses signed `long` for `tv_nsec` (`/usr/include/sys/_time.h:56–58`).

On a target with 32-bit signed `long`, converting `3.0` attempts to store
3,000,000,000 in that field. On a target with 64-bit signed `long`, converting
the finite duration `10000000000.0` attempts to store 10^19, also outside
the field's range even though the seconds fit a 64-bit `time_t`.
Floating-to-integer conversion outside the destination range is undefined;
the subsequent remainder cannot make the earlier conversion valid.
There are also no finite/range checks before assigning `src` to `tv_sec`.

**Reachability / impact:** the `sleep` binding accepts a Tag and dispatches
floating values through these constructors. On 32-bit-long targets, small
ordinary delays already hit the bug. On 64-bit-long targets, that particular
three-second example is safe, but sufficiently large or nonfinite values
are not. No alternate-platform build or actual long sleep was executed.

**Remediation direction:** validate finiteness and the seconds range before
integer conversion; split whole seconds from the fractional part before
scaling; normalize any rounding carry. Define negative-duration handling
consistently with the caller instead of relying on signed remainder.

**Completed remediation (2026-10-20):** the shared floating constructor
now checks `0 <= src < limit` before any floating-to-integer conversion.
The exclusive upper limit is an exact power of two derived from the
width and signedness of `time_t`, rather than a rounded floating
representation of its maximum integer. This rejects negative durations,
NaN, infinities, and out-of-range seconds. Both positive and negative zero
remain valid zero durations. Rejected input returns NULL without changing
the destination; the existing `kc3_sleep` caller then skips `nanosleep`.

After safely converting whole seconds, the constructor subtracts those
seconds from the source and scales only the fractional part by one
billion. Fractional arithmetic remains in the source type, including
F80/F128 when enabled; the source is not narrowed to double. A scaled
fraction that rounds up to one billion carries into seconds only after
checking that the increment fits, and leaves zero nanoseconds. The final
nanosecond conversion is therefore bounded independently of `long` being
32 or 64 bits, with `0 <= tv_nsec < 1000000000` on success.

This change is limited to floating constructors. The unsigned-integer
constructors, Time conversion, and `kc3_sleep`'s existing handling of
system-call errors are unchanged.

### F45 — Medium: random Base64 assembly shifts negative signed characters

**Locations:** `libkc3/str.c:1241–1291`,
`libkc3/crypt.c:95–109`, `http/http_request.c:506–520`,
`lib/kc3/0.1/str.facts:59–61`.

`str_init_random_base64` stores random bytes in `char *` and assembles a
three-byte value using `random_bytes[i] << 16` and
`random_bytes[i + 1] << 8`. On implementations where plain `char` is signed,
bytes with the high bit set promote to negative `int` values. Left-shifting
those negative values is undefined behavior. Assigning the completed
expression to unsigned `uw u` does not change the operand types used by
the earlier shifts. The two-byte tail repeats the same problem.

**Reachability / impact:** ordinary random data suffices; for example, a
three-byte block starting with byte `0x80` reaches a negative left shift
on a signed-char target. `Str.random_base64(16)`, the native password-salt
generator, and multipart-upload name generation all use this code.
The finding is platform-dependent and concerns defined arithmetic in these
paths. It does not demonstrate predictable salts, an entropy loss, an
out-of-bounds alphabet index, or a remote exploit.

**Remediation direction:** treat random input as unsigned bytes and perform
the assembly in a sufficiently wide unsigned type before shifting. Preserve
the intended output length and alphabet. The neighboring random-length
arithmetic deserves separate checking; it is not the basis of this finding.

**Completed remediation (2026-10-20):** `str_init_random_base64` now reads
random input through `u8 *`, uses a `u32` accumulator, and explicitly casts
each shifted byte to `u32` before shifting. Both the three-byte assembly
and two-byte tail avoid negative signed operands; the one-byte tail also
receives the byte's nonnegative value without sign extension. The complete
three-byte value is at most `0xFFFFFF` and fits the accumulator.

The existing Base64url alphabet, low-six-bits-first emission order,
requested output length, allocations, and termination behavior are
unchanged. Length conversion and allocation-size arithmetic remain
outside this finding's fix; no entropy or broader random-generator
hardening is claimed.

### F46 — Medium: multi-separator splitting discards a successful first segment

**Locations:** `libkc3/buf.c:1252–1305`,
`libkc3/str.c:1972–2000`, `libkc3/buf.c:1124–1174`,
`libkc3/buf.h:126–128`, `libkc3/str.h:176–178`.

`buf_read_until_list_into_str` initializes `result` to zero and never
increments it. Even after consuming a delimiter and writing an allocated
prefix to `dest`, it returns that unchanged zero. Its temporary buffer is
also never cleaned after initialization on the match path.

`str_split_list` interprets a nonpositive result as the terminal/no-match
case and immediately calls `buf_read_to_str` into the same destination.
For input `"a,b,c"` and the separator list `[","]`, the helper first
allocates `"a"` and consumes the comma, but returns zero. The caller then
overwrites that string with the remaining `"b,c"`, loses the first
allocation, and finishes with a single-element list rather than three
segments. With pthread support, the uncleaned temporary also loses its
allocated rwlock. This failure does not require OOM or invalid separators.

**Reachability / impact:** incorrect output and leaks through the exported
native `str_split_list` API and its helper. This pass found no standard
KC3 language binding for that API; the ordinary `Str.split` binding uses
the distinct single-separator implementation and is not implicated by
this example. No network exposure is claimed.

**Remediation direction:** maintain and return the actual consumed-byte
count/status, distinguish successful empty segments from failure, and
balance the temporary buffer on every initialized path. Ensure the caller
does not overwrite an owning destination when interpreting a helper result.
The return-status defect is distinct from F33's earlier selected temporary
buffer cleanup and F38's constructor failure contracts.

**Completed remediation (2026-10-27):** `buf_read_until_list_into_str`
now accumulates the consumed byte count, including the matched delimiter,
and returns that positive count on success. Empty prefixes are successful
matches too. It checks temporary-buffer initialization and cleans that
buffer after the string copy on both success and failure, releasing its
rwlock. The temporary is a read-only view of borrowed input, preventing
refill compaction from modifying that input. Byte-count additions are
checked against `SW_MAX`.

The helper validates every separator as a nonempty Str whose size fits
the return type; an empty separator list means no match. It checks for
buffer exhaustion before decoding, returns zero for normal end-of-stream
without a match, and returns negative statuses for detected failures.
Normal input retains UTF-8 stepping; undecodable buffered input falls
back to consuming one byte, avoiding stalls or a spurious failure solely
from a truncated trailing sequence. On no match or failure, it restores
the saved read position and line plus the original pretty-print column,
without publishing a destination string. Separator precedence is unchanged.

`str_split_list` now checks buffer and list-node allocation, propagates
negative helper results, and reads the final tail only for a zero result.
It accepts a zero-length tail, preserving empty input and leading,
adjacent, or trailing empty segments. Failed operations clean the partial
list and leave the caller's destination unchanged. Successful segments
are no longer overwritten by the fallback tail copy.

This is not a redesign of the underlying UTF-8, delimiter-matching, or
refill primitives. Their independent boundary and I/O-error behavior,
and the separate single-separator split API, are outside this fix.

## Disposition and limits

- F40–F46 are fixed as recorded above, subject to their stated scope
  limits. No new regression tests were added, following the user's
  instruction.
- Source examples have not been executed; the earlier passing suites are
  not evidence of dedicated coverage for these findings.
- F40 depends on a random payload without an interior NUL reaching a
  C-string consumer; F41 requires a replacement-copy failure; F42/F43/F46
  have ordinary successful-input paths; F44 depends on platform widths and
  value range; F45 requires signed plain `char` and high-bit random bytes.
- Candidate issues without sufficiently established contracts/caller paths
  were not promoted to findings. This report is not a claim that every
  inspected function is safe outside the paths described.
- Remaining coverage includes generated arithmetic more broadly, parser
  error unwinding, cross-thread ownership, every serialized type, and
  platform-specific behavior. Existing unchecked buffer-initialization
  consumers remain outside F38's completed constructor-level scope.
- No commits were made.

## F40 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed.
- `git diff --check`: passed; the audit report has no whitespace errors.
- Logs: `/tmp/kc3-f40.yvnRjynJ/`.
- Only existing suites are used; no dedicated F40 test cases or standalone
  probes were added. Existing-suite results do not imply dedicated coverage
  of F40's terminator and length boundaries.

## F41 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed, including the existing struct and struct-update cases.
- `git diff --check`: passed; the audit report had no whitespace errors.
- Original logs: `/tmp/kc3-f41.lBxinrxZ/`; these temporary files were no
  longer present when starting F42. Results were observed during F41 work.
- These are existing-suite results, not dedicated coverage of replacement
  allocation/copy failures or aliasing. No fault injection was performed.
- No regression tests, standalone probes, headers, source manifests, or
  code comments were added or changed. No commit was made.

## F42 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed.
- `git diff --check`: passed; the audit report has no whitespace errors.
- Logs: `/tmp/kc3-f42.3dL3dUv9/`.
- No dedicated division leak test, allocation-failure injection, or
  standalone probe was run. Existing-suite results do not establish
  dedicated coverage of F42's resource-lifecycle paths.
- No regression tests, standalone probes, headers, source manifests, or
  code comments were added or changed. No commit was made.

## F43 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed.
- `git diff --check`: passed; the audit report has no whitespace errors.
- Logs: `/tmp/kc3-f43.Ia9uV2fo/`.
- No dedicated carry-boundary regression case or standalone probe was
  added or run. Existing-suite results do not establish dedicated
  coverage of the exact-one-second boundary.
- No regression tests, headers, source manifests, or code comments were
  added or changed. No commit was made.

## F44 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed.
- `git diff --check`: passed; the audit report has no whitespace errors.
- Logs: `/tmp/kc3-f44.grEnYpaK/`.
- The local configuration enables F32, F64, and F80, but not F128. No
  alternate-platform build, dedicated boundary test, standalone probe,
  or long-duration sleep was run. Existing-suite results do not establish
  coverage of nonfinite inputs, range limits, or rounding carries.
- No regression tests, headers, source manifests, or code comments were
  added or changed. No commit was made.

## F45 validation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed, including the existing random Base64 size checks.
- `git diff --check`: passed; the audit report has no whitespace errors.
- Logs: `/tmp/kc3-f45.oF1DsuKy/`.
- Existing string tests request random Base64 lengths 1, 2, and 64 and
  check the resulting sizes. These are not exhaustive byte-pattern or
  sanitizer checks of the shifts. No standalone probe was run.
- No regression tests, headers, source manifests, or code comments were
  added or changed. No commit was made.

## F46 validation

- `. ./env && make -C libkc3 all`: passed after the final read-only
  temporary-buffer change; release and debug libraries were built.
- On 2026-10-27, `. ./env && make` and `. ./env && make test` initially
  stopped before tests because `/usr/local/include/git2.h` was missing.
  Retrying the full build after the dependency became available built
  the Git extension in release and debug modes, then stopped at missing
  `/usr/local/include/qrencode.h` (prerequisite of `qrencode.main.o`).
- `. ./env && make -C test test` initially stopped at missing
  `/usr/local/include/gtk-4.0/gtk/gtk.h`. After the user installed GTK4,
  a fresh attempt built the test executable and all 511 C tests passed.
  The integration phase then stopped because `runj` was not found;
  the zero-test summary also reported a zero divisor. No integration
  tests ran in that attempt. `runj` is an external dependency documented
  in `doc/3_Guides/3.1_Install.en.md`, not a bundled executable.
- A subsequent `. ./env && make` passed, including the SDL2 release
  and debug targets. The earlier missing-header diagnostics are historical,
  not current blockers. After the user installed `runj` and `kmx_sort`,
  both were found in `/usr/local/bin`.
  No dependency packages or build configuration were changed by this fix.
- The last assistant-run `. ./env && make test` passed all 511 C tests and all
  71 IKC3 integration tests. KC3S then reported 4 passes and 3 failures:
  `facts_acceptor_loop`, `facts_connection`, and `facts_secret`.
  All three hard-code `/etc/ssl/fullchain.pem` and
  `/etc/ssl/private/privkey.pem`; the test output reports the certificate
  missing and the key unreadable, followed by TLS configuration and
  handshake errors. Read-only checks confirmed the absent certificate
  and a root-owned, mode-0700 `/etc/ssl/private` directory. No certificate
  or private-key contents were read, and no system permissions were changed.
  The top-level test target exited with status 2; later integration suites
  were not reached in that attempt.
- User confirmation (2026-10-27): the TLS certificate problem is fixed
  and all tests are green. The TLS blocker is resolved. This successful
  full-suite result is user-reported; no additional test run was performed
  by the assistant after that confirmation.
- `git diff --check`: passed; the audit report has no whitespace errors.
- The first build rejected a separator-size comparison as always false
  on the local 64-bit target. Widening its operand to `uw` preserves the
  intended 32-bit bound check without that diagnostic.
- Earlier validation logs: `/tmp/kc3-f46.6ms56NxJ/`.
  The successful full-build log is
  `/tmp/kc3-f46-recheck.VX92pNW1/build.log`; the last assistant-run test
  attempt is `/tmp/kc3-f46-recheck.VX92pNW1/test-retry.log`.
- No dedicated multi-separator cases, allocation-failure injection,
  sanitizer checks, or standalone probes were added or run. The split
  examples and edge-case analysis above are source-derived, not runtime
  reproductions; passing existing C tests does not establish their coverage.
- No regression tests, headers, source manifests, or code comments were
  added or changed. No commit was made.
