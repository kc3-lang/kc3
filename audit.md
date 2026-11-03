# libkc3 fresh audit — 2026-11-02

**Baseline:** `0a4a74ae5` (`audit`). The working tree was clean at the
start of this pass. This is a new report, continuing numbering after F64;
it does not restore the deleted audit history.

**Progress:** F65–F72 are fixed, with regression tests. F71 is committed
in `08bd1f7f9`; the F72 repair is in the working tree. No confirmed
findings in this report remain open. Resume the broader `libkc3` audit;
it remains incomplete. The additional `lib` build and HTTPd dump-restore
failures are repaired and verified in the dated sections below.

## Scope and method

Source review covered pointer-decoder caches and their owning wrappers,
Complex arithmetic/error propagation and inspection, Integer/Ratio floating
conversions, buffer transfers, and selected array/map/string bounds.
These were focused checks, not an exhaustive review of all `libkc3` paths.

Runtime checks used native OpenBSD amd64 and the release library/interpreter.
`. ./env && make ikc3` succeeded. Temporary C probes were compiled with
`test/build_c_main` and linked against the repository's `libkc3`.
F80 is enabled; F128 and ASan are unavailable in this configuration.
Core dumps were disabled for the two deliberate crash reproductions.
The initial audit did not run the full C/interpreter suite, raw
allocator-failure injection, separate platform tests, or network
reachability tests. Subsequent repair verification is recorded below.

High marks confirmed native object-lifetime or out-of-bounds defects under
the stated conditions. Medium marks incorrect arithmetic or error handling,
including a demonstrated null-pointer interpreter crash. Priority does
not establish remote reachability or exploitability.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F65 | High | Callable/Frame/Var/Facts/Struct decoder caches retain freed addresses | Fixed; regression tests pass in both copy modes |
| F66 | High | `buf_xfer` length arithmetic wraps before capacity checks | Fixed; overflow and callback regressions pass |
| F67 | Medium | Failed Complex division is published as a null Complex | Fixed; checked dispatch and regression coverage |
| F68 | Medium | Ratio-to-float conversion overflows components before division | Fixed; exact quotient rounding and cast regressions |
| F69 | Medium | Extended Integer/float conversions narrow through F64 | Fixed; native precision/range and cast regressions |
| F70 | Medium | Complex add/subtract/multiply discard scalar-operation failures | Fixed; checked components, construction, and tag dispatch |
| F71 | Medium | Floating infinity inspection does not terminate | Fixed; non-finite writers and size calculations agree |
| F72 | Medium | Unary negation rejects floating values | Fixed; native floating negation preserves type, signs, and tag metadata |

## Findings

### F65 — High: decoder caches retain freed Callable/Frame/Var/Facts/Struct objects

**Baseline locations:** `libkc3/marshall_read.c:1605,1631` (raw cache entries and
retention cleanup), `:2157` (`marshall_read_pcallable`), `:2275`
(`marshall_read_pfacts`), `:2328` (`marshall_read_pframe`), `:2744`
(`marshall_read_pstruct`), and `:2968` (`marshall_read_pvar`). Copy/delete
contracts are in `pcallable.c`, `callable.c`, `pframe.c`, `frame.c`,
`pvar.c`, `var.c`, `pfacts.c`, `facts.c`, `pstruct.c`, and `struct.c`.

On a cache miss, these five decoders allocate an object, put its numeric
address into the reader's offset hash table, and give its only ownership
to the caller. They do not retain the decoded object in the reader's
owning pool. Cleaning the first result therefore leaves a dangling
address in the still-live reader.

A later reference to that offset finds the dangling address and passes it
to the type's copy/reference helper. Callable, Var, Facts, and Struct
helpers read or update freed storage; a Frame hit may copy freed storage
or return the dangling pointer, depending on copy mode.

**Preconditions / impact:** valid serialized shared references and a
caller releasing the first result before a later read. Malformed data
and allocation failure are unnecessary. The ordinary later read can
perform a use-after-free. The existing Call/Complex retention repair does
not cover these types.

**Runtime verification:** for each type, a C probe serialized the same
source twice, decoded the first result, saved its numeric address, and
deleted it. The four reference-counted types had reference count one
before deletion; Frame deletion unconditionally frees the object.
The probe then consumed the second pointer marker and called only
`marshall_read_heap_pointer` to retrieve the cached numeric address.
All five addresses remained cached with `pass_by_copy=false` and again
with `pass_by_copy=true` (ten observations).

Sources were an ordinary parsed `fn (x) { x }`, an empty Frame, an unbound
U8 Var, an empty Facts database, and a Struct with a valid one-U8-field
StructType. The probe stopped before the normal cache-hit copy/reference
operation, so it did not dereference freed memory or depend on allocator
reuse. Reader and source cleanup completed successfully.

**Repair — 2026-11-02:** fixed in `libkc3/marshall_read.c`.
Callable, Var, Facts, and Struct now retain canonical objects in the
reader's owning tag pool. Both first reads and cache hits use the type's
copy/reference helper to return caller-owned values.

Frame now retains one canonical node per offset using dedicated cleanup
in the same pool. Canonical `next` links borrow other retained nodes;
cleanup deletes each node once without following those links. Public
Frame reads return independently owned frame chains in both copy modes,
because `frame_new_ref` does not increment a reference count. Direct
`marshall_read_frame` results also own their `next` chains. No public
structure layout or wire-format change was needed.

Added `marshall_read_cache_owned` and `marshall_read_cache_frame` in
`test/marshall_read_test.c`. They exercise all five types in both modes:
release every initial result before later cache hits, keep results alive
through a conflicting-type error and header reset, then verify and clean
them after reader destruction. Sources include a function with a captured
binding, a bound Var, populated Facts, a Struct with an allocated string
field, and Frame chains with shared suffixes and distinct parents. Copy
mutation checks cover Callable, Struct, and Frame; Frame checks also cover
the direct non-pointer decoder.

**Verification:** before the production repair, the two new cases failed
safely on the original code: Callable reference count was one rather than
two, and the first Frame result aliased its canonical cache entry. After
the repair:

