# libkc3 audit — 2026-11-09

**Baseline:** `c9f4400a1` (`audit`). This report begins with the new
findings from the resumed whole-library pass. Earlier findings and repair
history are intentionally omitted.

**Progress:** F73–F92 are fixed. F88 and F89 were separated from F78 during
review. This closes the defined native audit pass described below and tracks
the completed repairs.

## Scope and method

The pass inventories all 242 non-hidden C translation units in `libkc3`
and compiles each unit with the configured native feature flags plus
`-Wall -Wextra -Wuninitialized -Wconditional-uninitialized
-Wnull-dereference -Warray-bounds -Wformat-security`. All 242 units compile;
six warnings were inspected. One led to F80. The other five are control-flow
warnings in `buf_parse.c`, `env_eval.c`, `file.c`, and `hash.c`;
their reported paths were either correlated with an existing guard or did
not fail under a native probe.

Manual review covers allocation products and failures, buffer positions and
callbacks, container iteration, string indexing and trimming, floating
parsers and arithmetic dispatch, Complex error propagation, environment
fork identity, filesystem error paths, IPv4/IPv6 address formatting,
marshaller ownership, Facts/storage paths, FFI/control flow, threads and
locks, TLS/socket/DNS helpers, encoding, hashes, time, and platform branches.
Runtime probes use the release library on native OpenBSD amd64, with one
case per process, timeouts, and core dumps disabled for the deliberate
allocation-failure crash.

A deterministic malformed-input parser pass initially isolated the two
non-terminating cases recorded as F86 and F87. After removing those two
known trigger characters from the generated alphabet, 10,000 inputs of up
to 64 bytes completed within the bound. This is bounded robustness coverage,
not exhaustive fuzzing.

The installed Clang reports that its static analyzer action was not compiled
in. ASan and F128 are unavailable in this configuration. Those tools and
other operating systems are outside the validation claim.

Each finding ID below represents one root defect. Repeated instances of the
same generated or copied defect remain grouped, while independent defects
in the same function have separate IDs.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F73 | High | Reverse buffer transfer trusts a flush result without rechecking capacity | Fixed; `buf_xfer_reverse_capacity` passes |
| F74 | High | Set and block constructors overflow their allocation products | Fixed; `set_init_size_overflow` and `do_block_init_size_overflow` pass |
| F75 | Medium | Empty/exhausted hash-table iteration reports success with a null item | Fixed; `ht_iterator_empty` and `ht_iterator_exhausted` pass |
| F76 | Medium | IPv6 address formatting reads bytes from the wrong structure field | Fixed; `socket_addr_to_str_ipv6` passes |
| F77 | Medium | Square-root dispatch rejects all floating tags | Fixed; `tag_sqrt_f32`, `tag_sqrt_f64`, and `tag_sqrt_f80` pass |
| F78 | Medium | `complex_norm` publishes a result after an arithmetic operation fails | Fixed; `complex_norm_arithmetic_failure` passes |
| F79 | Medium | `file_pwd` dereferences a null allocation | Fixed; `file_pwd_alloc_failure` passes |
| F80 | Medium | `str_character` accepts an out-of-range character index | Fixed; `str_character_out_of_bounds` passes |
| F81 | Medium | `str_trim` discards content after the first internal space | Fixed; `str_trim_boundary_space` passes |
| F82 | Medium | The F32 fractional divisor wraps to zero on a long literal | Fixed; `buf_parse_f32_fractional_divisor` passes |
| F83 | Medium | Forked environments issue colliding object identities | Fixed; `env_fork_identity` passes |
| F84 | Medium | `buf_peek_to_str` reports success after allocation failure | Fixed; `buf_peek_to_str_alloc_failure` passes |
| F85 | Medium | Float-to-integer casts invoke undefined behavior outside the destination range | Fixed; `integer_cast_float_out_of_range` and `integer_cast_float_truncation_boundaries` pass |
| F86 | Medium | A final line comment without a newline makes the parser loop forever | Fixed; `buf_parse_comment_eof` passes |
| F87 | Medium | A short dotted module symbol makes `buf_parse_sym` loop forever | Fixed; `buf_parse_sym_short_module` passes |
| F88 | Medium | Complex-to-float conversions return an indeterminate value after norm failure | Fixed; `complex_to_float_norm_failure` passes |
| F89 | Medium | Complex-to-float conversions leak the temporary norm tag | Fixed; `complex_to_float_norm_cleanup` passes |
| F90 | Medium | `socket_addr_to_str` gives `inet_ntop` the socket structure length as output capacity | Fixed; `socket_addr_to_str_ipv6` passes |
| F91 | Medium | `socket_addr_to_str` reports conversion failure as success | Fixed; `socket_addr_to_str_invalid_family` passes |
| F92 | Medium | The F64 parser overflows its mantissa before applying the decimal scale | Fixed; `buf_parse_f64_mantissa_overflow` passes |

