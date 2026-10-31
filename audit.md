# KC3 audit log — F47 onward

## Fresh libkc3 audit — 2026-10-27

**Original code baseline:** `c6c9144c2`.

**Restart baseline — 2026-10-29:** `9c4d66e5c`. F47–F52 were present
in this checkout. F53's implementation was absent despite the saved fixed
status, so it was implemented again using the contract recorded below.
Earlier verification totals describe their original checkouts; the
results from that restart are recorded under F53.

**F54 continuation baseline — 2026-10-30:** `d77a0b88a`.

**F55 continuation baseline — 2026-10-30:** `387b93ff5`.

**F56 continuation — 2026-10-30:** HEAD `387b93ff5`, with the existing
F55 working-tree changes preserved.

**F57 continuation baseline — 2026-10-31:** `69aa494ee`. The F55 and F56
changes are committed in this checkout.

**F58 continuation baseline — 2026-10-31:** `2b5da21ef`. F57 is committed
in this checkout.

**F59 continuation baseline — 2026-10-31:** `9e413de87`. F58 is committed
in this checkout.

**F60 continuation baseline — 2026-10-31:** `70f688eef`. F59 is committed
in this checkout.

**Progress:** the initial focused pass and a serialization/arithmetic
continuation are complete. F47–F60 are fixed and the C suite reports
496 passing tests. F56's counter change is explained in its verification
record. No open findings remain in this log. The full-library audit remains
incomplete; further review is still needed in the areas listed below.

**Method:** source inspection and tracing of contracts, callers, ownership,
and arithmetic. Previously fixed files were not excluded. Open findings
describe defects in the inspected code, not historical defects assumed
to remain unfixed. Examples and execution sequences are source-derived
unless a finding's verification explicitly records a runtime check.
Completed fixes are identified explicitly.

The audit passes were source-only. Subsequent code changes and verification
are recorded under the affected findings. Existing character-inspection
tests were extended with capacity/rollback checks, and one compact Ratio
cast case covers the integer conversion contract across all ten types.
The existing saved-write test now also checks FILE suffix compaction and
nested-save rollback.
The existing Ratio power case and interpreter fixture now verify exact
and approximate rational powers with explicit precision.
One compact tag-shift case checks both directions across supported operand
types and boundaries; the existing integer interpreter fixture also checks
the shift contract.
Through F53, no standalone probes or new test files were added.
F54 adds one registered C case in `test/socket_buf_test.c` for construction
failure ownership and cleanup across all three entry points. F55 adds one
case to the existing FD-buffer group for logical read positions. F56
extends the existing shared-cache and cycle cases, including list mutation,
early result cleanup, shared tails, cache reset, and tuple lifetimes.
F57 adds one case to the existing serialization group for integer count
bounds, truncated payloads, ordinary round trips, and refill behavior.
F58 adds one case to the existing tag group for division/remainder bounds
and extends the existing integer interpreter fixture with eight checks.
F59 adds one case to that tag group for Integer failure propagation,
including helper-boundary failure injection, and eight interpreter checks.
F60 adds one mixed-division case to the tag group, extends those failure
wrappers for Ratio helpers, and adds eight checks to each of the existing
integer and Ratio interpreter fixtures.

**Coverage:** file-to-string readers; FILE/FD buffering and save records;
character inspection; rational conversions and exponentiation; selected
fixed-width shift and division/remainder dispatch; socket-buffer
construction and teardown.
Base32/Base64, random strings, arrays, maps, hash-table iteration, struct
copying, and associated FFI/evaluator/library paths were also inspected
selectively. The continuation traced decoded-list cache lifetimes through
cleanup and repeated reads, integer wire-length arithmetic on 32-bit
targets, and selected division/modulo dispatch and failure propagation.
The source-only continuation read the original shared-cache tests; F56
records the later runtime checks and extensions. These are focused passes,
not an exhaustive audit of all `libkc3` functions, generated arithmetic
combinations, platforms, or threads.

High denotes a native memory-safety defect under the stated preconditions.
Medium denotes undefined arithmetic, incorrect results, data corruption,
or resource-lifecycle failure without an established memory-corruption path.
Low denotes a narrower functional defect. Severity does not establish
remote reachability or exploitability.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F47 | High | File readers return allocated strings without a NUL terminator | Fixed |
| F48 | High | Slice-read failure frees an advanced pointer | Fixed |
| F49 | High | Character-inspection failure leaves a stack save record linked | Fixed |
| F50 | Medium | Ratio casts narrow the denominator before dividing | Fixed |
| F51 | Medium | FILE-backed flush does not move the saved unwritten suffix | Fixed |
| F52 | Medium | Rational exponentiation ignores the exponent denominator | Fixed |
| F53 | Medium | Fixed-width shifts execute unchecked C shift expressions | Fixed |
| F54 | Medium | Socket-buffer construction has inconsistent FD ownership on failure | Fixed |
| F55 | Low | FD-backed read position includes already-buffered bytes twice | Fixed |
| F56 | High | Decoded-list/tuple cache entries outlive their objects | Fixed |
| F57 | High | Serialized integer byte counts wrap on 32-bit targets | Fixed |
| F58 | Medium | Fixed-width division and remainder execute undefined C operations | Fixed |
| F59 | Medium | Big-integer division/modulo failures are reported as successful results | Fixed |
| F60 | Medium | Mixed-type division reduces the wrong result object | Fixed |

## Findings

### F47 — High: file readers omit the string terminator

**Status:** fixed in `file_read_all` and `file_read_slice`. The description
and locations below document the pre-fix code.

**Locations:** `libkc3/file.c:729–781,832–892`,
`libkc3/str.c:421–429`, `libkc3/time.c:218–233`,
`lib/kc3/0.1/file.kc3:64–75`, `doc/1_KC3/1.20_Str.en.md:3–5`.

`file_read_all` allocates exactly `sb.st_size` bytes and fills all of them
before passing the allocation to `str_init`. The nonempty-success path in
`file_read_slice` likewise allocates exactly `end - start` bytes and fills
that entire allocation. Neither reserves or writes a trailing NUL.
`str_init` only records the pointer, ownership, and size; it does not append
a terminator. Zero-initialization by `alloc` does not help after all allocated
bytes have been overwritten by non-NUL file contents.

KC3 documents strings as NUL-terminated, with the terminator outside their
counted size. A direct consumer chain is
`Time.from_str(File.read_all(path))` for a file containing a timestamp such
as `2026-10-27 00:00:00` without a trailing NUL. `time_init_str` passes the
returned pointer to `sscanf`, which expects a terminated input string and
may read past the allocation. The nonempty/non-NULL guard does not prevent
this. The nested C-function argument path evaluates into argument storage
and passes the Str directly; it does not insert a terminating string copy
(`env_eval.c:382–409`, `cfn.c:112–125,162–191`, `tag.c:1639–1643`).

**Impact / preconditions:** out-of-bounds reads through ordinary successful
file reads followed by a C-string consumer. An interior NUL can mask the
issue. No allocation failure or malformed Str metadata is required. This
does not demonstrate disclosure, code execution, or a particular network
entry point. The repaired random-string producer is not implicated here.

**Remediation direction:** validate the readable length against the Str
representation and allocation limits, reserve a separate terminator byte,
and preserve arbitrary payload bytes and the counted size. Handle an empty
file without requesting a zero-byte allocation. Keep cleanup ownership
correct independently of the slice-reader failure in F48.

**Completed fix:** both readers reserve an extra byte using word-sized
allocation arithmetic and explicitly write the trailing NUL outside the
counted payload. Lengths must fit the `u32` Str size field and `SW_MAX`;
negative file sizes, reversed/clamped-invalid slices, and oversized
lengths are rejected before allocation. An empty whole-file read allocates
one terminator byte, while an empty slice retains the existing canonical
empty-Str behavior. Embedded NULs and all other payload bytes remain
unchanged. F48's partial-read cleanup is not changed by this fix.