- Release decoder suite: **69/69** tests pass.
- Debug decoder suite: **64/64** tests pass (five release-only cases are
  excluded by the existing debug guards).
- Full release C suite: **504/504** tests pass.
- Release interpreter fixtures `marshall`, `fn`, `var`, `struct`, and
  `facts`: **5/5** pass.
- `git diff --check` and the audit file whitespace check pass.

Logs: `/tmp/kc3-f65-before.log`, `/tmp/kc3-f65-release.log`,
`/tmp/kc3-f65-debug.log`, `/tmp/kc3-f65-full.log`, and
`/tmp/kc3-f65-ikc3.log`. ASan remains
unavailable in this native configuration; allocator-failure injection
and other platforms were not tested for this repair. General Frame
sharing outside the decoder is unchanged and remains a separate review
area.

### F66 — High: `buf_xfer` wraps its bounds checks before an unbounded copy

**Baseline locations:** `libkc3/buf.c:2174–2210`; declaration at
`libkc3/buf.h:185`. Existing `test/buf_test.c:1356` exercises small
transfers, including ordinary insufficient-source requests.

The source check adds `src->rpos + size`; the destination check adds
`dest->wpos + size`. Both operands use unsigned word arithmetic. A large
requested length can wrap both sums and skip both refill/capacity checks.
The subsequent `memcpy` still receives the original, unbounded length.

**Runtime reproduction:** initialize two ordinary eight-byte buffers,
with source `rpos=1`, `wpos=2` and destination `rpos=0`, `wpos=1`.
A one-byte control transfer returns one. Reset those positions and call
`buf_xfer(&dest, &src, UW_MAX)`.

On amd64 the probe printed:

```text
one-byte control: 1
request=18446744073709551615 source end=0 destination end=0
```

Both wrapped ends compare within capacity, and the native process then
terminated with status 139 in the oversized transfer. The buffers and
positions were valid before the call; the requested length was oversized.
No huge allocation or forged buffer metadata was needed.

**Preconditions / impact:** a caller supplies a sufficiently large `uw`
length. This bypasses the function's bounds checks and reaches an
out-of-bounds read/write. The probe demonstrates a crash; a language-level
or network input path supplying that length has not been established.
The presence of internal serialization callers does not establish that
their separately validated lengths can trigger this defect.

**Repair — 2026-11-02:** fixed `buf_xfer` in `libkc3/buf.c`.
Requests above `SW_MAX` and invalid source/destination positions now
return `-1` before invoking callbacks or changing data or positions.
Availability checks use subtraction after validating
`rpos <= wpos <= size`, avoiding unchecked end-position additions.
The source is checked again after refill, and both buffers are checked
before copying, including after destination flush.

The function now measures actual free space after a flush instead of
comparing the requested length with bytes flushed. This also permits a
valid transfer when flushing one buffered byte frees enough room for a
larger request. Negative callback errors propagate, ordinary source
shortage returns zero, and insufficient destination space returns `-1`.
The final source-read and destination-write increments follow a complete
copy; callbacks may still compact, refill, or flush buffers before an
unsuccessful transfer.

Added four cases in `test/buf_test.c`: `buf_xfer_limits`,
`buf_xfer_positions`, `buf_xfer_refill_flush`, and
`buf_xfer_callback_error`. Coverage includes `SW_MAX + 1`, `UW_MAX - 1`,
`UW_MAX`, zero, a `SW_MAX` source shortage, exact end boundaries, invalid
positions in either buffer, source compaction and chunked refill, valid
flushes that report fewer bytes than the request, callback errors,
non-progress/overstated flush results, and invalid positions after
callbacks. Rejected initial lengths and positions leave both buffers
unchanged and do not invoke either callback.

**Verification:** reran the original native crash probe before the fix:
exit **139**. With the fix, the same probe's one-byte control returns
**1**, its `UW_MAX` request returns **-1**, and the process exits **0**.
Release and debug buffer suites both pass **43/43** tests. The full
release C suite passes **508/508** tests. Release interpreter fixtures
`marshall`, `str`, `call`, `quote`, `list`, and `tuple` pass **6/6**.
`git diff --check` and the audit file whitespace check pass.

Logs: `/tmp/kc3-f66-before.log`, `/tmp/kc3-f66-probe.log`,
`/tmp/kc3-f66-release.log`, `/tmp/kc3-f66-debug.log`,
`/tmp/kc3-f66-full.log`, and `/tmp/kc3-f66-ikc3.log`.
ASan is unavailable in this native configuration; other platforms were
not tested.

### F67 — Medium: failed Complex division is published as a null Complex

**Baseline locations:** `libkc3/tag_div.c:47–52` and the other Complex result
branches, `libkc3/complex.c:82,305`, `libkc3/tag_init.c:268`, and
`libkc3/buf_inspect.c:1768,3497`. Tag cleanup also dispatches to
`pcomplex_clean` for this result type.

`complex_div` correctly propagates a scalar division failure, and
`complex_new_div` returns NULL. The dispatcher passes that NULL directly
to `tag_init_pcomplex`, which sets the result type to Complex and returns
success without checking the pointer. Inspection or cleanup then assumes
a real Complex object.

**Runtime reproduction:**

```elixir
(1 +i 2) / (0 +i 0)
```

The release interpreter emitted `tag_div: division by zero` and terminated
with status 139. An independent C probe stopped before inspection/cleanup
and confirmed that `tag_div` returned its success pointer after replacing
a U8 canary destination with `TAG_PCOMPLEX` and a NULL pointee.

**Preconditions / impact:** ordinary Complex division by zero. The
mathematical error is detected, but its failure is converted into an
invalid successful result and a native null-pointer crash. No allocator
failure or malformed Complex object was supplied. No arbitrary memory
corruption or remote exploitability is claimed.