## Finding test layout

The cases live with the API they exercise. Buffer transfer, parsing,
strings, tags, sets, and environment forks extend their existing test
modules. APIs that did not have a target now have focused modules named
after the API: `buf_peek_to_str_test.c`, `complex_test.c`,
`do_block_test.c`, `file_pwd_test.c`, `ht_test.c`,
`integer_cast_test.c`, and `socket_addr_test.c`. Crash, assertion, and
non-termination paths run in bounded child processes.

## Findings

### F73 — High: reverse buffer transfer writes beyond declared capacity

**Location:** `libkc3/buf.c:2230-2250` (`buf_xfer_reverse`).

When the destination lacks space, the function accepts any flush callback
return value at least as large as the transfer. It never revalidates
`wpos` and `size` after the callback. A callback can claim progress
without changing the buffer, after which the reverse-copy loop writes past
the destination's declared size. Position subtraction/addition is also
performed before validating the positions and available capacity.

**Reproduction:** a four-byte source, a destination with declared size two,
and a flush callback returning four produce result 4 and `wpos == 4`.
Bytes at backing-array offsets two and three change from sentinels to
`b` and `a`, demonstrating writes outside the declared buffer. This is
the reverse-transfer path; the earlier forward `buf_xfer` repair does not
cover it.

**Resolution:** `buf_xfer_reverse` now validates both buffers before doing
position arithmetic, rejects transfers larger than `SW_MAX`, and checks
the actual source and destination bounds again after flushing. A flush
callback can no longer authorize a write without freeing enough capacity.

### F74 — High: set and block constructors underallocate after size overflow

**Locations:** `libkc3/set__tag.c:181-196`,
`libkc3/set__fact.c:181-196`, `libkc3/set__uw.c:181-196`, and
`libkc3/do_block.c:40-51`.

The set constructors evaluate `max * sizeof(item_pointer)` without an
overflow check. `do_block_init` does the same with
`count * sizeof(s_tag)`. Each records the original element count and
treats a wrapped nonzero allocation as success. Subsequent initialization,
access, and cleanup use the huge recorded count against the undersized
allocation.

**Reproduction:** on this 64-bit build,
`max = UW_MAX / sizeof(void *) + 2` is 2305843009213693953 and the product
wraps to 8. All three constructors return success with one allocated bucket
and the huge `max`. A block count of 128102389400760777 wraps its
allocation to 272 bytes and also returns success. The probes free the raw
allocations directly because ordinary cleanup would iterate the huge
recorded counts. No insertion is needed to confirm the inconsistent set;
insertion would index the undersized table.

**Resolution:** all three set constructors now reject `max` values whose
item-pointer allocation would exceed `UW_MAX`. `do_block_init` applies the
same check to its tag allocation. Each function returns null without
changing its destination when the product cannot be represented.

### F75 — Medium: hash-table iteration cannot represent an empty end

**Location:** `libkc3/ht.c:210-247` (`ht_iterator_next`).

The bucket scan stops at `size - 1`, but the end check requires
`position >= size`. For an empty table, `items` remains null while the
end check is false, and the function returns success with
`&i->items->tag`. Repeated calls remain at the same position.

**Reproduction:** an initialized empty table of size two returns a non-null
result twice, sets the output item to null both times, and leaves
`position == 1`. Callers can loop indefinitely or dereference a null item.

**Resolution:** the iterator now clears the destination before searching,
advances through every bucket, and records `position == size` when it
reaches the end. Empty, newly exhausted, and repeated post-exhaustion calls
all return null without exposing an item or asserting.

