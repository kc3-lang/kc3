# KC3 audit — F60 onward

Updated 2026-11-01.

## Progress

F60–F64 are fixed. No open findings remain in this log. The full-library
audit remains incomplete; continue with the review areas listed below.

- F60 is committed as `b9ade72ca`.
- F61 is committed as `fb6690bf9`.
- F62 is committed as `2a9cb4681`.
- F63 is committed as `b0c4565e8`, the F64 baseline.
- F64 is committed as `6a1e1ce64`, the current HEAD.
- The latest full C suite passed all 502 reported tests.

The fresh audit that identified F61–F64 used baseline `8349cd54c`.
Verification ran on native 64-bit OpenBSD. The full-library audit remains
incomplete; the findings below describe the specific paths reviewed.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F60 | Medium | Mixed-type division reduces the wrong result object | Fixed (`b9ade72ca`) |
| F61 | High | Decoded Call/Complex cache entries outlive their objects | Fixed (`fb6690bf9`) |
| F62 | Medium | Sw/Integer modulo narrows a valid large remainder | Fixed (`2a9cb4681`) |
| F63 | Medium | Ratio division by zero returns an invalid Ratio | Fixed (`b0c4565e8`) |
| F64 | Medium | Mixed float/Ratio arithmetic discards fractional values | Fixed (`6a1e1ce64`) |

## Findings

### F60 — Medium: mixed-type division reduces the wrong result object

**Status:** fixed, committed as `b9ade72ca`.

**Location:** `libkc3/tag_div.c`, S8/Ratio and Uw/S16, Uw/S32, Uw/S64
branches; `libkc3/ratio.c`, `ratio_init_s8`.

These branches wrote their answer into the caller's destination and then
tried to reduce a separate, untouched local tag. Valid divisions such as
`(Uw) 6 / (S64) 2` consequently failed or returned Void.

The Uw branches now calculate into the local result consumed by the
integer reducer. S8/Ratio checks conversion and division, owns its local
Ratio result until success, and publishes it directly without integer
reduction. Its zero-divisor guard rejects a zero Ratio numerator.
`ratio_init_s8` also checks initialization and cleans partial results.
Failures leave the caller's destination unchanged.

Regression coverage includes mixed signed/unsigned boundaries, Ratio
values and ownership, result types, and helper-boundary failure injection.
Verification at this fix passed 496 C tests, six focused debug tag tests,
four interpreter fixtures (`integer`, `integer_div`, `integer_mod-2`,
`ratio`), and two debug interpreter fixtures (`integer`, `ratio`).
That fix guarded S8/Ratio specifically; F63 below extends the handling
to the shared Ratio helper and all real Ratio division branches.

### F61 — High: decoded Call/Complex caches retain freed addresses

**Status:** fixed, committed as `fb6690bf9`.

**Locations:** `libkc3/marshall_read.c:2094,2209`;
`test/marshall_read_test.c:1557`.

The decoders cached a raw address and transferred the object's sole
ownership to the first caller. Cleaning that result left a dangling cache
entry. Reading the same serialized offset again copied or referenced
freed storage in either copy mode. Ordinary shared serialized pointers
suffice; malformed input is not required.

A temporary audit probe confirmed that the cached address survived object
deletion in both modes. It stopped before dereferencing freed memory.

Each decoder now retains the canonical object in an owning tag in the
existing reader retention pool. First reads and cache hits return copies
or counted references. Reset and reader cleanup release the cache's
ownership. Allocation/decoding failures retain a valid cleanup path.
No reader layout or wire-format change was needed.

The new regression covers both copy modes, releasing all caller results
between shared-offset reads, nested Calls and containers, large Integer
Complex components, copy independence, a conflicting-type error with an
unchanged destination, reset, and returned values outliving the reader.
It failed on the original code because the first Call had only one
reference instead of a retained cache reference as well.

Verification passed 497 C tests, 62 focused debug serialization tests,
and three interpreter fixtures (`marshall`, `call`, `complex`). Other
pointer-cache types still need ownership review; this fix covers Call
and Complex.

### F62 — Medium: Sw/Integer modulo narrows a valid large remainder

**Status:** fixed, committed as `2a9cb4681`.

**Locations:** `libkc3/tag_mod.c:664–673`, `test/tag_test.c:846`,
`test/ikc3/integer_mod-2.kc3`.

The backend computed an exact Integer remainder, but the dispatcher
converted it to Sw before reduction. The divisor determines the backend
remainder's sign, so a remainder can exceed the dividend's native range.

Before the fix, `(Sw) -1 mod (Integer) 18446744073709551616` returned
`-1` on the native 64-bit build. The equivalent S64/Integer operation
correctly returned `18446744073709551615`.

