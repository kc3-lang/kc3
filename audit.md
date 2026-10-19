# KC3 audit log — F34 onward

## Focused libkc3 re-audit — 2026-10-19

**Baseline:** `0c4d160b6` (`audit`); worktree clean at the start.
That commit removed the previous report after archiving it in history.
This file records the new pass, without renumbering the earlier findings.
Historical reports consulted: `ea2356129:audit.md` (F01–F11),
`ba9ee2cc3:audit.md` (F12–F23), and `88db53900:audit.md` (F24–F33).
Those reports retain their documented completion scope.

**Original audit method:** source inspection, arithmetic and ownership analysis, and
caller tracing. No builds, tests, probes, sanitizers, or interpreter
expressions were run during that pass. Examples below are source-derived,
not runtime reproductions. Only `audit.md` was written; no production or
test code was changed and no commit was made during the audit.

**Remediation update — 2026-10-19:** the user subsequently requested fixes
for all six findings without regression tests. The fixes below are now
implemented in `libkc3/array.c`, `buf.c`, `list.c`, `str.c`, and `time.c`.
No tests, headers, source manifests, or code comments were added or changed.
No commit was made. The original descriptions and locations below refer
to the pre-fix baseline; each section records its completed remediation.

**Scope:** selected string allocation, case conversion and date conversion
paths; array construction/access; buffer initialization and cleanup;
list/association-list indexing; and their tag, evaluator and language
bindings. Queue, hash-table iteration, Cow/tuple constructors, Base32,
and time/timespec operations were also inspected selectively. Repository
documentation, the compiled Unicode table, existing tests, and local
OpenBSD libc source were consulted where relevant.

This is **not an exhaustive audit of libkc3**. Generated arithmetic,
all parser branches, nonlocal exits, concurrency/lifetime rules, every
marshalled type, and platform-specific behavior remain incompletely covered.
The regression ideas below remain unimplemented at the user's request.
Validation uses the existing build and test infrastructure only, without
standalone probes or new test cases.

High denotes a native memory-safety defect with the stated prerequisites.
Medium denotes availability, data-integrity, or resource-lifecycle failure.
These labels do not establish exposure through a particular network service.

## Finding status

| ID | Priority | Finding | Status |
| --- | --- | --- | --- |
| F34 | High | Two-string concatenation wraps its allocation size before copying | Fixed; no new regression tests |
| F35 | High | Native array construction accepts overflowing extents and strides | Fixed; no new regression tests |
| F36 | Medium | UTF-8 case conversion assumes the output byte length is unchanged | Fixed; no new regression tests |
| F37 | Medium | Date parsing passes canonical empty-string storage to `sscanf` | Fixed; no new regression tests |
| F38 | Medium | Buffer constructors hide lock-allocation failure or leak owned bytes | Fixed at constructor boundary; no new regression tests |
| F39 | Medium | Index zero on an empty list reaches a null tag dereference | Fixed; existing debug assertion policy retained |

## Findings

### F34 — High: string concatenation allocates a wrapped size

**Locations:** `libkc3/str.c:646–663`, `libkc3/types.h:695–699`,
`libkc3/tag_init.c:664–674`, `libkc3/tag_add.c:1088–1091`.

`s_str.size` is `u32`. `str_init_concatenate` adds the two input sizes into
that field without checking overflow, allocates `tmp.size + 1`, then copies
the original, untruncated input lengths. The allocation and copies therefore
disagree when the combined length exceeds the 32-bit representation.

For two valid input strings of size `0x80000000` each, the stored sum is
zero and the destination allocation requests one byte. The first `memcpy`
still requests `0x80000000` bytes. The inputs may refer to the same valid
source storage; forged pointers or a malformed serialized object are not
required. This is an arithmetic example, not a multi-GiB runtime test.