**Repair — 2026-11-02:** fixed in `libkc3/tag_div.c`. All 30 existing
Complex result branches now use a checked helper, `tag_div_complex`.
It calls `complex_new_div`, returns NULL on failure, and only then
publishes a non-null Complex through `tag_init_pcomplex`. The destination
is unchanged on failure, including its ownership/reference metadata.
The Integer/Ratio branches' existing cleanup now runs correctly after
quotient construction fails, releasing their converted operands.

The shared `tag_init_pcomplex` contract is unchanged. Supported operand
combinations and floating arithmetic semantics are also unchanged.

Added `tag_div_complex` coverage in `test/tag_test.c`: Complex and
mixed Integer/Ratio/fixed-width zero divisors; an aliased failure
destination; successful real and imaginary components for every existing
dispatch branch; injected NULL returns at converter and quotient
constructor boundaries; and deterministic mixed Ratio/non-finite float
failures. Temporary converted Integer/Ratio objects are counted and must
be released on both success and failure. An owned Ratio canary destination
must remain byte-for-byte unchanged after each failure.

Extended `test/ikc3/complex.kc3` with six failed divisions that must produce
Void and eight successful mixed-division controls, with expected output.

**Verification:** the original interpreter probe still exited **139**
before the fix. The new C regression also failed against the original
code because `tag_div` returned success for Complex division by zero.
After the fix:

- Release and debug tag suites: **10/10** tests pass in each build.
- Full release C suite: **509/509** tests pass.
- Release interpreter fixtures `complex`, `ratio`, and `marshall`:
  **3/3** pass; the updated `complex` fixture also passes in debug.
- The original crash expression now emits the division error and `void`,
  then exits **0**.
- `git diff --check` and the audit file whitespace check pass.

Logs: `/tmp/kc3-f67-interpreter-before.log`, `/tmp/kc3-f67-before.log`,
`/tmp/kc3-f67-release.log`, `/tmp/kc3-f67-debug.log`,
`/tmp/kc3-f67-full.log`, `/tmp/kc3-f67-ikc3.log`,
`/tmp/kc3-f67-ikc3-debug.log`, and `/tmp/kc3-f67-interpreter-after.log`.
Injection checks helper boundaries, not raw allocator failures or every
internal Complex constructor failure. ASan and F128 are unavailable in this native
configuration; other platforms were not tested. F70 remains open.

### F68 — Medium: Ratio-to-float conversion overflows components before dividing

**Baseline locations:** `libkc3/ratio.c:557,568,581,596` (`ratio_to_f32/f64/f80/f128`),
`libkc3/integer.c:899–931`, and the Ratio branches of floating casts.

Each Ratio conversion first converts the arbitrary-precision numerator
and denominator separately to the destination float, then divides them.
The components can overflow even when their quotient is finite and easily
representable. Converting two large components to infinities produces
NaN instead of their finite quotient.

**Runtime verification:** a C probe built valid, reduced Ratios with
checked Integer operations and called the public conversion helpers:

| Input Ratio | Conversion | Observed | Correct rounded value |
| --- | --- | --- | --- |
| `(2^200 + 1) / 2^200` | F32 | NaN | `1.0` |
| `(2^2000 + 1) / 2^2000` | F64 | NaN | `1.0` |
| `(2^2000 + 1) / 2^2000` | F80 | NaN | `1.0` |

These numerators are odd and denominators are powers of two, so the
examples are already reduced. The small excess above one is below the
rounding resolution of each tested destination type. The F80 example
also passes through the F64 narrowing described in F69. The same
component-overflow design exists in the conditional F128 helper, but that
configuration was not executed.

**Preconditions / impact:** large valid Ratio components. A finite
numeric value becomes NaN, affecting casts and calculations that use
these conversion helpers. This is separate from exact float-to-Ratio
construction: the defect is in the reverse conversion direction.

**Repair — 2026-11-02:** replaced the component conversions in
`libkc3/ratio.c` with exact quotient/remainder rounding. The helper uses
bit counts and an exact comparison to find the quotient's exponent,
scales Integer operands to the destination's normal/subnormal quantum,
and rounds once to nearest with ties to even. It builds the resulting
significand directly in F32/F64/F80/F128 arithmetic, avoiding F64
narrowing of extended results. Distant overflow/underflow is classified
before large shifts; negative underflow preserves signed zero.
The original Ratio components are unchanged.

Added the missing Ratio branch in `libkc3/f64.c:f64_init_cast` so ordinary
F64 casts reach the corrected conversion, consistently with the other
floating cast helpers. Public signatures and structure layouts are
unchanged. Multi-precision errors are checked; after cleanup, an error
aborts as the existing Ratio-to-integer observers do.

Added `ratio_to_float` in `test/ratio_test.c`. Conditional native-type
cases cover large coprime components above the target's exponent range,
both signs, normal midpoints and distant sticky bits, ties to both even
neighbors, signed zero, subnormal midpoints, the normal/subnormal
boundary, the finite/infinity threshold, true overflow/underflow, exact
float-to-Ratio-to-float round trips, and small non-dyadic quotients.
Each conversion is checked for unchanged input components.

Added twelve interpreter cast checks in `test/ikc3/ratio.kc3`: large
finite F32/F64 quotients, signs and reciprocals, midpoint rounding,
minimum subnormal round trips, and negative underflow. Inspection-string
comparisons distinguish finite output from NaN. Infinity inspection is
excluded from the passing fixture because of the separate F71 defect;
native C tests check overflow results directly.

**Verification:** the new C regression failed on the original F32
large-component conversion. Release and debug Ratio suites now both pass
**7/7** tests. The full release C suite passes **510/510** tests. Release
interpreter fixtures `ratio`, `complex`, and `marshall` pass **3/3**;
the updated `ratio` fixture also passes in debug. `git diff --check` and
the audit file whitespace check pass.

An independent temporary C/Python probe compared **500**
deterministic large signed rational-to-F64 conversions with Python's
`float(Fraction(n, d))`; every result matched bit-for-bit, including
signed underflow and overflow (seed 68).