**Verification:** `. ./env && make test_libkc3` rebuilt the library and
passed all 511 existing C tests, with zero failures. `git diff --check`
for `libkc3/file.c` passed. The existing buffer/file tests exercise binary
whole-file reads; no new terminator-specific regression test, standalone
probe, sanitizer run, or 32-bit runtime check was added or performed.

### F48 — High: a failed slice read frees an interior pointer

**Status:** fixed in `file_read_slice`. The description and locations
below document the original audited code.

**Locations:** `libkc3/file.c:868–883`,
`lib/kc3/0.1/file.kc3:72–75`, `libkc3/alloc.c:50–60`.

`file_read_slice` stores the allocation base in `tmp`, but also uses `p` as
the advancing read cursor. Every successful read executes `p += r`.
If a later read returns zero or an error before the requested slice is
complete, cleanup calls `alloc_free(p)` instead of freeing the original
allocation base.

A concrete sequence is: allocate four bytes; the first read returns two
bytes; the next read returns EOF. The cleanup pointer is now `base + 2`,
which is not a pointer returned by the allocator. A concurrent truncation
after the size/seek checks can produce a short read followed by EOF;
a read error after partial progress reaches the same path.

**Impact / preconditions:** invalid free, with allocator abort or undefined
heap behavior, when a positive partial read is followed by failure. Failure
on the first read does not trigger this specific defect. Debug assertions
abort before the invalid free; the returned-error path is relevant to
release builds. No race or I/O failure was induced during this audit.

**Remediation direction:** retain an immutable owning base pointer and use
a separate cursor or offset. Free the base, or clean the owning temporary
Str, on every error exit. Do not free a cursor merely because it originally
held the allocation address.

**Completed fix:** the failed-read branch now calls `str_clean(&tmp)`
instead of freeing the advancing cursor. `tmp.free` retains the allocation
base throughout the read loop, so both an immediate failure and a failure
after partial progress release the correct allocation. The descriptor is
still closed, the destination is left untouched on failure, and successful
reads retain the F47 termination and ownership behavior.

**Verification:** `. ./env && make test_libkc3` rebuilt the library and
passed all 511 existing C tests, with zero failures. `git diff --check`
for `libkc3/file.c` passed. The cleanup path was checked by ownership
tracing; no partial-read race or injected read failure was run. No new
test or fault-injection machinery was added for this one-line change.

### F49 — High: character inspection leaves a dangling save record on error

**Status:** fixed in `buf_inspect_character`. The description below
documents the pre-fix behavior.

**Locations:** `libkc3/buf_inspect.c:1728–1749,5041–5042`,
`libkc3/buf_save.c:17–35,52–63`,
`libkc3/buf.c:1911–1919,1929–1950`, `libkc3/io.c:213`.

`buf_inspect_character` links its automatic `s_buf_save save` into
`buf->save`. If writing the opening quote fails, it immediately returns
without calling `buf_save_clean`. The buffer retains a pointer to an
object whose stack lifetime has ended.

An initialized zero-capacity buffer suffices to make the opening-quote
write return an error. After that call, a later save-list traversal such
as `buf_save_min_wpos` follows the dangling pointer. A later flush or
nested inspection can therefore read or update invalid stack storage.
The generic `buf_inspect_tag` character branch uses this function too.

The later failure exits have a related status defect: after restoring the
write position and unlinking the save, the function returns `result`, not
the failing `r`. With space for only the opening quote, payload failure
rolls back the output but returns a positive count of one. A caller that
checks only for a negative result can mistake that rollback for success.

**Impact / preconditions:** stack-use-after-return on reuse/traversal after
the opening write fails, plus false success for later write failures.
This concerns output-capacity or I/O-error handling, not successful
character inspection. No arbitrary-write exploit is claimed.

**Remediation direction:** balance the save record on every return path,
restore output state on failure, and return the actual failure status.
Keep successful output byte counts unchanged.

**Completed fix:** opening-quote failure now uses the existing rollback
and cleanup labels rather than returning with a linked automatic save
record. The function returns `r`, which holds the failing writer's status
on error and the complete byte count on success. Every exit after save
initialization unlinks that save; failure restores the saved write
position and line through `buf_save_restore_wpos`.

**Contract coverage:** extended the existing
`BUF_INSPECT_TEST_CHARACTER` cases instead of adding a separate test case
per failure stage. For each existing ASCII, escaped, and multibyte UTF-8
example, every undersized capacity must return an error, restore `wpos`,
and preserve an outer save record. The same buffer must then succeed at
exact-fit capacity with the expected bytes/count and intact save chain.
The checks are in-memory, deterministic, and add no test files or fixtures.

**Verification:** before the fix, `. ./env && KC3_TEST=buf_inspect make
test_libkc3` failed at the outer-save assertion for zero capacity. After
the fix, that group passed all 11 cases in 0.42 seconds, and
`. ./env && make test_libkc3` passed all 511 C tests with zero failures.
`git diff --check` passed for the modified source and test. No sanitizer
or deliberate dereference of the dangling stack record was run.

### F50 — Medium: valid Ratio casts can divide by zero after narrowing

**Status:** fixed in all ten fixed-width/word-sized integer conversions.
The description and locations below refer to the pre-fix code.

**Locations:** `libkc3/ratio.c:602–709`,
`libkc3/integer.c:862–919`, `libkc3/u8.c:72–77`,
`lib/kc3/0.1/u8.facts:4–5`.

Each fixed-width `ratio_to_s*` / `ratio_to_u*` conversion converts the
arbitrary-precision numerator and denominator separately to its small
destination type, then divides those narrowed values. The assertion only
checks that the original arbitrary-precision denominator is positive.

For the valid ratio `1/256`, `ratio_to_u8` produces numerator 1 and
denominator 0: `integer_to_u8` explicitly narrows the value returned by
`mp_get_u32`. The subsequent C integer division is by zero. The public U8
cast calls this path, so `(U8) (1/256)` is a source-derived language-level
trigger even though the intended truncated quotient, zero, fits U8.

The problem is not limited to zero divisors. `(U8) (257/2)` divides 1 by 2
and produces zero instead of the representable quotient 128. The analogous
signed/unsigned conversions at other widths use the same order of
operations; larger positive denominators can likewise narrow to zero.

**Impact / preconditions:** undefined integer division and possible process
termination for valid Ratio values, or silently incorrect results. No
invalid original denominator, OOM, or malformed serialization is needed.

**Remediation direction:** perform division in arbitrary precision first,
then apply the intended fixed-width conversion/range policy to the
quotient. Do not narrow the denominator before establishing the quotient.

**Completed fix:** a shared private helper initializes a temporary Integer
and checks arbitrary-precision division before any narrowing. Each cast
then applies its existing `integer_to_*` conversion to that quotient and
cleans the temporary. Division truncates toward zero, inputs are not
modified, and overflow/negative-to-unsigned narrowing follows Integer
cast behavior. Public signatures and floating-point conversions are
unchanged.

The scalar APIs cannot return an error status. Allocation or division
failure therefore emits a diagnostic and aborts, including in release
builds, instead of returning a fabricated numeric value. The helper checks
initialization before division and cleans an initialized temporary on
division failure.

**Test scope:** one case in the existing Ratio group compares all ten
conversions against independently specified integer quotients. Thirteen
inputs cover signed fractional truncation, zero, denominator one,
denominators that narrow to zero, operands beyond 64 bits, and quotients
at/beyond unsigned 64-bit limits. No new fixture or test file is needed.