**Impact / preconditions:** heap overwrite when sufficiently large valid
strings reach the two-argument concatenation API. String addition reaches
it through `tag_init_str_concatenate`; several native callers use it
directly. This requires large resident/addressable input storage and is
not a demonstrated small-input or unauthenticated network exploit.

F26 fixed the distinct Base64 encoder sizing path. The checked summation
in `str_init_concatenate_v` does not protect this function.

**Suggested remediation:** check the combined length and required terminator
before allocation or copying, using a type wide enough for the calculation.
Respect the string allocator's representable-size contract and leave the
destination unchanged on failure.

**Completed remediation:** `str_init_concatenate` rejects
`a->size >= U32_MAX - b->size` before adding, allocating, or copying.
This bounds the result to `U32_MAX - 1` and leaves room for the terminator
in the existing 32-bit allocation expression. Empty and aliased inputs
retain their normal copy behavior; failure does not publish a destination.

**Suggested regression:** extend `test/str_test.c` with ordinary, empty,
binary and aliased-input concatenations. Use the existing source-inclusion
allocation-hook pattern to test sums around `U32_MAX` without allocating
multi-GiB strings or reading synthetic source pointers. Overflowing sums
must be rejected before allocation/copy; accepted boundary requests can
stop at an injected allocation failure.

### F35 — High: native array extents can wrap into a tiny allocation

**Locations:** `libkc3/array.c:42–51,85–123,168–212`,
`libkc3/tag_init.c:54–65,886–903`.

`array_init` computes dimension-storage bytes, total element count, packed
strides and total data size with unchecked `uw` multiplication. It then
publishes the inconsistent layout as a successful initialization.
`array_allocate` trusts the resulting size. `array_data` checks each index
against its dimension count but only asserts that the final offset fits
the allocated size.

On a 64-bit `uw` target, construct `U8[]` with two extents:
`{2, 0x8000000000000001}`. Only two dimension records are required.
The constructor calculates:

- element count: `2 * 0x8000000000000001`, wrapping to 2;
- inner stride: 1; outer stride: `0x8000000000000001`;
- total byte size: the same product, wrapping to 2.

After `array_allocate`, index `{0, 2}` passes both dimension-bound checks
and produces offset 2, one byte past the two-byte allocation. In release,
`array_data_set` can then perform its element copy out of bounds. Debug
asserts at the final offset check instead of rejecting the constructor's
unrepresentable layout. No enormous input array or data allocation is
needed for this example; it uses a small dimensions array with large extents.

**Impact / preconditions:** out-of-bounds native array access if an embedding
caller supplies overflowing extents. Public tag-array constructors delegate
to the same implementation. No language-level dimension-construction route
or deployed network exposure was established in this pass.

This is distinct from F01: the deserializer now checks packed layouts,
while the ordinary native constructor was explicitly outside that fix.
F16's mesh-specific checks also do not validate every call to `array_init`.

**Suggested remediation:** check every dimension/count/stride/byte-size
product before allocating or publishing the layout. Preserve the existing
zero-dimensional and zero-count cases. Consider a release-mode final-offset
check as defense in depth, not a substitute for correct construction.

**Completed remediation:** `array_init` checks dimension-storage sizing,
then builds counts and strides from the innermost dimension outward with
division-based overflow checks before each multiplication. Failed layouts
free their temporary dimensions and leave the destination unchanged.
Zero-dimensional arrays and representable zero-count layouts remain valid;
an unrepresentable stored inner stride is rejected even if an outer extent
is zero. `array_data` now also checks the final offset in release builds.
This does not make arbitrary externally forged array metadata valid.

**Suggested regression:** extend `test/array_test.c` with small dimension
vectors whose products overflow, including the example above, one-dimensional
element-size overflow, and a representable outer count with overflowing
inner stride. Verify rejection and unchanged destination state without
allocating data. Retain ordinary multidimensional and zero-count coverage;
F01's marshalling tests must continue to pass.

### F36 — Medium: case conversion loses characters or adds NUL bytes