Logs: `/tmp/kc3-f68-before.log`, `/tmp/kc3-f68-release.log`,
`/tmp/kc3-f68-debug.log`, `/tmp/kc3-f68-full.log`,
`/tmp/kc3-f68-ikc3.log`, `/tmp/kc3-f68-ikc3-debug.log`, and
`/tmp/kc3-f68-oracle.log`. The standalone reference probe is `/tmp/kc3-f68-oracle.c` plus
`/tmp/kc3-f68-oracle.py`. F32/F64/F80 were executed natively; F128 and
ASan are unavailable in this configuration. Raw allocation-failure
injection and other platforms were not tested. Separate Integer
conversion APIs are tracked under F69 below.

### F69 — Medium: extended Integer/float conversions narrow through F64

**Baseline locations:** `libkc3/integer.c:309,320,756,774,913,924`, and Integer
branches in `f80.c`/`f128.c` and the corresponding cast paths.

`integer_set_f80` and `integer_set_f128` cast their input to `double`
before `mp_set_double`. `integer_to_f80` and `integer_to_f128` likewise
obtain a double from `mp_get_double` and only then widen it. Extended
precision is lost before the destination conversion can use it.

**Runtime verification with native F80 (`LDBL_MANT_DIG=64`):**

- Integer `9007199254740993` converted to F80 becomes
  `9007199254740992`, even though F80 can represent the original exactly.
- F80 `9223372036854775809.0L` passed to `integer_init_f80` becomes
  Integer `9223372036854775808`. The probe compared the resulting Integer
  against that incorrect value; the comparison was equal.

Both examples succeed without an allocation failure. Source inspection
also shows that finite F80 values beyond F64's exponent range can narrow
to infinity and be rejected by the Integer backend; that range failure
was not deliberately executed in this pass. F128 counterparts were
source-reviewed only because F128 is disabled here.

**Impact:** valid casts silently lose integer precision, and the available
extended range cannot be honored. This is independent of the now-exact
float-to-Ratio constructors, which do not use these Integer float helpers.

**Repair — 2026-11-02:** fixed in `libkc3/integer.c`.

`integer_init_f80`/`integer_init_f128` extract integer bits using the
source type's own arithmetic, truncating finite values toward zero.
Trailing zero bits use a single multiprecision shift. Values beyond F64's
range retain their full extended precision. The constructors build a
local Integer and publish it only on success. Non-finite inputs return
NULL; checked multiprecision failures clean the temporary and return
NULL. Both leave the destination unchanged. The corresponding setters
construct a temporary and exchange storage only on success, releasing
the old initialized Integer.

`integer_to_f80`/`integer_to_f128` pass a borrowed Integer/unit fraction
to F68's exact Ratio rounding engine. Results round once to nearest,
ties to even, across the destination's full range; true overflow becomes
signed infinity. The source remains unchanged. The borrowed fraction
owns neither component and is not cleaned. Multiprecision conversion
errors follow the Ratio observer's existing cleanup-and-abort policy.
Public signatures and layouts are unchanged.

Added `tag_integer_extended` in `test/tag_test.c`, conditional on each
extended type. It covers the original precision reproductions, both
signs, fractional truncation, signed zero, subnormal and normal inputs,
finite values beyond F64's range, the maximum finite value, repeated
setters, Integer and floating cast APIs, and non-finite rejection with
unchanged owned destinations. Integer-to-float cases exercise ties to
both even neighbors, distant sticky bits, and both sides of the
finite/infinity threshold. Float-to-Integer expectations use the separate
exact float-to-Ratio constructor followed by integer division.

**Verification:** the new regression failed against the original
Integer-to-F80 conversion of `9007199254740993`. Release and debug tag
suites now both pass **11/11** tests. The full release C suite passes
**511/511** tests. Release interpreter fixtures `integer`, `integer_eq`,
`ratio`, and `complex` pass **4/4**. A temporary native F80 script, after
`require F80`, passes ten checks for precise round trips, truncation,
and rejection of NaN and both infinities. No infinity is inspected.
`git diff --check` and the audit whitespace check pass.

An independent temporary C probe (seed 69) compared **500** deterministic
signed Integers, up to 17,200 bits, against libc `strtold` conversion of
their exact decimal forms. Another **500** finite F80 inputs across the
extended exponent range were compared with libc `truncl` and exact
decimal-to-Integer conversion, for both constructors and repeated
setters. All comparisons matched, including signed overflow results.

Logs: `/tmp/kc3-f69-before.log`, `/tmp/kc3-f69-release.log`,
`/tmp/kc3-f69-debug.log`, `/tmp/kc3-f69-full.log`,
`/tmp/kc3-f69-ikc3.log`, `/tmp/kc3-f69-extended.log` (plus `.err`), and
`/tmp/kc3-f69-oracle.log`. Temporary sources are
`/tmp/kc3-f69-extended.kc3` and `/tmp/kc3-f69-oracle.c`.
F80 was executed natively; F128 is disabled and ASan is unavailable in
this configuration. Raw allocation-failure injection and other platforms
were not tested.

### F70 — Medium: Complex add/subtract/multiply ignore failed scalar operations

**Locations:** `libkc3/complex.c:57` (`complex_add`), `:227`
(`complex_mul`), and `:401` (`complex_sub`), plus their `complex_new_*`
and tag-dispatch callers.

These helpers call scalar `tag_add`, `tag_sub`, and/or `tag_mul` without
checking their return values, then return the Complex destination as a
successful result. A failed component can retain old destination data or
leave a newly allocated component as Void. Multiplication can continue
operating on failed intermediate tags as well.

**Runtime verification:** for each of the three helpers, a C probe used
`a = {real: F64 NaN, imaginary: 0}` and
`b = {real: Ratio 1/2, imaginary: 0}`, and initialized the destination's
real component to U8 `77`. The mixed scalar operation emitted
`ratio_init_f64: non-finite value` and failed. Nevertheless, all three
Complex helpers returned their success pointer and left the real
component equal to the stale `77`. Multiplication also emitted an
invalid scalar-type diagnostic while processing a failed intermediate.