### F76 — Medium: IPv6 formatting reads the wrong address field

**Location:** `libkc3/socket_addr.c:30-40`
(`socket_addr_to_str`).

The function passes `&addr->sa_data[2]` to `inet_ntop` for every address
family. That happens to match the IPv4 address offset, but an IPv6 address
starts later in `sockaddr_in6`; the supplied pointer includes flow-info
bytes instead.

**Reproduction:** a valid `::1` address formats as `::` because the pointer
given to `inet_ntop` starts before `sockaddr_in6.sin6_addr`.

**Resolution:** `socket_addr_to_str` now selects `sin_addr` or `sin6_addr`
according to the address family and verifies the supplied structure length.

### F77 — Medium: square root rejects floating tags

**Location:** `libkc3/tag_sqrt.c:28-49` (`tag_sqrt`).

The dispatcher handles Integer, Ratio, and fixed-width integers, but has no
F32, F64, F80, or F128 cases. These ordinary numeric inputs therefore take
the invalid-type path.

**Reproduction:** an F64 tag containing 4 returns null, reports
`tag_sqrt: invalid tag type: F64`, and leaves the initialized destination
unchanged. The same missing dispatch is present for every floating tag in
the source; native F128 execution is unavailable.

**Resolution:** `tag_sqrt` now dispatches F32, F64, F80, and F128 tags to
their corresponding square-root operation and preserves the input float
type in the result. The F80 and F128 implementations and named tests remain
conditional on `HAVE_F80` and `HAVE_F128`, respectively; this native build
exercises F32, F64, and F80.

### F78 — Medium: Complex norm publishes failed arithmetic

**Location:** `libkc3/complex.c:429-442` (`complex_norm`).

`complex_norm` does not initialize its temporary Complex and ignores the
return values of two multiplications, addition, and square root. It cleans
and publishes the temporaries even after a failed operation, then returns
success. A failed operation can therefore feed an uninitialized or Void tag
to the next operation and to cleanup.

**Reproduction:** the current failure case makes the first multiplication
return null while preserving an initialized destination sentinel.
`complex_norm` continues through the remaining operations, replaces the
sentinel with a Void tag, and returns success. This behavior is independent
of the floating square-root dispatch fixed in F77.

**Resolution:** `complex_norm` now zero-initializes its arithmetic
temporaries, stops after the first failed operation, cleans every temporary,
and returns null without changing the caller's destination. The named test
uses a nested Complex whose multiplication fails through the supported
numeric dispatch rather than relying on a malformed tag.

### F79 — Medium: `file_pwd` crashes on allocation failure

**Location:** `libkc3/file.c:646-657` (`file_pwd`).

After a successful `getcwd`, the function does not check `alloc(size + 1)`
before passing the returned pointer to `memcpy`.

**Reproduction:** a one-shot allocator failure injected after environment
initialization makes the call exit with signal 11 (status 139). The probe
uses an otherwise valid working directory and disables core dumps.

**Resolution:** `file_pwd` now checks the allocation before copying the
working-directory bytes. On failure it returns null without changing the
destination.

### F80 — Medium: `str_character` returns the last character out of range

**Location:** `libkc3/str.c:244-257` (`str_character`).

The loop stops when the temporary string becomes empty but does not verify
that it reached the requested character position. It returns the last
decoded character and a positive byte count. On an empty string, the same
path writes the uninitialized local `c`, which is the one confirmed
whole-library compiler warning.

**Reproduction:** querying position 99 in the two-character string `ab`
returns 1 and writes character 98 (`b`) instead of reporting an
out-of-range index.

**Resolution:** `str_character` now returns -1 when the string ends before
the requested character position and leaves the destination unchanged.
The same check prevents an empty string from publishing an uninitialized
character.

### F81 — Medium: `str_trim` truncates at internal whitespace

**Location:** `libkc3/str.c:2128-2151` (`str_trim`).

After skipping leading whitespace, the function copies characters only
until it encounters the next whitespace character. A trim operation should
remove boundary whitespace while preserving content between the boundaries.

**Reproduction:** trimming `"  hello world  "` returns `"hello"`, losing
`" world"`.