**Verification:** before the fix, `. ./env && KC3_TEST=ratio make
test_libkc3` failed on `257/2`: S8 returned 0 instead of the Integer cast
of 128 (-128). After the fix, all four Ratio cases passed in 0.42 seconds.
`. ./env && make test_libkc3` passed all 512 C tests with zero failures
in 19.84 seconds. `git diff --check` passed for the modified source and
test. This run was on amd64; no separate 32-bit, sanitizer, or allocation
failure-injection run was performed.

### F51 — Medium: FILE flush loses the saved suffix

**Status:** fixed in `buf_file_open_w_flush`. The description and locations
below document the pre-fix code.

**Locations:** `libkc3/buf_file.c:170–207`,
`libkc3/buf_save.c:26–35,52–63`,
`libkc3/buf_inspect.c:1768–1793`, `libkc3/env.c:2191–2195`.

`buf_file_open_w_flush` writes only `buf_save_min_wpos(buf)` bytes so that
an active save can retain a not-yet-committed suffix. After a successful
`fwrite` and `fflush`, it reduces `buf->wpos` and every saved write position
by the number written, but never moves the retained suffix to offset zero.

A source-derived sequence using the existing native buffer APIs is:

1. Buffer `ABC` using raw writes, then establish a save at position 3.
2. Append `DEF`, so the buffer contains `ABCDEF` with write position 6.
3. Flush. The FILE receives `ABC`; the new write position is 3 and the
   saved write position is 0, but `DEF` is still stored at offset 3.
4. Release the save and flush again. The FILE receives `ABC` again,
   producing `ABCABC` instead of `ABCDEF`.

**Impact / preconditions:** silent output corruption on a successful
partial flush with an active save and a retained suffix. This is not a
short-write or error-only issue. Compound inspectors use buffer saves,
and environment output/error streams use the FILE backend. At discovery,
the sequence above was native-API analysis; runtime verification is
recorded below.
The separate FD and TLS implementations already move their tails; this
finding concerns the current FILE implementation.

**Remediation direction:** move the unwritten suffix to the beginning of
the buffer before treating it as the new buffered data, and keep saved
positions consistent. Preserve the backend's existing failed-write policy.

**Completed fix:** after both `fwrite` and `fflush` succeed, a nonempty
unwritten suffix is moved to offset zero with `memmove`, before the
existing write-position and saved-position adjustments. Overlapping
source/destination ranges are supported. Empty/full flush behavior and
the latched failed-write policy are unchanged; neither I/O failure branch
reaches the new move.

**Test scope:** extended `buf_write_saved` in the existing write group;
no new test case or file was added. Its original zero-prefix/full-flush
checks remain. The extension flushes one byte while retaining three
overlapping bytes under two nested saves, checks both adjusted positions
and the linked saves, verifies a blocked repeat flush leaves the tail
intact, then rolls back the inner save and appends replacement output.
After committing the outer save, readback must be exactly `abcdAbcE`.

**Verification:** before the fix, `. ./env && KC3_TEST=buf_write make
test_libkc3` failed because the retained buffer began with `A`, not `b`.
After the fix, all eight write cases passed in 0.46 seconds.
`. ./env && make test_libkc3` passed all 512 C tests with zero failures
in 19.72 seconds, including the existing FILE flush-error test.
`git diff --check` passed for the modified source and test. No sanitizer
run was performed.

### F52 — Medium: Ratio.pow ignores fractional exponents

**Status:** fixed on 2026-10-28. The description and locations below
document the pre-fix code. The implementation now resides in
`libkc3/ratio_pow.c`.

**Locations:** `libkc3/ratio.c:429–460`,
`libkc3/tag_pow.c:393–399`, `lib/kc3/0.1/ratio.facts:6–7`,
`test/ratio_test.c:113–117`.

`ratio_pow` checks that the exponent denominator is positive but never
uses its value. It converts only `b->numerator` to U32 and raises both
components of the base to that integer power.

For base `4/9` and exponent `1/2`, it consequently computes `4/9` rather
than the exact rational square root `2/3`, and reports success. This path
is exposed both as `Ratio.pow` and by the Ratio/Ratio branch of `**`.
The operator was previously misidentified in this entry: KC3 binds `**`
to `tag_pow` and binary `^` to `tag_bxor`.
The existing native exponentiation test inspected here covers `0/1`
raised to `1/1`; it does not establish fractional-exponent behavior.

**Impact / preconditions:** silently incorrect arithmetic for positive
fractional exponents, with ordinary valid values and successful allocations.
The example does not depend on negative-exponent conversion, very large
powers, or an irrational result.

**Remediation direction:** either implement the intended rational-exponent
semantics, or explicitly reject unsupported nonintegral exponents. Returning
the numerator-only power as successful evaluation is not a valid fallback.
Audit sign/range checks and arithmetic error propagation when implementing
the chosen contract; those are not proven correct by this narrow example.

**Completed fix / chosen contract:** `ratio_pow(a, b, precision_bits,
dest)` and the language binding `Ratio.pow(base, exponent,
precision_bits)` always require a positive explicit precision. There is
no default precision or implicit wrapper. The language argument is `Uw`;
for example, after `require Ratio`,
`Ratio.pow(4/9, 1/2, (Uw) 64)` returns exactly `2/3`.

For a nonzero real mathematical result `y`, the returned Ratio `r`
satisfies `abs(r - y) <= abs(y) * 2^(-precision_bits)`. Precision describes
relative accuracy, not a numerator/denominator storage limit. Exact
rational results are preserved, including rational roots and integer
powers. Both base and exponent are simplified with arbitrary-precision
GCD/division before sign, parity, and root decisions. Negative exponents
use reciprocation. Negative bases accept the real branch when the reduced
exponent denominator is odd; non-real results fail. Zero with a negative
exponent fails; a zero exponent returns `1/1`, including a zero base.

The old U32 exponent restriction and mutable static initialization are
removed. Integer powers inspect all exponent bits. The exact-root check
narrows a root degree only after proving it is no greater than the
radicand's bit length; a larger degree cannot have a nontrivial exact
integer root. Arbitrarily large degree values still reach approximation.
Available memory and the Integer backend's bit-index capacity remain
physical representation limits; failures return an error.

As requested, binary Ratio exponentiation does not invent a precision.
The Ratio-result paths in `tag_pow` return an explicit error directing
callers to `Ratio.pow(base, exponent, precision_bits)`. Other native
integer/float power paths retain their existing behavior. The C
destination is assigned only after success; failing operations clean
their temporaries and leave an existing destination intact.