**Preconditions / impact:** a component operation returns failure. The
reproduction uses normal numeric tag types and deterministic non-finite
rejection, not allocation failure. The Complex API conceals that failure
as a successful, incorrect result. This is distinct from F67, where the
inner Complex division already returns failure correctly and its outer
tag wrapper discards it.

**Repair — 2026-11-03:** fixed in `99f499da7`, based on
`a4a056d45` (`fix marshall facts after dump log file relative path`).
`complex_add`, `complex_sub`, and `complex_mul` now check every scalar
operation and stop at the first failure. They construct both components
in a zero-initialized local result and publish it only after complete
success. All intermediate tags and any partial local result are cleaned
on failure. An existing destination, including one aliasing either input,
remains unchanged on failure; `complex_new_add/sub/mul` consequently
return NULL and release their outer allocation.

`libkc3/tag_add.c`, `tag_sub.c`, and `tag_mul.c` each route all 31 Complex
dispatch branches through a checked constructor helper. A failed
construction returns NULL before modifying the destination; no null
Complex is published. Integer/Ratio conversion temporaries are cleaned
on failure as well as success. Subtraction's previously uninitialized
Complex temporary is now zero-initialized: the expanded checks exposed
a cleanup wait when stale tag metadata was interpreted as reference-count
and mutex state. No code comments were added.

**Regression coverage:** `test/tag_test.c` adds two cases. The direct
Complex case exercises failure in both add/subtract components, all four
multiplication products, and the final real subtraction and imaginary
addition. These are real numeric failures: mixed F64 NaN/Ratio conversion
rejection, or finite floating products that overflow before encountering
a Ratio in the final scalar operation. Owned Ratio destinations and both
inputs remain unchanged; direct calls, allocating constructors, aliased
outputs on failure, and Complex/Complex tag dispatch are checked.

The tag-dispatch case covers successful arithmetic for every supported
Complex/scalar pair in both operand orders, then injects failures at
conversion and Complex-construction boundaries using the existing test
wrappers. It verifies an unchanged owned destination, preserved tag
metadata, and no live conversion temporary. Natural NaN/Ratio failures
also propagate through both operand orders for native F32, F64, and F80.
This is helper-boundary injection, not raw allocator-failure injection.

`test/ikc3/complex.kc3` adds 13 failure expressions, including late
multiplication failure after finite inputs overflow, and three exact Ratio
arithmetic controls. Failed ordinary arithmetic C functions retain the
existing interpreter Void result. The fixture compares their diagnostics
and Boolean results without inspecting infinity (F71 remains separate).

**Verification:** the two new C cases failed against the original code
(11/13 focused tests passed). After repair, focused release and debug
tag suites pass **13/13** each, the full release C suite passes
**519/519**, and the full release interpreter suite passes **71/71**.
Release module links were restored after debug checks, and
`git diff --check` passes. Native OpenBSD amd64 executed F80; F128 and
ASan are unavailable. No other-platform validation is claimed.

Evidence: `/tmp/kc3-f70-before.log`, `-focused-release.log`,
`-full-release.log`, `-focused-debug.log`, `-interpreter-focused.log`,
`-interpreter-release.log`, and `-interpreter-build.log`.

### F71 — Medium: floating infinity inspection does not terminate

**Locations:** `libkc3/buf_inspect.c:1841,1866,1911,1936`
(F32 inspection and size calculation), `:1977,2002,2047,2072`
(the corresponding F64 paths). The F80/F128 implementations contain
similar scaling loops but were not exercised for this finding.

These formatters normalize positive magnitudes by repeatedly dividing by
ten while `x >= 10`. Infinity remains infinity after division, so the
loop never reaches a finite significand. It has no non-finite-input guard.

**Runtime verification:** separate release interpreter scripts containing
`inspect((F32) 1.0 / (F32) 0.0)` and the F64 equivalent both exceeded a
two-second timeout and exited **124** without output. Neither expression
uses a Ratio conversion. Core dumps were disabled. The same problem was
first encountered in an F68 interpreter overflow-output check.

**Impact:** valid floating infinity, including ordinary floating division
by zero, can stall inspection. F68's finite quotient conversion is fixed;
this is a separate existing output-path defect.

**Repair — 2026-11-03:** fixed in `08bd1f7f9`, based on
`99f499da7` (`fix complex arithmetic`). All eight F32/F64/F80/F128 writer
and size functions in `libkc3/buf_inspect.c` classify NaN and infinity
before normalization or conversion to an integer digit. Positive and
negative infinity print as typed `inf` and `-inf`, for example
`(F64) -inf`. Both NaN signs print as typed `nan`; NaN signs and payloads
are not represented. Size calculation uses the same spelling and updates
pretty-print columns consistently with writing. Finite formatting is
unchanged. These are display forms; the parser grammar is unchanged.
No code comments were added.

`test/buf_inspect_test.c` adds `buf_inspect_float_special`, conditional on
each enabled float type. It checks both infinity signs and both NaN
signs, exact-size buffers, every smaller buffer capacity, matching byte
counts and pretty-print state, allocated tag inspection, and nested
Tuple inspection. The existing finite tests still pass. An F80 control
also inspects `LDBL_MAX` as a finite decimal, checking that classification
does not narrow a finite extended value through F64.

`test/ikc3/inspect.kc3` adds 18 output checks for F32/F64 arithmetic,
direct interpreter printing, `inspect`, and nested Tuples. A separate
native F80 script checks the same nine outputs, after `require F80`.
The negative NaN expression uses multiplication by negative one, because
unary floating negation is the separate F72 finding below.

**Verification:** the new C case timed out against the original library
(exit **124**, six-second bound). After repair, the full release C suite
passes **520/520**, the focused release and debug inspection suites pass
**25/25** each, and the full release interpreter suite passes **71/71**.
The native F80 script exits zero with exactly the expected output.
Release module links were restored after debug checks, and
`git diff --check` passes. Native OpenBSD amd64 exercised F32/F64/F80;
F128 is disabled and its guarded code was not executed. ASan and
other-platform validation were not performed.