The branch now keeps the remainder in the local Integer result tag until
the shared reducer selects its representation. Checked helper calls and
common failure cleanup remain in use.

Fifteen new C vectors cover large results of both signs, results beyond
U64 range, `SW_MIN`/`SW_MAX`, zero, small values, reduced result types,
and caller metadata. Existing Sw/Integer helper-failure tests also pass.
Ten interpreter checks cover `mod`, `%`, signs, zero, and U64/Integer
result types. The new C regression failed before the production fix.

Verification passed:

- Full C suite: 498 reported tests.
- Focused tag suite: seven tests in release and seven in debug.
- Interpreter fixtures: `integer` and `integer_mod-2`, both passing.
- `git diff --check`.

### F63 — Medium: Ratio division by zero returns an invalid Ratio

**Status:** fixed in `ratio_div` and all real Ratio division branches of
`tag_div`, committed as `b0c4565e8`.

**Locations:** `libkc3/ratio.c:70`, `libkc3/tag_div.c:1241`,
`libkc3/ratio.h`; regression cases at `test/ratio_test.c:362` and
`test/tag_test.c:1003`, plus `test/ikc3/ratio.kc3`.

Before the fix, `ratio_div` checked a nonzero divisor numerator only with
an assertion. Release builds could produce a zero denominator, and
`ratio_simplify` likewise relied on an assertion for its positive-denominator
invariant. Most mixed Ratio dispatch branches also ignored `ratio_div`'s
return value and changed the caller's destination before success.

Observed release results at the original audit baseline:

- `1/2 / (U8) 0` returned `1/0`.
- `1/2 / 0/1` returned `1/0`.
- Control: `type((S8) 1 / 0/1)` emitted `tag_div: division by zero` and
  returned `Void`, using F60's branch-specific guard.

The original debug assertion would abort for a zero Ratio divisor. No
such abort was deliberately run. No downstream memory-corruption effect
was established.

**Fix:** `ratio_div` now checks the divisor numerator at runtime and
returns NULL with `ratio_div: division by zero` before allocating or
changing the destination. The C header records the failure contract.
All 29 real Ratio branches in `tag_div` now check their conversion helper
and use one shared division/result path. That path rejects a zero Ratio
numerator (including converted scalar zeros), reports
`tag_div: division by zero`, checks `ratio_div`, cleans temporary values,
and publishes the owned Ratio only after success. It preserves caller
metadata and the complete destination on failure. The interpreter's
existing null-result handling consequently returns `Void`.

**Regression coverage:** the direct Ratio case checks positive, negative,
and zero dividends against `0/1` and `0/7`, including failure destinations
that alias either input. Six successful vectors check exact reduction,
signs, a large result, positive denominators, and ownership after input
cleanup. The tag case covers all fifteen real numeric types supported
by this native build, in both directions, with zero-divisor rejection,
unchanged owned destinations, valid positive/negative/zero quotients,
Ratio result type, caller metadata, and result lifetime after input cleanup.
The existing included-dispatcher wrappers now cover the conversion helpers
for these types. Thirty successful controls and 58 injected conversion or
division helper failures verify ownership transfer and temporary cleanup.
Sixteen added interpreter checks cover diagnostics/Void, signed floating
zero, valid mixed quotients, and Ratio result type.

Verification passed on native 64-bit OpenBSD:

- `KC3_TEST='ratio tag' make test_libkc3`: 13 reported tests.
- `KC3_TEST='ratio tag' make test_libkc3_debug`: 13 reported tests.
- `. ./env && make test_libkc3`: 500 reported tests, 28.32 seconds.
- `IKC3_TEST='ratio integer integer_div' make test_ikc3`: three fixtures.
- `IKC3_TEST=ratio make test_ikc3_debug`: one fixture.
- `git diff --check` and audit whitespace checks passed.

Commands used the repository environment (`. ./env`). ASan and a separate
platform run were unavailable. Failure injection occurs at helper
boundaries; raw backend allocator failures and internal allocation errors
in the legacy Ratio constructors were not injected. Complex arithmetic
and general invalid-Ratio handling outside division remain outside this
fix's claim. Fractional float conversion was left for F64 below.

### F64 — Medium: mixed float/Ratio arithmetic discards fractions

**Status:** fixed, committed as `6a1e1ce64`. Baseline: `b0c4565e8`.

**Locations:** `libkc3/ratio.c:222` (shared float constructor definition),
`libkc3/ratio.h`, `libkc3/tag_add.c:1138`, `libkc3/tag_sub.c:1385`,
`libkc3/tag_mul.c:1391`, and F63's checked Ratio path in `tag_div`.
Regression cases: `test/ratio_test.c:505`, `test/tag_test.c:1119`, and
`test/ikc3/ratio.kc3`.