**Resolution:** `str_trim` now scans the complete string and records the
end of the last non-whitespace character. It copies the full range between
the leading and trailing boundaries, including internal whitespace.

### F82 — Medium: the F32 fractional divisor wraps to zero

**Location:** `libkc3/buf_parse.c:1483-1559` (`buf_parse_f32`).

The F32 fractional divisor is an unsigned word repeatedly multiplied by
ten; sufficiently many fractional digits wrap it to zero. The parser then
uses that zero divisor while evaluating a mathematically ordinary literal.

**Reproduction:** the F32 parser consumes all 402 bytes of `1.` followed by
400 zeroes, but returns NaN instead of exactly 1. The input is valid and
requires no malformed exponent or allocation failure.

**Resolution:** the F32 parser now uses an F64 divisor for fractional
digits. A long fraction can therefore exceed the useful F32 precision
without wrapping an integer divisor to zero.

### F83 — Medium: environment forks duplicate identity counters

**Locations:** `libkc3/env_fork.c:52-75` (`env_fork_init`),
`libkc3/pointer.c:102-123` (`pointer_init`), and
`libkc3/compare.c:523-529` (`compare_pointer`).

Every fork copies the parent's next Facts, Pointer, and Var identifiers,
then protects each copy with a separate mutex. Objects created in different
forks can therefore receive the same type and ID. Pointer comparison uses
only target type and ID, so physically different pointers can compare equal.

**Reproduction:** two environments forked from the same parent create
pointers in separate, sequential worker threads. The pointer addresses
differ, both IDs are 1, and `compare_pointer` returns zero. No concurrent
race is required. The copied Facts and Var counters have the same collision
mechanism.

**Resolution:** root environments now own the Facts, Pointer, and Var
identity counters and their mutexes. Forks retain the same identity owner,
so every allocation and unmarshalling update uses the shared counters.
Forks no longer copy the counters or create and destroy separate counter
mutexes.

### F84 — Medium: buffer peek conceals allocation failure

**Location:** `libkc3/buf.c:850-872` (`buf_peek_to_str`).

The function ignores the result of `str_init_alloc_copy`, then returns the
requested byte count. The destination can remain unchanged while the caller
is told that a new string was produced.

**Reproduction:** peeking the valid buffer `abc` with a one-shot allocation
failure returns 3 and leaves the destination equal to its previous value
`old`.

**Resolution:** `buf_peek_to_str` now returns -1 when string allocation
fails and leaves the caller's destination unchanged.

### F85 — Medium: float-to-integer casts invoke undefined behavior

**Locations:** the `*_init_cast` functions in
`libkc3/{s8,s16,s32,s64,sw,u8,u16,u32,u64,uw}.c`, beginning at line 36
in the signed files and line 50 in the unsigned files.

All ten generated fixed-width integer casts use direct C casts for F32 and
F64 tags; the unsigned variants also do so for F80. They do not reject NaN,
infinity, or finite values outside the destination range. C does not define
a floating-to-integer conversion when the truncated value is unrepresentable,
so valid numeric tags can invoke undefined behavior and produce
target/compiler-dependent results.

**Reproduction:** the actual `s8_init_cast` source was compiled with
Clang's `float-cast-overflow` trap instrumentation. Casting an F64
infinity to S8 terminates on the conversion with signal 4 (status 132).

**Resolution:** the signed and unsigned integer templates now reject
non-finite inputs and truncated values outside the destination range before
performing the C conversion. The generated S8–S64, Sw, U8–U64, and Uw
implementations were refreshed from those templates.
The ordinary build has no check and reports success. The same direct
conversion pattern is present in the other nine generated functions.

### F86 — Medium: an EOF-terminated line comment never advances

**Locations:** `libkc3/buf_parse.c:993-1042`
(`buf_parse_comment`) and `:1044-1089` (`buf_parse_comments`).

After consuming `#`, `buf_parse_comment` requires a newline. At end of
input it restores the original buffer position but returns the positive
number of bytes tentatively consumed. `buf_parse_comments` interprets
that as success and calls it again at the same position indefinitely.

**Reproduction:** `buf_parse_tag` on the one-byte finite buffer `#`
does not return within repeated one- and two-second process bounds. The
same occurs for `#text` and a trailing comment after an otherwise parsed
expression. A comment ending with a newline follows the normal path.