Evidence: `/tmp/kc3-f71-before.log`, `-focused-release.log`,
`-full-release.log`, `-focused-debug.log`, `-interpreter-focused.log`,
`-interpreter-release.log`, and `-interpreter-build.log`. The F80
artifacts are `/tmp/kc3-f71-f80.kc3`, `.expected`, and `.log`.

Artifacts: `/tmp/kc3-f68-inspect-infinity-F32.kc3` and `-F64.kc3`, their
matching `.log` files, and
`/tmp/kc3-f68-ikc3-infinity-timeout.log` (the original fixture timeout).


### F72 — Medium: unary negation rejects floating values

**Locations:** `libkc3/tag_neg.c:18` (`tag_neg`) and
`lib/kc3/0.1/init.kc3:33` (the unary `-` operator binding).

`tag_neg` dispatches Integer, Ratio, and fixed-width integer tags, but
has no F32/F64/F80/F128 cases. Applying unary minus to a floating
expression therefore reports `invalid tag type` and returns NULL. The
ordinary arithmetic C-function bridge turns that failure into Void.
Negative numeric literals are parsed directly and can work, concealing
the missing operator support until minus is applied to an expression.

**Runtime verification:** on native OpenBSD amd64,
`-((F32) 2.0)`, `-((F64) 2.0)`, and `-((F80) 2.0)` each print
`tag_neg: invalid tag type: F32` (or F64/F80) followed by `void`.
The Integer and Ratio controls `-(2)` and `-(1/2)` print `-2` and `-1/2`.
The finite values require no allocation-failure condition or non-finite
input. `/tmp/kc3-f72-neg.kc3` and `.log` record the reproduction; the
interpreter exits zero despite the failed arithmetic expressions.

**Impact:** ordinary unary floating arithmetic fails instead of returning
the sign-reversed value. F71's formatter repair is complete; this is a
separate operator-dispatch defect. F128 was not executed locally.

**Repair — 2026-11-03:** fixed in the working tree, based on
`08bd1f7f9` (`fix buf_inspect for special float values`).
`libkc3/tag_neg.c` now dispatches F32/F64 and the enabled F80/F128 types.
Each branch applies unary minus in the operand's native type and returns
its corresponding tag constructor's result. This preserves precision,
range, tag type, and IEEE sign behavior, including signed zero,
infinity, and NaN. The constructors preserve destination reference-count
and mutex metadata. In-place negation is supported. No code comments
were added.

**Regression coverage:** `test/tag_test.c` adds `tag_neg_float`, checking
16 values per enabled type: both signs of zero, ordinary finite values,
the minimum normal value, the smallest subnormal, the largest finite
value, a value with the type's full significand precision, infinity, and
NaN. It checks separate and aliased destinations, double negation,
unchanged input for separate output, and byte-for-byte preservation of
tag metadata. The native F80 range and precision cases extend beyond
F64. F128 coverage is conditional on its build flag.

`test/ikc3/float_neg.kc3` adds a dedicated fixture with 24 F32/F64
expressions. It checks finite values and computed expressions, type
preservation, double negation, signed-zero reciprocals, infinities, NaNs,
and inspection of the result. The existing Integer negation fixture also
passes. A temporary native F80 script checks the equivalent 12 outputs
after `require F80`.

**Verification:** the new C case fails against the original library
(13/14 focused cases passed). After repair, focused release and debug
tag suites pass **14/14** each, the full release C suite passes
**521/521**, and the full release interpreter suite passes **72/72**.
The F80 script exits zero with exactly the expected output. Release
module links were restored after debug checks, and `git diff --check`
passes. Native OpenBSD amd64 exercised F32/F64/F80; F128 is disabled and
was not executed. No ASan or other-platform validation is claimed.

Evidence: `/tmp/kc3-f72-before.log`, `-focused-release.log`,
`-full-release.log`, `-focused-debug.log`, `-interpreter-focused.log`,
`-interpreter-release.log`, and `-interpreter-build.log`.
The F80 artifacts are `/tmp/kc3-f72-f80.kc3`, `.expected`, and `.log`.
Changes remain uncommitted.

## Verification artifacts and continuation

Temporary artifacts are outside the repository and may not survive a
later environment reset:

- `/tmp/kc3-fresh-audit.c` and `.log`: numeric results, Complex error
  handling, and all ten safe cache-lifetime observations; final run exited 0.
- `/tmp/kc3-fresh-complex-zero.kc3` and `.log`: interpreter crash, exit 139.
- `/tmp/kc3-fresh-buf-xfer.c` and `.log`: one-byte control and oversized
  transfer crash, exit 139.
- `/tmp/kc3-fresh-audit-build.log`: successful release interpreter build.

Crash commands used `ulimit -c 0` and a timeout. The cache probe deliberately
did not follow a dangling pointer through the normal decoder. Source
locations and reproduction conditions above remain the durable record.

Selected map and array constructor allocation/count checks were inspected
and their obvious multiplication-overflow candidates were not promoted:
`map_init` and `array_init` already check the relevant products. No new
finding is claimed for the briefly inspected string concatenation paths.
General Frame sharing, further scalar/Complex error paths, additional
buffer APIs, parser/inspector rollback, concurrency, allocator failures,
and other platforms still need review. Passing probes do not establish
full-library safety.

All recorded findings through **F72** are fixed. Resume the broader
audit, including the remaining scalar/Complex error paths and buffer
APIs noted above; record new confirmed findings from **F73** onward.


## Build-error investigation — 2026-11-03

The requested `make` in `lib` was repeated with `. ./env` and an absolute
repository `bin` directory in PATH. The remaining diagnostics have two
primary causes and a separate cleanup defect. This section records the
original diagnosis; the repairs and completed verification follow below.
The pending F70–F71 work retains its numbering. Controlled reproductions
and experimental changes are under `/tmp/kc3-build-errors/`.

### Kpkg dependency cycle and parser diagnostics