**Approximation and error argument:** the algorithm uses
[NIST's logarithm series](https://dlmf.nist.gov/4.6.E4) and
[exponential series](https://dlmf.nist.gov/4.2.E19), evaluated with Integer
interval endpoints on a scale `S = 2^w`. All lower operations round down
and upper operations round up; no floating-point conversion is used.

1. Work with a positive base at least one, remembering any reciprocation
   and final sign separately. Normalize it as `2^k * m`, with
   `1 <= m < 2`. Compute `log(m) + k * log(2)`.
2. For each logarithm, `z = (m - 1)/(m + 1)` lies between zero and
   one third (including one third for `log(2)`). In
   `2 * sum(z^(2j+1)/(2j+1))`, an omitted tail starting at power `z^n`
   is at most `2*z^n/(n*(1-z^2)) <= 3*z^n`. The code adds three times
   its outward upper power bound to the upper endpoint.
3. Multiply the logarithm interval by the exact positive rational
   exponent. Divide the resulting argument by `2^s` until its upper
   endpoint is at most one half. For the positive exponential Taylor
   series, successive terms shrink by a factor at most one half, so an
   omitted tail beginning with `T_n` is at most `2*T_n`. Repeated squaring
   then restores the original argument, with outward rounding throughout.
4. For the final scaled interval `[L, U]`, accept only when
   `(U - L) * 2^p <= L`, where `p` is the requested precision. Returning
   `L/S` gives relative error at most `(U-L)/L <= 2^(-p)`. Returning
   `S/L` when reciprocation is needed satisfies the same bound. A final
   sign change preserves the absolute relative-error inequality.

Working precision includes guard bits and the exponent numerator's bit
length. If the final interval fails the requested certificate, working
precision increases and the calculation is repeated. These working bits
are internal to the algorithm and never supply a default API precision.

**Test scope:** extended the existing power case rather than adding test
cases or files. Exact-result checks cover simplification, fractional and
negative exponents, negative bases, zero, and exponent components beyond
64 bits. Forty-four approximation checks cover requested precisions of 1, 8,
64, and 128 bits across ordinary, reciprocal, negative real, near-one,
very large, and very small results. Their oracle uses exact Integer powers
and cross multiplication to verify
`r/(1+2^-p) <= y <= r/(1-2^-p)`; it does not use floating-point reference
values or the approximation algorithm itself. Explicit 128-bit vectors
for `(2/1)^(1/2)` and `(3/1)^(1/2)` also check 20-decimal rational
enclosures. Two further bounded-output
checks exercise nontrivial approximation with a numerator or denominator
larger than 64 bits. Error cases check that destination pointers and
contents survive unchanged; operator checks also preserve a successful
native integer power.

The existing interpreter Ratio fixture now includes explicit module
loading, correctly typed precision arguments, four exact power calls,
and six bounds on irrational square roots of two and three. Documentation
describes the API.
The existing vendored root implementation was added to the selected
LibTomMath build sources and the core library object list. Source lists
were regenerated with `./update_sources`, and the affected local build
configurations were regenerated.

**Verification:** before the fix, the strengthened existing Ratio case
failed on `4/9` to `1/2`, which returned the wrong numerator. After the
fix, `. ./env && KC3_TEST=ratio make test_libkc3` passed all four Ratio
cases in 0.45 seconds, including the wide-exponent checks.
`. ./env && make test_libkc3` passed all 512 C tests with zero failures
in 20.58 seconds on the final run.
`. ./env && KC3_TEST=ratio make test_libkc3_debug` also passed all four
Ratio cases with assertions enabled in 0.52 seconds.
`. ./env && IKC3_TEST=ratio make test_ikc3` passed the interpreter fixture
in 0.44 seconds on the final run. `git diff --check` passed for the
modified code, tests, documentation, and generated source lists, including
the LibTomMath changes; the new source file was checked separately with
`git diff --no-index --check`. No sanitizer, separate 32-bit target, or
allocation failure-injection run was performed.

The subsequent classic-vector extension changed only existing tests. It
passed the same focused release and debug Ratio groups in 0.45 and
0.54 seconds respectively, and the interpreter Ratio fixture in
0.44 seconds. `git diff --check` passed. The C test-case count remains 512.

### F53 — Medium: fixed-width shifts pass invalid operands directly to C

**Status:** fixed on 2026-10-29 in the shared shift dispatch and Integer
shift helpers, restored on restart baseline `9c4d66e5c`. The description
and locations below document the pre-fix code.

**Locations:** `libkc3/tag_shift_left.c:185–215,450–521`,
`libkc3/tag_shift_right.c:449–520`,
`lib/kc3/0.1/init.kc3:114–126`.

The fixed-width dispatch branches use direct C `<<` / `>>` expressions
with the caller's shift count. There is no preceding check for a negative
count or a count at least as large as the promoted left operand's width.
The right operand's type does not widen the left operand for a C shift.

On the local conventional 32-bit-int/64-bit-word target, source-derived
examples include `((U32) 1) << ((U8) 32)`,
`((U64) 1) >> ((U8) 64)`, and a U32 value shifted by `(S8) -1`.
Each selects a direct fixed-width branch and executes an undefined C shift.
Signed left-shift branches also directly shift negative values or values
whose shifted result is not representable. Integer-promotion details matter
for smaller types; the examples deliberately use U32/U64 to avoid confusing
the nominal tag width with the promoted C operand width.

**Impact / preconditions:** undefined arithmetic through the public `<<`
and `>>` operators, without allocation failure or malformed tags. Hardware
masking of shift counts does not establish a defined language result.
No memory-corruption or network-exploitation claim follows from this finding.

**Remediation direction:** define count and result semantics before
executing a shift. Validate counts and signed operands/results, or route
through defined unsigned/arbitrary-precision operations with explicit
conversion rules. Apply the policy consistently to both directions and
all operand-type combinations.

**Implemented fix:** both operators now share checked count extraction and
fixed-width dispatch. Negative counts are rejected without changing the
destination. Large unsigned and Integer counts are classified before any
narrowing, including counts larger than 64 bits. The count's type does not
change the result's type or bits.

Fixed-width shifts run in an unsigned 64-bit representation and retain the
left operand's nominal width and tag type. Left shifts discard bits outside
that width, including for negative signed operands and signed overflow.
Right shifts use zero extension for unsigned types and explicit sign
extension for signed types. Counts at least as large as the nominal width
produce zero, or minus one for a negative operand shifted right. Signed
results are reconstructed within their representable range rather than
depending on an out-of-range unsigned-to-signed cast. Bool retains the
existing normalized input and byte-sized result storage; Character uses
its U32 storage width. The operator contract is documented in
`doc/1_KC3/1.14_Integer.en.md`.

The same review found that the old Integer right-shift branches called
`integer_lshift` with a negated count, passing negative counts to
`mp_mul_2d` rather than dividing by a power of two. A proper
`integer_rshift` now uses checked division by powers of two and rounds
negative results downward, consistently with arithmetic right shift.
Counts beyond the operand's bit length return zero or minus one without
allocating proportional to the count.

`integer_lshift` rejects negative counts and nonzero results exceeding the
backend's signed-int bit capacity before calling `mp_mul_2d`; zero is
handled directly for any non-negative count. Both Integer helpers check
initialization and backend results, free temporaries on failure, and publish
the result only after success. Operand digit counts are checked before
calling the backend's signed-int bit counter; the count decoder likewise
classifies large Integer counts before computing or narrowing their bit
length. The tag dispatcher preserves destination tag metadata and
propagates helper failures.

**Current verification — 2026-10-29:** `. ./env && make test_libkc3`
passed all 502 C cases in 62.57 seconds. The focused release tag group
passed all three cases in 0.70 seconds before the final bit-counter guards;
the final full run includes that group. `. ./env && KC3_TEST=tag make
test_libkc3_debug` passed all three cases in 1.35 seconds. The existing
interpreter fixtures `integer`, `integer_band`, `integer_bnot`,
`integer_bor-2`, and `integer_bxor` passed 5/5 in 4.99 seconds using
`IKC3_TEST` with `make test_ikc3`.

The new C case covers zero/one counts across all 13 supported left and
count types, nominal-width boundaries, signed minimum values and wrapping,
negative counts, huge unsigned/Integer counts, exact Integer shifts,
arithmetic right shifts, destination metadata, and unchanged destinations
on rejected operations. Ten checks were appended to the existing integer
interpreter fixture. No new test files or standalone probes were added.

The library and root `update_sources` scripts regenerated the manifests
for the shared private header. Local LibTomMath and libkc3 build
configurations were regenerated: the old configuration omitted the
previously pulled Ratio power source and root backend from its object
lists. `git diff --check` and the separate new-header whitespace check
passed. No sanitizer, separate 32-bit target, or allocation-failure
injection run was performed.

### F54 — Medium: socket-buffer constructors disagree about who closes the FD

**Status:** fixed on 2026-10-30. The description and locations below
document the pre-fix code.

**Locations:** `libkc3/socket_buf.c:141–172,191–202,305–309`,
`libkc3/buf_rw.c:18–61`, `libkc3/buf_fd.c:39–52,55–68,169–182`,
`lib/kc3/0.1/socket/buf.kc3:27,37`.

`socket_buf_init` closes the supplied descriptor if `buf_rw_fd_open` fails,
but does not close it when buffer allocation, mutex allocation, or address
copying fails. `buf_rw_fd_close` only detaches/frees buffer adapters;
`buf_fd_close` does not close the underlying descriptor.

The two owning callers make opposite assumptions:

- `socket_buf_init_connect` has already opened and connected the socket.
  On an initializer failure it only frees `addrinfo` and returns, leaking
  the descriptor on the branches where the initializer did not close it.
- `socket_buf_init_accept` always closes the accepted descriptor after an
  initializer failure. On a `buf_rw_fd_open` failure this closes the same
  descriptor number a second time. If another thread opens a descriptor
  between those closes, the second close can affect that unrelated resource.

**Impact / preconditions:** descriptor leaks under construction failure,
and a conditional descriptor-reuse race from double close. Allocation
failure is required for the concrete adapter/buffer paths; debug assertions
may terminate before the cleanup paths. At discovery, no fault injection
or concurrent descriptor reuse was performed. This finding does not infer that the
separate successful `socket_buf_clean` API must close borrowed descriptors.

**Remediation direction:** establish one FD ownership rule across the
initializer and both owning callers, and close exactly once on every
failure path. A local temporary and a single ownership-transfer point
would make failure cleanup auditable.

**Completed fix:** `socket_buf_init` leaves the supplied FD open on every
returning failure path. It initializes a local temporary with `sockfd = -1`,
allocates and attaches its buffers and metadata there, then publishes the
destination only after every checked step succeeds. A single cleanup path
calls `socket_buf_clean` to detach adapters and release temporary resources
without closing the borrowed descriptor. The premature buffer `user_ptr`
assignments were removed so unattached buffers retain NULL adapter state.
Address-string allocation failure is now checked as well.

Both owning callers close their newly opened or accepted FD once when
initialization fails; the missing close in the connect caller was added.
Construction-failure assertions in the initializer and these caller branches
were removed so helper failures can return through cleanup in debug builds.
The header documents the ownership rule and the distinction between
`socket_buf_clean` and the explicitly closing API.

**Regression coverage:** one C case exercises five helper-level failure
points (buffers, mutex, FD adapter setup, address copy, and address string)
plus success through direct initialization, accept, and connect: 18
construction scenarios. The test uses the existing source-inclusion pattern
with renamed symbols, real buffers/mutexes/metadata and socketpair FDs, and
controlled constructor-helper and transport returns. It checks that direct
failure preserves the caller's FD, owning failures close it exactly once,
all failures leave the destination unchanged, adapters are detached before
buffer destruction, and acquired metadata is released. Successful clean
and close behavior and connect's addrinfo cleanup are also checked.

The test does not exhaustively inject failures inside each helper's own
allocations. In particular, read-adapter rollback after write-adapter
allocation failure in `buf_rw_fd_open` was verified by source tracing.
No concurrent descriptor-reuse race, sanitizer, separate platform, or actual
process-wide allocation exhaustion was run.

**Verification:** against a temporary copy of the pre-fix initializer,
the new case failed at FD-adapter setup: expected zero closes for the direct
initializer, observed one. The committed source was not changed for that
comparison; the temporary source snapshot was removed afterward.
After the fix, `. ./env && KC3_TEST=socket_buf make test_libkc3` passed
its one case in 0.51 seconds, and the corresponding debug group passed in
0.61 seconds. `. ./env && make test_libkc3` passed all 503 C cases in
32.59 seconds. After rebuilding `kc3s`, `. ./env && cd test && ./socket_test`
passed the existing listen/connect/accept/close fixture (1/1).

Test and root source manifests were regenerated, and the local test build
configuration was regenerated to register the new file. `git diff --check`
and separate whitespace checks for the new test and audit log passed.

### F55 — Low: FD-backed tell reports the wrong logical read position

**Status:** fixed on 2026-10-30. The description and locations below
document the pre-fix code.

**Locations:** `libkc3/buf_fd.c:55–67,149–166`,
`libkc3/buf.c:1758–1763`, `libkc3/buf_file.c:136–150`.

`buf_fd_open_r_tell` returns `lseek(fd, 0, SEEK_CUR) + buf->rpos`.
The descriptor offset is already at the end of the read-ahead buffer,
so the logical position is instead that offset minus `buf->wpos` plus
`buf->rpos`. The corresponding FILE-backed implementation uses this
subtraction correctly.

For a fresh buffer that reads eight bytes from offset zero, with one byte
then consumed, the descriptor offset is 8, `wpos` is 8, and `rpos` is 1.
`buf_tell_r` returns 9 through the installed FD callback, although the next
logical byte is at offset 1.

**Impact / preconditions:** incorrect position reporting on a seekable
descriptor after read-ahead. This is a native-API finding; no language-level
consumer or network impact is established. Unseekable sockets fail the
`lseek` call instead of reaching this particular incorrect result.

**Remediation direction:** account for the buffered window as the FILE
backend does, including after compaction. Keep the FD and FILE logical
read-position contracts consistent.

**Completed fix:** `buf_fd_open_r_tell` now returns the descriptor offset
minus `wpos` plus `rpos`, matching the FILE backend. Subtracting the buffered
window accounts for bytes fetched from the FD but not yet consumed.

**Regression coverage:** the new `buf_fd_read_tell` case in the existing
FD test file checks positions before and after every byte read across
multiple eight-byte buffer windows, starting at file offsets zero and
three. It also checks the logical position after EOF, explicitly confirms
that an active save was adjusted by compaction, restores that save, and
checks both the restored position and the next byte/position. No new test
file, fixture, source manifest, or standalone probe was needed.

**Verification:** before the fix, `. ./env && KC3_TEST=buf_fd make
test_libkc3` failed in the new case after the first byte: expected position
1, observed 9. After the fix, the same group passed all nine cases in
0.58 seconds. `. ./env && KC3_TEST=buf_fd make test_libkc3_debug` passed
all nine cases in 0.64 seconds. `. ./env && make test_libkc3` passed all
504 C cases in 29.07 seconds. `git diff --check` and the audit log's separate
whitespace check passed. No sanitizer or separate platform run was performed.

### F56 — High: decoded-list and tuple caches retain freed addresses

**Status:** fixed on 2026-10-30. The initial list finding below describes
the pre-fix code. The continuation also confirmed and fixed the analogous
cached-tuple lifetime defect in `marshall_read_ptuple`.

**Locations:** `libkc3/marshall_read.c:1550–1629,2307–2348`,
`libkc3/plist.c:382–390`, `libkc3/list.c:76–99,285–310`,
`libkc3/tag.c:293`, `libkc3/env.c:2689–2706`,
`test/marshall_read_test.c:1289–1331`.

On the first occurrence of a serialized list, `marshall_read_plist`
decodes a new list, stores its address with `marshall_read_ht_add`, and
hands that same allocation to the caller. The cache stores the pointer
as a `Uw`; it neither owns a list copy nor retains the list through an
owning tag. On a cache hit, `plist_init_copy` traverses the cached list.

A source-derived failing sequence needs no malformed offsets:

1. Serialize the same nonempty list twice with one marshaller. The writer
   reuses the first list's heap offset (`marshall.c:1366–1417`).
2. Read the first tag and clean it before reading the second. Cleaning a
   list tag frees its list nodes, but does not remove the cached addresses.
3. Read the second tag. The cache returns the freed pointer, and
   `list_new_copy_all` reads its tags and links.

`env_load_kc3c` uses precisely the read/evaluate/clean ordering that can
expose this lifetime mismatch when top-level records share a list offset.
The existing `marshall_read_cache_shared` test reads both results before
cleaning either, so it does not exercise early destruction of the first
result. The cache's failure flag does not help: the first decode and its
cleanup are successful operations.

**Impact / preconditions:** use-after-free when a later reference in the
same decoding context names a list released by an earlier consumer.
This can affect valid shared-object encodings and does not require an
allocation failure. The continuation established the list path and the
related tuple path described below. No remote exploit is claimed; this
finding makes no blanket ownership claim about other cached pointer types.

**Remediation direction:** give cached objects an explicit lifetime that
covers every possible cache hit. For lists, retain an independently owned
canonical representation and copy results from it, or introduce a sound
ownership/invalidation scheme. Do not require existing consumers to keep
all previous results alive without enforcing that contract.

**Related tuple defect:** `marshall_read_ptuple` likewise cached the
first allocation and handed it to the caller without a cache-owned
reference. With `pass_by_copy` enabled, the second result is a distinct
copy. Cleaning the first result therefore frees the cached tuple even
while the second remains alive, and a third read traverses freed storage.
The expanded lifetime test encountered an invalid tuple count on that
third read before tuple retention was added.

**Completed fix:** every decoded list node is now retained by the decoder
in its existing retention pool. Canonical next links may borrow other
retained nodes; public reads copy list nodes on both first decode and cache
hits. This keeps ordinary flat-list canonical storage linear in the node
count rather than retaining an independent full copy of every suffix.
Head payloads and non-list dotted tails retain their normal ownership.

Private list/tag readers allow borrowing only for the next field of a
canonical list node. Other readers, including custom marshaller callbacks,
continue to receive owned list copies. Cache cleanup detaches borrowed
next links and destroys each canonical node once, including its tag
metadata. The same cleanup runs on header reset and decoder destruction.
Failed decodes keep the existing failed-cache latch, and already-retained
nodes remain owned until cleanup.

Cached tuples now also have a retained owning tag in the pool. First
results, like cache hits, go through `ptuple_init_copy`: by-copy mode
returns a copy, while reference mode gives the caller its own reference.
The existing inline, uncached tuple format retains its ownership behavior.
No wire format, public signature, or decoder struct layout changed.

The now more frequently used `list_new_copy_all` path also checks whether
each node allocation succeeded before dereferencing it. On that failure,
its existing cleanup path releases any partial copy and reports failure.
The public list decoder header documents the caller's node ownership.

**Regression coverage:** extended the existing shared-cache case, retaining
its original tuple, list, and symbol vectors and adding empty, dotted,
and nested/mixed lists. All six run with both `pass_by_copy` settings.
The test mutates the first returned list, checks a second result against
the source, then frees early results before later reads of the same
serialized offset. Explicit suffix and distinct-parent records exercise
shared canonical tails. Re-reading the header clears the old cache while
results remain live; another decode and final decoder cleanup must leave
all retained caller results valid.

The existing cycle case now also rejects a list whose head contains an
already-decoded nested list and whose tail points back to itself. This
checks destination preservation, the failed-cache state, and cleanup after
partial canonical retention. No new test file, fixture, standalone probe,
or source manifest was added.

**Verification:** before the fix, the expanded shared-cache case failed
because mutating the first list changed the second decoded result
(`compare_tag` expected zero, observed one). This was a deterministic
aliasing check before freeing the list; no deliberate pre-fix traversal of
a freed list was run. The related tuple failure was encountered while
extending early-destruction coverage, as described above.

After the final changes, `. ./env && KC3_TEST=marshall_read make
test_libkc3` passed all 65 reported serialization tests in 1.82 seconds.
`. ./env && MALLOC_OPTIONS=J KC3_TEST=marshall_read make test_libkc3_debug`
passed all 60 debug-enabled tests in 8.33 seconds on OpenBSD.
`. ./env && make test_libkc3` passed all 492 reported C tests in
92.31 seconds. The existing marshalling interpreter fixture passed 1/1
in 1.51 seconds using `IKC3_TEST=marshall make test_ikc3`.

The total changed from 504 to 492 because twelve previously counted
`TAG_TEST_EQ` successes became fail-fast `TEST_EQ` assertions in the
extended shared-cache case. No registered test case or original input
vector was removed; the harness counts `test_ok` calls rather than just
registered cases. `git diff --check` and the audit's whitespace check passed.
ASan was not run: both local configurations report `HAVE_ASAN = false`.
No separate platform run or allocation-failure injection was performed.

### F57 — High: integer wire lengths wrap before bounds checking on 32-bit targets

**Status:** fixed in `marshall_read_integer`. The description and locations
below document the pre-fix code.

**Locations:** `libkc3/marshall_read.c:1828–1895,3474–3475`,
`libkc3/types.h:79–91`, `libtommath/bn_mp_unpack.c:9–40`,
`libkc3/facts_connection.c:182–185,578–604`.

`marshall_read_integer` reads an unrestricted `u32 count`, then computes
`size = count * digit_size`, with `digit_size` and `size` both `sw` and
`digit_size == 4`. `sw` is `intptr_t`. On a 32-bit target the multiplication
is performed in a 32-bit unsigned type after the usual conversions; it
can wrap before either buffer-length check.

For example, `count == 0x40000001` produces `size == 4`. A small integer
record containing only four payload bytes therefore passes the local
availability check. The subsequent `mp_unpack` call still receives the
original count of 1,073,741,825 four-byte words. Its byte-address calculation
and loop use that count, not the checked size, and access outside the
provided payload/buffer. No correspondingly large input allocation is
required. The enclosing header limits bound the actual message sizes,
not this per-integer count, and therefore do not prevent the mismatch.

**Impact / preconditions:** out-of-bounds reads and undefined pointer
arithmetic while decoding a malformed integer on a 32-bit build. i386
is a documented supported architecture. The same count multiplication
does not wrap on a 64-bit `sw`, so this is not a claim of that specific
overflow on the current 64-bit configuration.

The decoder is reachable from `MarshallRead.tag_from_str` and from fact
decoding. The replication thread reads facts through
`marshall_read_fact` → `marshall_read_ptag` → `marshall_read_tag` →
`marshall_read_integer`; connection setup checks authentication first.
Thus the traced network path requires an admitted peer on an affected
build, not an unauthenticated arbitrary connection. No network payload
or 32-bit runtime test was executed.

**Fix and verification — 2026-10-31:** the decoder compares `count` with
`SW_MAX / digit_size` in `u64` before any narrowing or multiplication.
Only a validated count is cast to `sw` and multiplied. On a 32-bit target,
the maximum accepted count is `0x1fffffff`, whose byte length is
`0x7ffffffc`; `0x20000000` and all larger counts are rejected before
integer initialization, refill, or `mp_unpack`. The count passed to
`mp_unpack` therefore describes exactly the validated byte span.

Payload checks use `wpos - rpos`, with position ordering checked before
subtraction, both before and after refill. A refill can compact the buffer,
so the final check uses the updated positions. Truncated payloads return
NULL and clean the temporary integer in debug and release builds; the
old short-read assertion was removed. Failure leaves the destination
unchanged. Zero, negative, and ordinary multiword encodings keep their
existing format and behavior.

One registered case in `test/marshall_read_test.c` exercises both heap and
data-buffer selection. It covers eight valid values, including zero,
negative values, the 28-bit word boundary, and values beyond 64 bits.
Eleven malformed count/payload combinations include partial words,
`0x1fffffff`, `0x20000000`, `0x3fffffff`, `0x40000000`, the reported
`0x40000001`, `0x80000000`, and `U32_MAX`. Each rejection preserves a
preinitialized destination and stops before consuming payload bytes.
Four additional refill paths cover an I/O error, EOF, a partial word
followed by EOF, and successful completion after compaction. No new test
file or source manifest was needed.

On native 64-bit OpenBSD, `. ./env && KC3_TEST=marshall_read make
test_libkc3` passed all 66 reported serialization tests in 1.06 seconds.
The debug equivalent passed all 61 enabled tests in 1.26 seconds.
`. ./env && make test_libkc3` passed all 493 reported C tests in
41.76 seconds. `IKC3_TEST=marshall make test_ikc3` passed the existing
marshalling fixture, 1/1, in 1.45 seconds. `git diff --check` and the audit
whitespace check passed.

No 32-bit runtime test or pre-fix memory-safety reproduction was performed.
The 32-bit overflow fix was checked by tracing the types and bounds. On
the native 64-bit run, the large forged counts are rejected for insufficient
payload; the 32-bit count-limit branch still needs a separate platform run.
ASan was unavailable (`HAVE_ASAN = false` in both local configurations).
No network test or allocation-failure injection was performed.

### F58 — Medium: division and remainder pass invalid operands directly to C

**Status:** fixed for pairs of fixed-width integer operands in `tag_div`
and `tag_mod`. The description and locations below document the pre-fix
code. The later F59 and F60 fixes are recorded below.

**Locations:** `libkc3/tag_div.c:481–495,688–701,842–855`,
`libkc3/tag_mod.c:531–545,713–726`,
`lib/kc3/0.1/init.kc3:66–82`.

The fixed-width branches of `tag_div` and `tag_mod` evaluate C `/` and `%`
without checking the divisor or the signed minimum/-1 overflow case.
The public `/`, `mod`, and `%` operators bind directly to these functions.

Source-derived examples are:

- `(U8) 1 / (U8) 0` and `(U8) 1 mod (U8) 0`: integer division or
  remainder by zero, including after integer promotion.
- `S64_MIN / (S8) -1`: the S64/S8 branch performs signed 64-bit division
  whose quotient is not representable.
- `S64_MIN mod (S64) -1`: C also makes this remainder operation undefined,
  even though the mathematical remainder would be zero.

The expressions above describe operand values and types; they were not
executed. Unlike F50, no valid Ratio denominator is narrowed into zero:
these branches operate directly on fixed-width integer operands.

**Impact / preconditions:** undefined behavior, potentially terminating
the interpreter, from ordinary arithmetic operands. There is no checked
failure before the C operation, in either debug or non-debug builds.
No memory-corruption consequence or network execution path is established.

**Fix and verification — 2026-10-31:** a shared private helper in
`libkc3/tag_divmod.h` reads the sign and unsigned magnitude of the ten
supported fixed-width integer types. Unsigned subtraction forms a negative
operand's magnitude without negating `S64_MIN` in a signed type. Both
operators reject a zero divisor before their existing dispatch runs and
return NULL without changing the destination. This covers all 100
fixed-width type pairs, including branches that otherwise convert their
operands to Integer.

Division of a negative fixed-width value by signed `-1` uses its unsigned
magnitude and the existing integer reducer. This returns exact, suitably
sized positive results for every signed minimum, including S8/S16 cases
where C promotion avoids division overflow but the old result was narrowed
back into the signed operand type. Fixed-width remainder by signed `-1`
returns U8 zero directly, avoiding C `%` for the signed-overflow case.

Review also found three 32-bit cast hazards in division: Sw/U32 can turn
`U32_MAX` into signed `-1`; U32/Sw can turn `0x80000000` into `SW_MIN`;
and Uw/U64 can truncate a divisor such as `0x100000000` to zero. The first
two expressions now calculate in S64 and the last in U64. The related
U64/Uw result stays U64 until reduction, preventing quotient truncation
on a 32-bit build. These changes preserve the operands' mathematical
values before the C operation.

One registered `tag_divmod` case covers all 25 signed type pairs at
minimum/-1, all 100 fixed-width pairs with zero divisors, ordinary negative
division/remainder, and four word-size conversion boundaries. It checks
result values and types, preserved destination metadata on success, and
an unchanged destination on rejected zero divisors. Separate F64/fixed
zero checks retain the existing infinity/NaN behavior. Eight checks added
to the existing integer interpreter fixture cover widened quotients,
zero remainders through both `mod` and `%`, and ordinary negative operands.
The English integer documentation records the fixed-width contract. Source
manifests include the new private header; no new test file was added.

Before the fix, the new case failed safely at S8 minimum/-1: the expected
U8 result type was returned as S8. It stopped there, before any zero-divisor
or signed-overflow reproduction was attempted. After the fix,
`. ./env && KC3_TEST=tag make test_libkc3` passed all four tag cases in
0.74 seconds; the debug equivalent passed all four in 0.76 seconds.
`. ./env && make test_libkc3` passed all 494 reported C tests in
32.09 seconds. `IKC3_TEST='integer integer_div integer_mod-2' make
test_ikc3` passed all three interpreter fixtures in 1.35 seconds.
`git diff --check` and whitespace checks for the audit and new header
passed.

Verification ran on native 64-bit OpenBSD. The 32-bit cast corrections
were source-checked and have portable regression vectors, but no 32-bit
runtime run was available. ASan was unavailable (`HAVE_ASAN = false`).
No allocation-failure injection was performed at this step. Integer
helper/reducer failure propagation was left for F59, whose fix is recorded
below; the wrong-result-object branches were left for F60. This fix does
not establish correctness of all other arithmetic combinations or remainder
sign conventions.

### F59 — Medium: big-integer errors become successful zero results

**Status:** fixed in the Integer division/modulo helpers, their dispatch
branches, and the shared integer reducer. The description and locations
below document the pre-fix code. F60's later fix is recorded below.

**Locations:** `libkc3/tag_div.c:274–278,1184–1192`,
`libkc3/tag_mod.c:220–224,1034–1040`,
`libkc3/integer.c:219–234,479–493`,
`libkc3/tag.c:872–883`, `libtommath/bn_mp_div.c:92–95`,
`libtommath/bn_mp_mod.c:12–29`, `libtommath/bn_mp_init.c:8–25`.

The Integer/Integer dispatch branches ignore the return values of
`integer_div` and `integer_mod`, then unconditionally reduce their
temporary result and return success. Both helpers initialize their
destination to zero before invoking LibTomMath. Division by zero makes
`mp_div` return `MP_VAL` without writing a result; `mp_mod` propagates that
failure as well.

In a non-debug build, with allocations succeeding, `(Integer) 1 /
(Integer) 0` consequently logs an error but returns a successful U8 zero:
the caller reduces the initialized, otherwise untouched temporary.
`(Integer) 1 mod (Integer) 0` follows the same path. Debug builds abort
at the helper assertion instead. This behavior is source-derived, not
an observed interpreter run.

**Impact / preconditions:** arithmetic failure is concealed from callers
as a normal numeric result. This does not depend on memory exhaustion.
It is distinct from F58: the multiprecision implementation detects the
invalid operation; the KC3 dispatcher discards its failure return.
Other ignored helper returns exist, but their allocation-failure effects
have not been exhaustively traced in this pass.

**Fix and verification — 2026-10-31:** every direct Integer constructor
and arithmetic call in `tag_div` and `tag_mod` now checks its return value
(101 calls in division and 119 in modulo). Failure enters a shared cleanup
path for the zero-initialized operand temporaries and result tag, without
reducing or publishing a failed result. This includes mixed fixed-width
conversions and branches that convert a modulo result back to the divisor's
type. Existing successful cleanup paths remain in place.

`integer_div` and `integer_mod` now initialize a local result, check
initialization and the backend operation, free that result on backend
failure, and publish it only on success. A failed operation leaves the
caller's destination unchanged. The ten fixed-width Integer constructors
and the zero constructor now stop when `integer_init` fails. Recoverable
backend errors in division/modulo and initialization/copy initialization
return NULL with diagnostics in debug and release builds.

`tag_integer_reduce` checks fixed-width temporary initialization and
returns the reducer's actual result. Its Integer case uses `integer_reduce`
and the existing transactional `tag_init_integer_copy` path, so failure to
copy a large result cannot first overwrite the destination's type.
The outer division dispatcher no longer asserts on a reducer failure.
The C header records the division/modulo destination contract; the English
integer documentation now includes arbitrary-precision zero divisors.

One registered case in `test/tag_test.c` checks all 21 type pairs involving
Integer and the ten fixed-width types, for both zero-divisor rejection and
valid quotient/remainder results. It also checks direct helper failures
against a preinitialized Integer destination, signed Integer arithmetic,
and exact results beyond 64 bits. Failure leaves the destination's full
object representation unchanged; successful tag results preserve caller
metadata.

The same case includes production dispatcher source under renamed test
symbols, substituting checked wrappers at constructor, arithmetic, cleanup,
and reduction boundaries. Seven representative paths in each operator
have successful controls and 46 total injected helper failures. The tests
check immediate propagation, unchanged destinations, and zero outstanding
owned scratch integers after each call. Both one- and two-conversion paths
and modulo's separate result temporary are exercised. No allocator or
production hook was introduced. The local test dependencies were regenerated
for the included source files. No new test file or source manifest was
needed.

Eight added interpreter checks verify diagnostics and Void for Integer or
mixed-type zero divisors through `/`, `mod`, and `%`, plus an exact large
quotient. Before the fix, the new C case failed because `tag_div` returned
a success pointer after the backend's zero-divisor error. After the fix,
`. ./env && KC3_TEST=tag make test_libkc3` passed all five tag cases in
1.03 seconds; the debug equivalent passed all five in 2.07 seconds.
`. ./env && make test_libkc3` passed all 495 reported C tests in
88.05 seconds. `IKC3_TEST='integer integer_div integer_mod-2' make
test_ikc3` passed all three fixtures in 2.58 seconds; the updated integer
fixture also passed under `make test_ikc3_debug`, 1/1 in 1.50 seconds.
`git diff --check` and the audit whitespace check passed.

Verification ran on native 64-bit OpenBSD. Injected failures occur at the
KC3 helper boundaries; raw backend allocator failures and a failing large
result-copy allocation were not injected. Those lower-level paths were
source-checked. ASan was unavailable (`HAVE_ASAN = false`); no separate
platform or network run was performed. Ratio/Complex operations, other
arithmetic dispatchers, and all remaining unchecked helper calls outside
the traced Integer division/modulo paths remain outside this fix's claim.

### F60 — Medium: mixed-type division reduces an untouched temporary

**Status:** fixed in the S8/Ratio and Uw/S16, Uw/S32, and Uw/S64 branches
of `tag_div`. The description and locations below document the pre-fix code.

**Locations:** `libkc3/tag_div.c:20–26,475–480,1100–1140,1184–1192`,
`libkc3/tag.c:866–945`, `libkc3/cfn.c:163–187`.

Several branches calculate their answer into `dest` and then jump to
`integer_reduce`, which operates on the separate local `tag`. That local
remains its initial `TAG_VOID` value. `tag_integer_reduce` rejects it,
so an otherwise successful division returns NULL, or aborts in debug.

Confirmed branches and source-derived examples:

- S8/Ratio: `(S8) 1 / (1/2)` computes `2/1` into `dest`, cleans the
  converted input, then incorrectly enters the integer-result path.
- Uw/S16, Uw/S32, and Uw/S64 when the Uw value fits S64: for example,
  `(Uw) 6 / (S64) 2` writes 3 into `dest` but then tries to reduce Void.

The FFI's null-result handling initializes the language result to Void
and cleans the partially produced output, so these valid operations do
not return their numeric answers in a non-debug interpreter. The cleanup
also means this finding does not claim a leak through that FFI path.

**Impact / preconditions:** deterministic incorrect failure/Void results
for valid, small operands, without allocation failures or malformed data.
This is separate from F59's ignored failing helper: here the arithmetic
can succeed and the wrong object is subsequently validated.

**Fix and verification — 2026-10-31:** the three affected Uw/signed
branches now calculate their S64 quotient into the local result tag that
the shared integer reducer consumes. This produces the expected reduced
integer value and type, and avoids writing the caller's destination before
reduction succeeds. Existing large-Uw multiprecision branches remain in
place; the same tests exercise them on the native 64-bit build.

The S8/Ratio branch checks conversion and division, calculates into an
owned local Ratio result, cleans the converted input, and publishes the
result directly after success. It retains Ratio type even when the reduced
denominator is one. It never enters the integer-only reducer. On failure,
the converted input and local result are cleaned and the destination is
unchanged. A zero Ratio divisor is rejected before the Ratio helper's
nonzero-numerator assertion. This zero guard applies to the S8/Ratio branch.

`ratio_init_s8` now checks both Integer constructors, frees its numerator
if denominator initialization fails, and publishes its completed local
Ratio only on success. Other Ratio constructors and the internals of
Ratio arithmetic remain outside this fix's general failure-safety claim.

One registered `tag_div_mixed` case covers eleven value/sign/boundary
combinations for each of S16, S32, and S64 divisors, including zero
quotients, truncation toward zero, the signed-word boundary, `UW_MAX`,
and large negative results. Seven S8/Ratio combinations cover zero and
negative dividends, positive and negative divisors, S8 limits, and exact
fractional results. It checks result types, values, caller metadata,
Ratio-result ownership after input cleanup, and an unchanged destination
for a zero Ratio divisor.

The existing included-dispatcher failure wrappers now also count owned
Ratio components. Three injected Uw reducer failures and two injected
Ratio constructor/division failures check immediate propagation, unchanged
destinations, and cleanup. Successful controls verify that Ratio results
bypass the integer reducer and transfer ownership to the caller. No raw
allocator failure was injected; the inner `ratio_init_s8` initialization
failure paths were source-checked. No new test file or source manifest
was needed.

The pre-fix release run failed at `(Uw) 6 / (S16) 2`: the dispatcher
reported that its local result was not an integer and returned NULL after
calculating the quotient. No deliberate debug abort was run. After the
fix, `. ./env && KC3_TEST=tag make test_libkc3` passed all six tag cases in
0.54 seconds; the debug equivalent passed all six in 0.63 seconds.
`. ./env && make test_libkc3` passed all 496 reported C tests in
28.37 seconds. `IKC3_TEST='integer integer_div integer_mod-2 ratio' make
test_ikc3` passed all four fixtures in 1.57 seconds. The updated integer
and Ratio fixtures also passed under `make test_ikc3_debug`, 2/2 in
0.97 seconds. The sixteen added interpreter checks cover restored values,
result types, and the S8/Ratio zero-divisor diagnostic/Void behavior.
`git diff --check` and the audit whitespace check passed.

Verification ran on native 64-bit OpenBSD. The word-boundary expectations
also describe 32-bit targets, but no 32-bit runtime run was available.
ASan remained unavailable. These checks close the recorded F60 branches;
they do not establish correctness of every mixed-type arithmetic branch
or backend allocation-failure path.

## Disposition and limits

- F47–F60 are fixed in the current checkout. The C suite reports 496
  passing tests; F56 explains the earlier changed subtest counter. The
  serialization and arithmetic cases cover the contracts recorded above.
  F57 and F58 still need runtime verification on a 32-bit target; F60's
  32-bit word-boundary expectations also remain unexecuted.
  Further work requires continuing the full-library audit, starting with
  the remaining arithmetic failure paths and ownership contracts below.
- The Base32 termination candidate was ruled out: `buf_init_alloc`
  reserves an extra zero-filled byte, and `buf_to_str` releases the buffer's
  lock while transferring its bytes. It was not promoted to a finding.
- The report makes no blanket safety claim about other inspected paths.
  Arithmetic failure propagation outside the traced division/modulo
  branches, remaining parser/inspector rollback, other serialized-object
  lifetimes, concurrency, and further platform-specific behavior still
  need deeper review.
- Passing existing tests does not establish coverage of every path.
  F47–F60 verification and its platform/failure-injection limits are
  recorded above. No full-library safety claim is made.