**Resolution:** `buf_parse_comment` now checks for a clean end of input
before peeking at the next UTF-8 character. An EOF-terminated comment is
consumed successfully, while refill and malformed UTF-8 failures retain
the existing restore path.

### F87 — Medium: short dotted module symbols never advance

**Locations:** `libkc3/buf_parse.c:4711-4798` (`buf_parse_sym`) and
`libkc3/buf.c:620-647` (`buf_peek_next_character_utf8`).

In the dotted uppercase-symbol branch, `buf_peek_next_character_utf8`
already returns the combined byte length of the current and next
characters. `buf_parse_sym` adds the current-character length again and
asks `buf_xfer` to transfer that oversized count. It treats a zero-byte
short transfer as success, leaves the read position at the dot, and repeats
forever.

**Reproduction:** the valid module symbol `A.B` does not return from
`buf_parse_tag` within a two-second process bound. At the dot, the parser
computes a three-byte transfer although only the two bytes `.B` remain.
`buf_parse_module_name` and direct symbol interning both return normally,
isolating the loop to the generic symbol parser.

**Resolution:** `buf_parse_sym` now uses the combined byte count returned
by `buf_peek_next_character_utf8` directly. The `.B` suffix advances by
two bytes and the parser returns the complete symbol.

### F88 — Medium: Complex conversion returns indeterminate data on failure

**Location:** `libkc3/complex.c:451-500` (`complex_to_f32`,
`complex_to_f64`, `complex_to_f80`, and `complex_to_f128`).

Each conversion declares its scalar result without initializing it, ignores
the return value of `complex_norm`, and ignores the return value of the
scalar cast. If norm calculation fails, the cast receives the unchanged
Void norm tag and also fails. The function then returns the indeterminate
scalar. The scalar-return API does not currently define a failure value.

**Reproduction:** a nested Complex that makes `complex_norm` fail reaches
the unchanged Void norm tag. The release conversions return non-NaN
indeterminate values, while the debug F32 conversion asserts in
`f32_init_cast`.

**Resolution:** every Complex-to-float conversion now checks both norm
calculation and scalar conversion and returns NaN on failure. The F32, F64,
and available F80 paths are covered directly; F128 uses the same guarded
path but is unavailable in this build.

### F89 — Medium: Complex conversion leaks the norm tag

**Location:** `libkc3/complex.c:451-500` (`complex_to_f32`,
`complex_to_f64`, `complex_to_f80`, and `complex_to_f128`).

After a successful `complex_norm`, none of the conversion functions calls
`tag_clean` on the temporary norm. Norms represented by Ratio or Integer
tags own allocated storage, so every conversion of such a value loses that
storage after extracting the scalar.

**Reproduction:** `complex_to_float_norm_cleanup` uses the Ratio components
`3/5` and `4/5`, whose norm is an owning Ratio tag. Its instrumented
`tag_clean` count establishes the cleanup count for `complex_norm` plus its
returned tag, then verifies that each available float conversion performs
the same number. Before the fix every conversion performs one fewer cleanup.

**Resolution:** every Complex-to-float conversion now cleans the norm after
extracting the scalar and also cleans it if the scalar cast fails. The F32,
F64, and available F80 paths are covered directly; F128 uses the same cleanup
path but is unavailable in this build.

### F90 — Medium: socket address output capacity uses the input size

**Location:** `libkc3/socket_addr.c:30-40`
(`socket_addr_to_str`).

The function passes the socket structure length to `inet_ntop` as the
capacity of its local output array. These sizes describe unrelated objects.
For IPv6, `sizeof(struct sockaddr_in6)` is too small for the longest textual
address plus its terminator.

**Reproduction:** the valid address
`ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff` makes `inet_ntop` fail with
`ENOSPC` even though the local output array is large enough.

**Resolution:** `socket_addr_to_str` now passes the full capacity of its
local output array to `inet_ntop`.

### F91 — Medium: socket address conversion failure reports success

**Location:** `libkc3/socket_addr.c:30-40`
(`socket_addr_to_str`).

When `inet_ntop` returns null, the function returns the destination pointer
instead of null. The caller is told that conversion succeeded while the
destination still contains its previous value.