`lib/kc3/0.1/kpkg.kc3:17` requires `Kpkg.Package`, whereas
`lib/kc3/0.1/kpkg/package.kc3:13` requires `Kpkg` before its `defstruct`
at line 25. `kc3c/kc3c.c:69` enters the requested file directly through
`env_load(..., false)`, without the loading marker established by
`env_module_load`. The trace for compiling Kpkg therefore enters Package,
then recursively enters Kpkg while Package's struct is still undefined.
The `%Kpkg.Package{...}` literal at `kpkg.kc3:209` cannot be parsed.

This accounts for `struct_type not found: Kpkg.Package`, the surrounding
`buf_parse_struct` / `buf_parse_do_block_inner` / `buf_parse_fn_clause`
messages, and the subsequent Kpkg module-load and `Kpkg.defpackage`
resolution failures. The module file exists; its load failed.

The later `cannot cast Ident to Block` is a parser fallback consequence:
a failed `do ... end` block is retried as the identifier `do`
(`libkc3/buf_parse.c:5381`). The resulting `defmodule Kpkg do` call carries
an Ident where `KC3.defmodule` expects Block. `tag_to_ffi_pointer` and
`cfn_apply` then report the type mismatch.

A three-line module containing a function with an undefined struct
reproduced the parser and Ident/Block cascade. A minimal two-module cycle
also reproduced it; removing the child's `require` of its parent in the
temporary copy eliminated those diagnostics. The original large Kpkg
source reproduced the cycle; a complete compilation control after
removing its back edge was stopped by a 45-second timeout, so no passing
full Kpkg build was claimed at that stage. The small cycle could print
errors and still exit zero; successful exit alone is insufficient verification here.

### GL.Object layout and tuple read failures

`libkc3/tag.c:73` (`tag_alignment`) calculates a small/empty array tag's
alignment from its element type. However, `sym_type_size` represents an
array field using the full `s_array` descriptor. The two empty array
fields at `lib/kc3/0.1/gl/object.kc3:20` consequently give GL.Object an
alignment of 4 through `struct_type_init` (`libkc3/struct_type.c:227`).

The recorded descriptor has size 192 and offsets 0, 88, 176, 180, 184,
188. The reader derives alignment 8 for each array field, then rejects
`align_max=4` at `libkc3/marshall_read.c:3316`. This produces
`marshall_read_struct_type: invalid layout` at line 3475 while loading
GL.Object embedded in the GL.Box and GL.MarchingCubes caches. The
surrounding tuple consequently reports `read_tag error` at line 3753.
These are successive reports of the same failed descriptor read.

A temporary override made array tags use the Array descriptor alignment.
Recompiling both GL.Box and GL.MarchingCubes in the isolated module tree
with that override produced caches which the original reader accepted;
compiling `kubz.kc3` then exited zero without layout errors. Updating only
GL.Box exposed the same defect in the remaining MarchingCubes cache.
Affected embedded descriptors require rebuilding along with the fix.

### Cleanup crash after descriptor rejection

`marshall_read_tag_1` assigns `TAG_PSTRUCT` before decoding the payload
(`libkc3/marshall_read.c:3594`). A failed payload read leaves that tag with
a NULL Struct pointer. `marshall_read_tuple` calls `tuple_clean` on the
failure path, which reaches `tag_clean` and then `pstruct_clean`.
`libkc3/pstruct.c:26` dereferences `(*s)->ref_count` without a NULL check.

The isolated, unmodified conversion path exited 139. LLDB confirmed
`marshall_read_tuple -> tuple_clean -> tag_clean -> pstruct_clean`, with
the Struct pointer equal to NULL. Failed tag reads need to leave
cleanup-safe state; correcting the layout alone does not repair this
error path.

### Earlier environment errors

The initial `kc3c` lookup failure came from PATH after sourcing `env`;
prepending the repository's absolute `bin` directory resolved it. The
initial GL shared-library/symbol failures were also environmental:
`ldd gl/libkc3_gl.so` failed without `env` and resolved its dependencies
successfully with `env` sourced. The earlier
`pcallable_clean: invalid argument` is the guarded equivalent of partial
failed-decode cleanup: `pcallable_clean` receives a NULL callable after
C symbol resolution fails (`pcallable.c:26`, `marshall_read.c:537`).

Evidence logs: `kpkg-isolated.log`, `missing-struct.log`,
`buildcycle-before.log`, `buildcycle-after.log`, `kubz-layout-trace.log`,
`cleanup-crash.log`, `gl-dependencies-rebuilt.log`, and `kubz-rebuilt.log`
under `/tmp/kc3-build-errors/`. The trace executable interposes a temporary
copy of `marshall_read.c` containing diagnostic output; the ordinary
probe also reproduced the original layout diagnostic and cleanup crash.


## Build repairs and verification — 2026-11-03

All reported `lib` build errors are repaired in the working tree. The
requested `. ./env && export PATH="${PWD}/bin:$PATH" && cd lib && make`
completes successfully for all 148 sources without diagnostics. The
library caches were regenerated with the corrected struct descriptors.
Release library links were restored after the debug checks. No commit
was created. F70 and F71 remain open.

### Repairs

- `lib/kc3/0.1/kpkg/package.kc3` loads `Kpkg` inside `load_all`, after its
  own definitions are available. The 19 repository definitions require
  `Kpkg`, which supplies the `Kpkg.defpackage` macro they use. This removes
  the eager parent/child dependency cycle while preserving dependencies.
- `libkc3/tag.c` uses the Array descriptor alignment for array fields,
  matching their `s_array` storage rather than element alignment or payload
  size. Empty, small, and larger arrays now produce consistent C layouts.
- `libkc3/marshall_read.c` decodes tags and tuples into temporary values
  and publishes them only on success. Failed reads preserve the caller's
  destination and allow containing values to clean up safely. Tuple
  mutexes are destroyed before moving storage and initialized at the
  destination; initialized mutexes are not copied as live objects.
- `libkc3/buf_parse.c` treats failure after recognizing a long `do` block
  as an error, preventing fallback to the identifier `do`. It also reuses
  an already parsed primary expression when no binary operator follows.
  Previously, nested functions repeatedly reparsed their entire bodies:
  a native probe at depths 1–8 counted 6, 14, 30, ... 1022 primary parses;
  the repaired parser counts 2, 3, 4, ... 9. The public operator-only
  parser still returns zero and restores input for a standalone primary.