**Locations:** `libkc3/str.c:1440–1471`,
`libkc3/character.c:140–155`, `libkc3/buf.c:1726–1738,1878–1921`,
`ucd2c/ucd.c:587,11382`, `lib/kc3/0.1/str.facts:71–76`.

Both case-conversion functions allocate exactly `src->size` bytes, ignore
the return from `buf_write_character_utf8`, and return `buf_to_str`.
However, the existing simple-case mapping can change a character's UTF-8
byte length. `buf_to_str` uses the buffer capacity as the string size,
not its actual write position.

The compiled Unicode table provides a concrete pair:

- Lowercase U+023A (`C8 BA`, two bytes) becomes U+2C65
  (`E2 B1 A5`, three bytes). Its write cannot fit in the two-byte output
  buffer and returns `-1`. That error is ignored; the function returns a
  two-byte zero-filled string instead of the mapped character.
- Uppercase U+2C65 becomes U+023A. Only two bytes are written into the
  three-byte buffer, but the returned size remains 3: `C8 BA 00` includes
  an extra NUL as string data.

These are mappings already present in this checkout, not a request to
change its Unicode version or implement multi-character case folding.
Mixed strings can similarly lose mapped characters or retain unwritten
zero bytes. Fixed-buffer bounds checks prevent the expanding write;
this finding is incorrect output, not a demonstrated buffer overwrite.

**Impact / preconditions:** silent data corruption for ordinary valid
UTF-8 passed to `Str.to_lower` or `Str.to_upper`. No large input, malformed
UTF-8, or allocation failure is required.

**Suggested remediation:** size the encoded mapped output correctly, check
write results, and return its actual byte count. Preserve the existing
character mappings and explicitly test malformed/truncated-input handling
against the established string contracts rather than changing it implicitly.

**Completed remediation:** both functions share a private two-pass helper.
The first pass sums the UTF-8 sizes of the existing mapped characters with
an output-size/terminator bound; the second checks every write. Ownership
is transferred with the actual byte count, and write failure cleans the
temporary buffer without publishing the result. Expansions and contractions
therefore produce the correct length without padding counted as data.
The existing decoder and simple Unicode mappings are unchanged. As before,
conversion stops at the first undecodable or incomplete sequence; only
the converted prefix is returned, without the old unwritten padding.
Ordinary EOF and empty input remain successful.

**Suggested regression:** extend `test/str_test.c` with this pair in both
directions, adjacent ASCII and multibyte characters, several expansions or
contractions, empty input, embedded NULs, and unchanged characters. Check
exact bytes and lengths, including absence of padding counted as data.
Add language fixtures for the exposed `Str` functions if appropriate.

### F37 — Medium: a valid empty Str representation crashes date parsing

**Locations:** `libkc3/time.c:218–232`, `libkc3/str.c:788–794`,
`libkc3/base64.c:83–94`, `libkc3/buf.c:1130–1134`,
`lib/kc3/0.1/time.kc3:21`.

`str_init_empty` produces `{size = 0, ptr = NULL, free = NULL}`. This is
normal library output: Base64 encoding of empty input and reading an empty
buffer both use it. `time_init_str` passes `src->ptr.p_pchar` directly to
`sscanf`, without first rejecting empty input or handling that representation.
Its intended invalid-date return path is never safely reached for a null
backing pointer.

The minimal C path is `str_init_empty(&src)` followed by
`time_init_str(&time, &src)`. On this host, the inspected libc implementation
(`/usr/src/lib/libc/stdio/sscanf.c`) calls `strlen(str)` before scanning,
so this passes NULL to `strlen`.

The language binding exposes the same parser as `Time.from_str`.
`Time.from_str(Base64.encode(""))` is a source-derived regression candidate:
the nested encoder returns the null-backed empty value, and evaluated C
function results are passed as `Str` arguments without an extra string copy
(`env_eval.c:378–414`, `cfn.c:113–125`, `tag.c:1639–1643`). This expression
was not executed. A literal or copied empty string can have different
backing storage, so testing only a non-null `""` does not cover the defect.