Before the fix, float-to-Ratio constructors converted the numerator
through Integer and set the denominator to one. The Integer conversion
truncated fractions; F80/F128 Integer conversion also narrowed through
F64. Mixed arithmetic used these constructors and lost fractional input
before performing the operation.

Observed release results at the original audit baseline:

- `(F64) 0.5 / 1/2` returned `0/1` instead of a value of one.
- `1/2 + (F64) 0.5` returned `1/2` instead of a value of one.

The operand `0.5` is binary-exact, so ordinary floating-point rounding
did not explain the defect. After F63, a fractional float divisor that
truncated to zero was rejected as a zero divisor as well.

**Conversion contract and fix:** mixed arithmetic continues to return a
Ratio. Each finite float is converted to its exact stored binary value,
not to a decimal approximation or an integer. Float-to-Ratio casts use
the same constructors. For example, F64 `0.1` becomes
`3602879701896397/36028797018963968`; F32 `0.1` has a different exact
binary value and becomes `13421773/134217728`.

A shared constructor definition handles F32, F64, and conditionally F80
and F128 without narrowing extended types. Exact scaling by powers of two
normalizes the finite magnitude; bit extraction builds its significand
in an Integer. Stopping at the last set bit leaves an odd significand.
The exponent then determines a numerator shift or a power-of-two
denominator, yielding reduced form without losing any fractional bits.
Both signed zeros become `0/1`. Binary floating point is a compile-time
requirement, matching the project's IEEE floating types.

NaN and infinities return NULL with a type-specific diagnostic. Every
constructor initialization/backend operation is checked; failure cleans
the local result and leaves the destination unchanged. The header records
this contract. Float/Ratio branches in addition, subtraction, and
multiplication now check conversion and arithmetic, own their temporary
results, and publish only after success. Their failure paths preserve
the full destination and clean temporaries. Division already uses F63's
checked shared path. All 24 configured-source float/Ratio conversion
call sites across these four operators now check the constructor result.
The English Ratio documentation explains exact conversion and non-finite
input handling.

**Regression coverage:** the new constructor case failed on the original
implementation at F32 `0.5`. It now checks 45 native constructor vectors
across F32, F64, and F80: positive/negative fractions, signed zero, exact
binary `0.1` values, precision beyond the next smaller type, minimum
normal/subnormal values, maximum finite values, an F80 exponent beyond
F64 range, and non-finite rejection with an unchanged owned destination.
Numerator and denominator checks verify exact reduced representation.
F128 common vectors are conditional on that optional feature.

The mixed-operator case covers 117 successful fractional/zero combinations
across the three native float types, both operand orders, and all four
operators. It checks Ratio type, caller metadata, and result ownership
after input cleanup. Seventy-two non-finite combinations verify failure
without modifying an owned destination. Twenty-four successful wrapped
controls and 48 injected conversion/arithmetic helper failures verify
ownership transfer and cleanup. The included-dispatcher test dependencies
were regenerated for addition, subtraction, and multiplication. No new
source file, production failure hook, or source manifest was needed.
Twenty added interpreter checks cover the original examples, all four
operators, fractional divisors, signs, result types, exact casts, and
non-finite diagnostics/Void behavior.

Verification passed on native 64-bit OpenBSD:

- `KC3_TEST='ratio tag' make test_libkc3`: 15 reported tests.
- `KC3_TEST='ratio tag' make test_libkc3_debug`: 15 reported tests.
- `. ./env && make test_libkc3`: 502 reported tests, 28.64 seconds.
- `IKC3_TEST='ratio integer integer_add integer_sub integer_mul integer_div'
  make test_ikc3`: six fixtures.
- `IKC3_TEST=ratio make test_ikc3_debug`: one fixture.
- `git diff --check` and audit whitespace checks passed.

Commands used the repository environment (`. ./env`). F80 is enabled and
was executed, including precision/range beyond F64. F128 is disabled in
this native configuration; its conditional constructor was source-reviewed
but not compiled or executed here. ASan and other platform runs were
unavailable. Helper-boundary failures were injected; raw backend allocator
failures inside the new constructors were source-checked, not injected.
Float-to-Integer, Ratio-to-float, other arithmetic branches, and general
allocator-failure handling remain outside this fix's claim.

## Verification limits and continuation

ASan was unavailable. No separate platform run or raw allocator-failure
injection was performed for F61–F64. F62 includes portable 32-bit Sw
boundary expectations, but they have not been executed on a 32-bit target.
Helper-boundary failure injection is recorded under F60, F63, and F64.

The recorded F60–F64 findings are closed. Continue auditing remaining
arithmetic error paths and conversion directions, other serialized-object
lifetimes, parser/inspector rollback, concurrency, and platform-specific
behavior. Passing the recorded tests does not establish
full-library safety or coverage of every arithmetic type combination.