**Reproduction:** an unsupported address family makes `inet_ntop` fail,
but the function returns success and preserves the destination string
`sentinel`.

**Resolution:** unsupported families and conversion failures now return
null without changing the destination.

### F92 — Medium: the F64 mantissa overflows before decimal scaling

**Location:** `libkc3/buf_parse.c:1562-1638` (`buf_parse_f64`).

The parser accumulates all mantissa digits before applying a decimal scale.
A sufficiently long mantissa makes the accumulator reach infinity while
the deferred scale reaches zero, so their final product is NaN for an
ordinary finite literal.

**Reproduction:** the F64 parser consumes all 402 bytes of `1.` followed by
400 zeroes, but returns NaN instead of exactly 1. The same intermediate
overflow affects a 401-digit integer whose `e-400` exponent should bring it
back to 1. The dedicated `buf_parse_f64_mantissa_overflow` case covers both
forms independently of F82.

**Resolution:** the parser periodically renormalizes a large finite
significand and carries the removed decimal power into its existing scale.
The significand therefore remains finite until the combined decimal and
explicit exponent is applied.

## Verification and limits

Probe sources and logs are under `/tmp/kc3-audit-completion/`, including
`probe.c`, `reverse.probe.log`, `sets.probe.log`, `ht.probe.log`,
`ipv6.probe.log`, `sqrt.probe.log`, `norm.probe.log`,
`pwd-oom.probe.log`, `character.probe.log`, `trim.probe.log`,
`parse-floats.probe.log`, `fork-ids.probe.log`, and
`buf-oom.probe.log`. The later checks add `block.probe.log`,
`cast-plain.probe.log`, `cast.probe.log`, `comment-eof.probe.log`,
`dotted-symbol.probe.log`, and `parser-bounded.probe.log`. Compiler
inventory and diagnostics are
recorded in `inventory.json`, `compiler-results.json`, and
`compiler-check.log`.

| Check | Result |
| --- | --- |
| Configured compiler diagnostics | 242/242 C translation units compiled; 6 warnings triaged |
| Current full release C suite | 545/545 pass after all finding repairs |
| Prior full release C suite with finding cases | 541 total: 532 pass; fixed cases for F73–F78, F90, and F91 pass, while 9 then-open finding cases fail |
| Earlier selected release API modules | 145 total: 136 pass; 9 then-open finding cases fail |
| Earlier selected debug API modules | 147 total: 138 pass; 9 then-open finding cases fail |
| Full debug C suite | F78 passes; the run later aborts because the configured `lib/kc3/0.2/http.so` is unavailable |
| Current parser target | F82, F86, F87, and F92 pass in release and debug; 39/39 release and 38/38 debug cases pass |
| Current F83 environment-fork target | 11/11 pass in release and debug, including shared Facts, Pointer, and Var identities |
| Current F84 buffer-peek target | `buf_peek_to_str_alloc_failure` passes in release and debug |
| Current F85 integer-cast target | `integer_cast_float_out_of_range` and `integer_cast_float_truncation_boundaries` pass in release and debug |
| Current F86 comment target | `buf_parse_comment_eof` passes in release and debug and consumes the full EOF-terminated comment |
| Current F87 symbol target | `buf_parse_sym_short_module` passes in release and debug and returns `A.B` |
| Current F88 Complex target | `complex_to_float_norm_failure` passes for F32, F64, and F80 in release and debug |
| Current F89 Complex target | `complex_to_float_norm_cleanup` passes for F32, F64, and F80 in release and debug |
| Release interpreter suite before test-only changes | 72/72 passed |
| Deterministic parser pass after isolating F86/F87 | 10,000/10,000 inputs completed |
| Finding probes | F73–F92 reproduced |
| Generated test inventory | Refreshed with `test/update_sources` and `test/configure` |
| Repository changes | Named C tests, generated test inventory, runner failure limit, the F73–F92 production fixes, and this report |

The runtime results cover native OpenBSD amd64 with F80 enabled. F128,
ASan, the unavailable Clang static-analyzer action, external network
behavior, other operating systems, and architecture-specific branches were
not executed. The complete pass here means every current C translation
unit received configured compiler diagnostics and every library family
received a risk-based manual review; it does not claim proof that no other
defects exist.