**Impact / preconditions:** null dereference/process termination on an
ordinary library-produced empty value, instead of a reported invalid date.
No forged string metadata or failure injection is necessary. This finding
does not claim that arbitrary non-NUL-terminated foreign buffers satisfy
the documented KC3 string contract.

**Suggested remediation:** reject empty input safely before calling the
C-string parser, preserving the existing invalid-date result and leaving
the destination unchanged. Keep any broader date-format/range-policy change
separate from this representation bug.

**Completed remediation:** `time_init_str` checks for zero length or a NULL
backing pointer before `sscanf`. Both empty representations now take the
existing invalid-date return path without changing the destination.
Date grammar, numeric-range handling, and timezone behavior are unchanged.

**Suggested regression:** add date-conversion coverage to the existing C
runner for canonical null-backed empty strings, non-null empty strings,
empty results from another library API, valid dates, and invalid nonempty
text. Check failure leaves the destination unchanged. Use the existing
bounded-child pattern for the crashing pre-fix case, not a standalone probe.

### F38 — Medium: buffer constructors mishandle lock initialization failure

**Locations:** `libkc3/buf.c:402–459,477–486,525–544`,
`libkc3/rwlock.c:72–102`, `libkc3/alloc.c:23–37`.

With `HAVE_PTHREAD`, `buf_init` correctly returns NULL when `rwlock_new`
fails and does not publish its temporary object. Several neighboring
constructors fail to preserve that contract:

- `buf_init_const` does not check `rwlock_new` at all. It returns a buffer
  with `rwlock == NULL` as a successful initialization.
- `buf_init_1`, `buf_init_1_const`, and `buf_init_str` ignore a NULL return
  from `buf_init`. They publish their zero-initialized temporary anyway,
  setting `wpos` to the source length while leaving pointer, size and lock
  unset. `buf_init_str_const` inherits the constant constructor's failure.
- `buf_init_alloc` allocates its byte storage first, then directly returns
  the result of `buf_init`. If lock initialization fails, the byte allocation
  is lost: it was never installed in the caller's buffer and is not freed.

**Impact / preconditions:** under allocation or pthread-lock initialization
failure, release builds can leak an already-allocated buffer or report
success with an unusable buffer. Ordinary locked operations on a falsely
successful object then reach a null lock. Debug allocation/lock failure
paths may abort earlier under the existing assertion policy. This is not
a normal-success leak or a claim that resource exhaustion is remotely
inducible in a particular deployment.

F33 balanced temporary buffers after successful initialization and checked
RPC's initialization results. It did not repair these constructors' own
failure contracts; merely checking their return values cannot detect a
false success.

**Suggested remediation:** propagate initialization failure consistently,
avoid publishing partial buffers, and free internally allocated bytes when
lock creation fails. Preserve caller ownership of borrowed storage and
define ownership of supplied storage on failed initialization. Audit
callers that currently assume these constructors cannot fail.

**Completed remediation:** `buf_init_const` checks lock creation, and
`buf_init_1`, `buf_init_1_const`, `buf_init_str`, and `buf_init_str_const`
propagate failure without publishing their temporary objects.
`buf_init_alloc` frees its internally allocated bytes if `buf_init` fails;
it also rejects `UW_MAX` before adding the terminator byte. The heap wrapper
`buf_new` now frees its newly allocated wrapper and returns NULL on failed
initialization. Other allocation/copy constructors already check their
delegated initializer and retain that behavior.

Caller-supplied storage, including storage passed with `p_free == true`,
remains caller-owned when initialization fails; ownership transfers only
on success. This closes the reported false-success and constructor-local
leak paths. It is not a repository-wide allocation-failure cleanup audit:
existing consumers outside the constructor family still include unchecked
buffer-initialization calls and must not be assumed safe under resource
exhaustion merely because the constructor contract is now correct.