- `libkc3/cfn.c` propagates a native NULL Result return from a special
  operator as failure, so failed `require` and `defmodule` evaluations
  cannot become successful Void values. Ordinary C functions retain their
  existing NULL-to-Void behavior.
- `kc3c/kc3c.c` compiles each input file in a fresh environment, preserving
  command arguments and compiler options. Reusing one environment for the
  whole batch caused a later `require Kpkg` to stall after compiling Kpkg
  directly. Independent file environments eliminate that order-dependent
  state; the four-file reproduction and the complete library batch pass.

### Regression coverage

`test/marshall_read_test.c` checks array field offsets, total size, and
alignment against an actual C struct for empty U8/F32 arrays and one/four
F32 elements, including descriptor serialization and map updates. Failed
Struct decoding is exercised directly and inside tuples, maps, and quotes,
plus the direct tuple reader, with both copy modes. An existing owned
destination remains unchanged and can be cleaned normally. The two new
cases failed against the original implementation and pass after repair.

`test/buf_parse_test.c` checks malformed long blocks and undefined nested
structs in release mode, plus a 20-level nested function tree and the
public operator-parser fallback in both builds. Debug omits malformed
source cases that encounter existing parser assertions.

`test/env_test.c` checks special-operator failure without modifying an
owned destination and verifies ordinary C-function NULL-to-Void behavior.
A native malformed-module compiler probe also exits one on failed module
evaluation, rather than reporting successful compilation.

### Completed checks

| Check | Result |
| --- | --- |
| Full release C suite | 516/516 passed |
| Focused release parser/reader/environment suite | 125/125 passed |
| Focused debug parser/reader/environment suite | 119/119 passed |
| Full release interpreter suite | 71/71 passed |
| Requested release `make` in `lib` | 148/148 sources, exit zero, no diagnostics |
| Kpkg, kmx_sort, HTTPd, Kubz in one compiler batch | Release and debug both passed |
| Malformed-module compiler probe | Exit one as expected |
| `git diff --check` | Passed |

Checks ran on native OpenBSD amd64. The release C total includes the F69
working-tree repair. No ASan, allocator-failure injection, or other
platform validation is claimed.

Evidence is under `/tmp/kc3-build-errors/`: `regressions-before.log`,
`parse-count-before.log`, `parse-count-after.log`, `full-release.log`,
`focused-release-final.log`, `focused-debug.log`, `ikc3-release.log`,
`lib-build-final.log`, `lib-build-final-release.log`, `batch.log`,
`batch-debug.log`, and `operator-failure.log`. The earlier timeout logs
record diagnostic experiments, not the final build result.


## HTTPd dump relocation repair — 2026-11-03

**Baseline:** `691e53ed1` (`fix kpkg and kc3c`). The reported deployment
restored `test/httpd/kc3.dump` under `/home/kc3-lang.org/kc3`, but its
nested Config database retained the absolute log path from this checkout:
`/home/dx/c/thodg/kc3-lang/kc3/test/httpd/db/app.facts.bin.facts`.
`facts_open_memoized` canonicalizes the live database path, and
`marshall_facts` previously serialized that absolute path unchanged.
Restoring the dump on another checkout therefore attempted to reopen or
create the old checkout's database files and failed.

The subsequent SIGSEGV was a separate error-handling defect in
`httpd/httpd.c`: `main` ignored a failed `kc3_init` and continued with a
NULL environment. A missing-dump reproduction exited 139 before the fix.
The reported nested Facts failure was also reproduced by removing the
database directory from a temporary copy of the application.

**Repair:** `libkc3/marshall.c` serializes both the database log path and
its after-dump log path relative to the current working directory, using
`file_relative`, without modifying the live database's paths. An empty
after-dump path remains empty so the reader can derive the default sidecar.
The existing wire format is unchanged. `httpd/httpd.c` now returns one
immediately if runtime initialization fails. No code comments were added.

The tracked `test/httpd/kc3.dump` and `httpd/fx/kc3.dump` were regenerated
with the repaired serializer. Their previous dumps were backed up under
`/tmp/kc3-build-errors/`. Existing deployed dumps containing absolute paths
need regeneration on their deployment host; installing the source fix
alone does not rewrite old snapshots. The newly regenerated tracked
application dumps contain no paths under the original checkout root.

**Regression:** `test/marshall_read_test.c` adds
`marshall_read_facts_log_relocated`. It opens an absolute database path,
serializes the Facts, appends a fact after the snapshot, closes the source,
and moves its entire directory before restoration. It verifies relative
paths, replay of the appended fact, and pathname memoization. Both an
explicit custom sidecar path and an empty path with a derived default
are covered. Live paths remain unchanged by serialization. The new case
failed against the original serializer and passes with the repair.

**Verification:** native OpenBSD amd64, core dumps disabled for failure
probes. The full release C suite passes **517/517**; focused release
marshaller/reader/environment/Facts tests pass **158/158**, and focused
debug tests pass **150/150**. Release HTTPd and kc3s builds succeed; release
module links were restored after debug tests.

A temporary application copied under `/tmp` restores the regenerated dump
using `db/app.facts.bin.facts`, serves HTTP 200 with HTML and the expected
Server header, returns 404 for a missing route, and shuts down with exit
zero on SIGINT. Moving its database directory away reproduces the reported
restore-diagnostic chain and now exits **one**, without SIGSEGV. A missing
dump also exits one. No remote deployment was performed.

Evidence under `/tmp/kc3-build-errors/`: `httpd-init-before.log`,
`httpd-init-after.log`, `facts-relocation-before.log`,
`facts-relocation-release.log`, `httpd-fix-full-release.log`,
`httpd-fix-focused-debug.log`, `test-httpd-redump.log`,
`httpd-fx-redump.log`, `httpd-relocated-check.log`,
`httpd-relocated-startup.log`, and `httpd-missing-db-after.log`.
Changes remain uncommitted; F70–F71 remain open.