**Suggested regression:** extend the existing buffer tests with test-local
lock-allocation/initialization failure hooks. Check each constructor's
return, unchanged destination, allocation balance, and borrowed-byte
ownership. Verify successful constructors still support normal reads,
writes and cleanup. Run the pthread-disabled variant when supported;
do not induce process-wide memory exhaustion to test these paths.

### F39 — Medium: empty-list indexing falls through to a null tag

**Locations:** `libkc3/list.c:32–74,161–172`,
`libkc3/kc3.c:145–161`, `libkc3/alist.c:29–74`,
`libkc3/tag.c:589–597`, `lib/kc3/0.1/init.kc3:21–25`.

An empty list is represented by a NULL list pointer. `list_at` checks for
the end of the list only inside `while (position)`. For position zero,
that loop is skipped and it calls `tag_init_copy(dest, &l->tag)` with
`l == NULL`. `tag_init_copy` dereferences the source tag. The initial
`assert(list)` aborts debug builds but provides no release-mode check.

This is reachable through the normal indexing dispatch, not just an
invalid direct call to a non-null-only helper. `kc3_access` treats the
empty list as an association list; `alist_get` finds no entry and returns
NULL; the fallback calls `list_access` with the same empty list and numeric
key zero; that delegates directly to `list_at(NULL, 0, dest)`.
Thus indexing an empty list at zero is a source-derived crashing case.

**Impact / preconditions:** process termination for an ordinary out-of-range
index on an empty list. By contrast, stepping past the end of a nonempty
list reaches the explicit NULL error return in release builds. No corrupted
list links or oversized index is required. This is a null dereference,
not an arbitrary read/write primitive.

**Suggested remediation:** handle the empty-list case before accessing
its first tag, consistently with the project's out-of-range access policy.
Ensure the association-list fallback cannot turn a missing element into
an unchecked dereference, and preserve destination ownership on failure.

**Completed remediation:** `list_at` traverses only while a list node is
present and checks for a missing node before copying its tag. Empty lists
at zero or nonzero indices now use the same diagnostic/error path as other
out-of-range accesses, without modifying the destination. Release builds
return NULL. The existing out-of-range debug assertion is intentionally
retained; this does not change the project's general assertion policy.
The association-list fallback therefore cannot reach an unchecked first
tag through this function.

**Suggested regression:** extend `test/list_test.c` with empty lists at
zero and nonzero indices, singleton access, and out-of-range nonempty
access. Exercise the actual `kc3_access` dispatch as well as `list_at`;
retain associative-list and nested indexing behavior. Use the existing
C runner's bounded-child pattern where the pre-fix path terminates.

## Disposition and limits

- F34–F39 have the scoped code fixes recorded above. No new regression tests
  were added, as requested.
- Earlier F01–F33 closures are not reopened wholesale. F35 and F38 identify
  distinct constructor paths outside the earlier fixes' documented scope.
- No dedicated runtime reproduction, exploit demonstration, sanitizer run,
  multi-GiB test, fault injection for F34–F39, cross-thread analysis, or
  alternate-platform build was performed. Existing-suite results do not
  establish dedicated coverage of the new boundary/failure cases.
- The scope and preconditions above matter: large strings for F34, native
  shape construction for F35, ordinary UTF-8 for F36, ordinary empty values
  for F37/F39, and resource-failure paths for F38.
- No changes were committed.

## Validation of this remediation

- `. ./env && make`: passed, including release and debug compilation.
- `. ./env && make test`: passed; all 511 C tests and all integration suites
  passed.
- `git diff --check`: passed; the new report also has no whitespace errors.
- Build and test logs: `/tmp/kc3-f34-f39.TiWPMkqu/`.
- No new test cases or standalone probes were created; only existing suites
  were run.
