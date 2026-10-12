# KC3 audit log

- Audit date: 2026-10-08
- Baseline commit: `4bdffa88b` (`README`)
- Scope: targeted source review of native buffers, unsigned integer parsing,
  array/struct/container deserialization, shared-object reconstruction, JSON
  parsing, HTTP framing, response parsing, file uploads, identifier evaluation,
  and the facts replication authentication parser.
- Method: the two audit passes used manual source inspection after the request
  to "just read the code." The subsequent request to fix findings and use the
  test infrastructure starts the separately recorded remediation work below.
- Source changes: F01–F10 are present in the current baseline `f25e6223a`.
  F11 batches delimiter-terminated file output and propagates write failures,
  with six core C regression groups. This diff is uncommitted for user review.
  No commits have been made by the assistant.
- Status: two targeted review passes complete; all eleven findings below are
  implemented and tested. No numbered findings remain open. This is not an
  exhaustive security assessment or a dependency audit.

Severity is a remediation priority: **High** identifies memory corruption or
input-validation defects affecting trust boundaries; **Medium** identifies
data corruption, parser contract violations, accumulating resource leaks, or
avoidable resource consumption.
All findings were originally identified through source inspection, not
successful runtime reproductions. Original example inputs describe behavior
derived from the code. Fix validation is recorded separately. Finding locations
refer to the audited baseline, before remediation changed line numbers.

## Remediation status

Work proceeds one finding at a time, with an uncommitted diff for user review.

| Finding | Status |
| --- | --- |
| F01 — Array layout validation | Present in baseline `180f93a01`; full release and focused debug tests pass |
| F02 — Unsigned parsing overflow | Present in baseline `66dda63ab`; full release and focused debug tests pass |
| F03 — Content-Length parsing | Present in baseline `c5c742381`; full release and HTTP debug tests pass |
| F04 — JSON string semantics | Present in baseline `e4c5ca46b`; full release and JSON debug tests pass |
| F05 — JSON grammar and boundaries | Present in baseline `78a813ff3`; full release and JSON debug tests pass |
| F06 — HTTP response chunk leak | Present in baseline `8f6e72d66`; scoped allocation checks, full release and HTTP debug tests pass |
| F07 — Container allocation overflow | Present in baseline `b5a995142`; full release, focused debug and authentication probes pass |
| F08 — Serialized struct layouts | Present in baseline `957c3febe`; full release, focused debug, replication and scoped allocation checks pass |
| F09 — Shared-object cache types | Present in baseline `482153eeb`; full release, focused debug, replication and scoped allocation checks pass |
| F10 — Qualified identifier lookup | Present in baseline `f25e6223a`; full release, focused release/debug and interpreter debug tests pass |
| F11 — Per-byte upload flushing | Implemented; full release, focused release/debug, HTTP debug, multipart and scoped allocation checks pass; awaiting review |

## Findings

### F01 — High: array deserialization trusts an attacker-supplied element stride

**Locations:** `libkc3/marshall_read.c:309–335`,
`libkc3/marshall_read.c:756–765`, `libkc3/array.c:41–48`.

`marshall_read_array` reads dimension counts, strides, total element count,
total byte size, and element type from the serialized input. Its allocation
check uses the serialized stride:

```c
item_size = tmp.dimensions[tmp.dimension_count - 1].item_size;
if (! item_size || tmp.count > tmp.size / item_size)
  /* reject */;
```

However, `marshall_read_data` writes according to `tmp.element_type`, whose
actual size is never compared with that stride. For a one-dimensional `U64[]`
with dimension count 1, element count 1, stride 1, and total byte size 1, the
check passes. `array_allocate` allocates one byte, then `marshall_read_u64`
writes eight bytes into it.

**Impact:** heap overwrite when a malformed serialized array is loaded.
The related dimension counts and outer strides are also not reconciled with
the allocation; `array_data` later trusts them when calculating offsets
(`libkc3/array.c:92–107`).

**Exposure:** the public `MarshallRead.tag_from_str` path reaches the tag reader
through `libkc3/kc3.c:1096–1111`. Facts replication also uses the reader, but its
connection setup performs authentication (`libkc3/facts_connection.c:166`);
this review does not establish an unauthenticated network exploit.
F07 separately identifies a header-processing issue before the replication
shared-secret check; it does not depend on array decoding.

**Suggested fix:** derive element size from the validated element type;
recompute counts, strides, and total size with checked multiplication; reject
inconsistent serialized metadata before allocation or decoding. The existing
`marshall_read_array_size` test (`test/marshall_read_test.c:1091`) checks a
reduced total size while retaining the correct stride, so it does not cover
this bypass.

**Original validation:** source trace from serialized metadata to allocation and typed
write. No malformed payload was successfully executed during this audit.

**Remediation — implemented, awaiting review (2026-10-08):**

- `marshall_read_array` checks that the array and element types agree and
  obtains the actual element size with `sym_type_size`.
- It verifies the packed strides from the innermost dimension outwards,
  checking each multiplication before computing the next extent. The wire
  byte size and element count must exactly match the checked layout.
- Validation precedes element allocation and decoding for data-backed,
  tag-backed, and unallocated arrays. Existing zero-dimensional empty arrays
  and canonical zero-count unallocated arrays remain supported.
- The wire format, constructors, and unrelated readers are unchanged. This
  does not resolve F07–F09 or make all deserialization safe.

**Regression coverage:** `marshall_read_array_layout` exercises 12 invalid
layouts in three storage modes (36 rejection checks): short, long and zero
element strides; inconsistent outer strides; incorrect element counts and
byte sizes; inner and outer multiplication overflow; inconsistent types; and
zero-sized elements. Tests check that rejection leaves the destination
untouched and does not advance into the payload. Data-backed cases include
a serialized `U64` value, so the one-element stride case is not just an EOF
failure. These negative tests follow the existing `NDEBUG` convention because
invalid input triggers assertions in debug builds.

`marshall_read_array` adds round trips for six inputs in both parsed and
evaluated forms, including multidimensional arrays, strings, and empty arrays.
`marshall_read_array_empty` covers `array_init_void` and a zero-count
multidimensional unallocated array. The older forged-size regression remains.

**Fix validation:**

- Release builds: `make -C libkc3 -j4 build` and `make -C test -j4 build` passed.
- Focused release: `KC3_TEST='array marshall marshall_read' make -C test
  test_libkc3` passed, 75/75 reported tests.
- Full core C release suite: `make -C test test_libkc3` passed, 297/297 reported
  tests, including the final regression-test changes.
- Debug builds: `make -C libkc3 -j4 debug` and `make -C test -j4 debug` passed.
- Focused debug: `KC3_TEST='array marshall marshall_read' make -C test
  test_libkc3_debug` passed, 68/68 reported tests.
- Full core debug suite was attempted but aborted in the struct tests because
  `env_dlopen` could not load `lib/kc3/0.1/http.so`. The symlink and target exist;
  the cause of that module-loading failure was not investigated or changed in
  this patch. This run is not counted as passing.
- `git diff --check` passed. ASan is unavailable in the generated configuration.
- Project-wide follow-up (2026-10-09): ran `. ./env && make test` from the
  repository root as requested. The build succeeded; core C tests passed
  297/297 and interpreter (`ikc3`) tests passed 71/71. The `kc3s` suite passed
  4/7 and failed `facts_acceptor_loop`, `facts_connection`, and `facts_secret`.
  All three reported permission denied opening `/etc/ssl/private/privkey.pem`,
  followed by TLS configuration/handshake failures. The directory is owned by
  root with mode `0700`; the tests run as `dx`. No key contents were read and
  no permissions or test configuration were changed. `make test` exited 2;
  the later integration suites were not reached. This was a release run and
  does not replace the earlier debug results.
- Latest rerun (2026-10-09): `. ./env && make test` completed successfully
  with exit status 0. All 415 reported tests across 12 suites passed: core C
  297, interpreter 71, `kc3s` 7, event polling 2, socket 1, TLS 2, PDF 4,
  embedded KC3 8, JSON 4, Markdown 2, HTTP 15, and crypto 2. In particular,
  all three previously failing replication tests now pass. No permissions,
  test configuration, or source code were changed by the assistant for this
  rerun. The full debug target was not rerun.

Full release and debug logs are retained in `/tmp/kc3-f01.BWZ8NdoI/`.

### F02 — High: unsigned integer overflow check permits wrapped values

**Locations:** `libkc3/buf_parse_u.c.in:130–146`, with generated instances in
`libkc3/buf_parse_u8.c:130`, `libkc3/buf_parse_u16.c:130`,
`libkc3/buf_parse_u32.c:130`, `libkc3/buf_parse_u64.c:130`, and
`libkc3/buf_parse_uw.c:130`. See also `libkc3/ceiling.c:31–40`.

Before multiplication, the parser checks `u > ceiling(MAX, radix)`. Using the
ceiling admits an accumulator that is already too large to multiply safely.
The subsequent addition check sees the wrapped result and can accept it.

Examples derived directly from the arithmetic:

- Parsing decimal `260` as `U8`: 26 is not greater than `ceil(255 / 10)`,
  so multiplication proceeds, narrows to 4, and the final zero is accepted.
- On a 64-bit host, decimal `18446744073709551620` becomes 4 when parsed as
  `Uw`, rather than being rejected as greater than `UW_MAX`.

**Impact:** values outside the destination type's range silently become small
numbers. In particular, HTTP request parsing uses this `Uw` parser before its
64 MiB Content-Length check (`http/http_request.c:329–339`), so that check
operates on the wrapped value and accepts an invalid declared length.

**Suggested fix:** check `u > MAX / radix` and, at equality,
`digit > MAX % radix` before multiplication/addition. Fix the generator and
regenerate all affected unsigned parsers. Cover values just beyond the limit
that wrap during multiplication, not only `MAX + 1`.

**Original validation:** arithmetic and control-flow review only; these examples were
not successfully reproduced in the temporary harness.

**Remediation — implemented, awaiting review (2026-10-09):**

- Started from `180f93a01` with no tracked source changes. Updated
  `libkc3/buf_parse_u.c.in` to reject `u > (MAX - digit) / radix` before
  multiplying or adding. This is equivalent to checking whether the next
  mathematical value `u * radix + digit` would exceed the destination limit,
  without computing an overflowing intermediate value.
- Removed the now-unused ceiling dependency and the redundant post-multiply
  addition check. The overflow diagnostic includes both the radix and digit.
  Failure still returns `-1`, restores the input position, and leaves the
  destination unchanged.
- Regenerated `buf_parse_u8.c`, `buf_parse_u16.c`, `buf_parse_u32.c`,
  `buf_parse_u64.c`, and `buf_parse_uw.c` through their existing Makefile rules.
  No signed parsers, numeric-prefix grammar, or HTTP header parsers changed.
  At that step F03 remained open: range checking is not strict HTTP framing
  validation. Its subsequent fix is recorded below.

**Regression coverage:** added three always-enabled cases in the ordinary
`buf_parse` group: `buf_parse_unsigned_limits`, `buf_parse_unsigned_overflow`,
and `buf_parse_unsigned_base`. They cover valid maxima and maximum-minus-one
values for U8/U16/U32/U64/Uw; addition overflow and multiply-wrap examples;
binary/octal/hexadecimal prefixes; leading zeroes and a long decimal input;
and a direct base-36 parser call. Failed parses must preserve a sentinel
destination, restore a nonzero starting input position, and release their
buffer-save records. Successful parses stop at the trailing delimiter.
Both 32-bit and 64-bit Uw cases are provided; this host ran the 64-bit branch.
The existing optional width-specific test groups were not enabled or changed.

**Fix validation:**

- Before changing production code, the new tests reproduced incorrect
  acceptance of decimal `260` and base-36 `80` as U8 values. The parser group
  reported 31/33 passing, with exactly those two new negative cases failing.
- Rebuilt the release and debug core libraries and test binaries. After the
  fix, `KC3_TEST=buf_parse make -C test test_libkc3` and its
  `test_libkc3_debug` counterpart both passed 33/33 with `./env` sourced.
- `. ./env && make test` passed with exit status 0: 418 reported tests across
  all 12 suites, including 300 core C tests and all HTTP/TLS integration tests.
- Generated parser targets are up to date and `git diff --check` passed.
  No full debug integration run, sanitizer run, or 32-bit cross-build was
  performed for F02.

Logs: `/tmp/kc3-f02.s0GqCVRP/before.log`, `after-release.log`,
`after-debug.log`, and `full-release.log`. The original `test/kc3.dump` was
preserved before testing and restored afterward; the newly generated dump
is retained as `/tmp/kc3-f02.s0GqCVRP/generated-test-kc3.dump`.

### F03 — High: HTTP Content-Length accepts partial and nondecimal values

**Locations:** `http/http_request.c:327–341`, `libkc3/uw.c:137–155`,
`libkc3/buf_parse_uw.c:35–81`.

`http_request_buf_parse` passes the entire header value to `uw_init_str`.
That helper accepts any positive number of consumed characters without
requiring the entire string to be consumed. Its underlying parser also
recognizes KC3 binary, octal, and hexadecimal prefixes.

Consequently, values such as `4junk`, `0x4`, and `4, 5` are accepted as 4.
The duplicate-header check compares only these parsed numbers, so it cannot
detect the conflicting lengths inside the single value `4, 5`.

**Impact:** the server accepts malformed or ambiguous message framing and
consumes a body based on its own interpretation. This creates a request
desynchronization risk when another HTTP component accepts the same bytes
with a different interpretation. No particular proxy configuration or
end-to-end request-smuggling exploit was demonstrated.

**Suggested fix:** use an HTTP-specific parser that consumes the complete
value, accepts only decimal digits after permitted whitespace handling,
checks arithmetic overflow, and rejects conflicting lengths. Fixing F02
alone does not fix partial parses or nondecimal prefixes.

**Original validation:** source trace of the header value through `uw_init_str`
and `buf_parse_uw`. No HTTP requests were sent during the source-only audit.

**Remediation — implemented, awaiting review (2026-10-09):**

- Started from `66dda63ab` with no tracked source changes. Replaced the request
  parser's `uw_init_str` call with a private HTTP-specific decimal parser in
  `http/http_request.c`. It trims only surrounding ASCII SP/HTAB, requires at
  least one ASCII decimal digit, and rejects any other byte anywhere in the
  remaining value. It checks `length > (UW_MAX - digit) / 10` before multiplying
  or adding, and assigns its destination only after complete validation.
- Preserved the 64 MiB body limit, case-insensitive header-name matching,
  acceptance of numerically identical duplicate header lines, and rejection
  of conflicting duplicates and unsupported Transfer-Encoding. Invalid syntax,
  arithmetic overflow, and lengths over the body limit produce an explicit
  `invalid Content-Length` diagnostic and use the existing cleanup path before
  body reading. The public parser's existing `void` failure result is unchanged.
- Comma-separated values are rejected, including lists of identical lengths.
  This follows the decimal grammar and permitted rejection policy in
  [RFC 9110 §8.6](https://www.rfc-editor.org/rfc/rfc9110.html#section-8.6);
  surrounding SP/HTAB handling follows HTTP field-value whitespace rules in
  [RFC 9112 §5.1](https://www.rfc-editor.org/rfc/rfc9112.html#section-5.1).
- Kept the change local to request Content-Length values. General header
  splitting (including its existing requirement for a literal `": "`
  separator), response parsing, and server connection handling were not
  changed. This is not a claim of complete HTTP framing compliance.

**Regression coverage:** `test/http/16_content_length.kc3` and its expected
output/return fixtures add 36 cases to the existing HTTP harness: eight valid
cases and 28 rejection cases. They cover zero and multidigit lengths, leading
zeroes, surrounding SP/HTAB, mixed-case names, matching and conflicting
duplicates, KC3 base prefixes, suffixes, comma lists, signs, fractions,
exponents, empty/whitespace-only values, embedded whitespace, control/NUL bytes,
non-ASCII digits, the body limit, and integer-overflow inputs. Valid cases
check the decoded body and parse a following pipelined request. Rejection
cases require `void` and verify that the body and following request bytes
remain unread. The new fixture uses memory buffers, not network listeners.

**Fix validation:**

- An initial fixture run failed before reaching header parsing because it
  loaded HTTP.Request without HTTP's upload-module initialization. Added
  `require HTTP`, matching existing request tests; no production change was
  made for that setup issue.
- Before changing production code, the corrected fixture reproduced acceptance
  and body consumption for `4junk`, `0x4`, binary/octal prefixes, comma lists,
  fractions/exponents, embedded whitespace/NUL, and malformed duplicate values.
  It also showed rejection of the valid leading-SP/HTAB cases. The new fixture
  failed as intended; the output and diff were preserved.
- After the fix, `. ./env && HTTP_TEST=16_content_length make test_http`
  passed the new fixture with all 36 expected results.
- `. ./env && make test_http_debug` rebuilt the relevant debug libraries and
  interpreter, selected debug library links, and passed all 16 HTTP fixtures.
- `. ./env && make test` restored release library links, rebuilt the project,
  and passed with exit status 0: 419 reported tests across all 12 suites,
  including 300 core C tests and all HTTP/TLS integration tests.
- `git diff --check` passed. No full debug integration run, sanitizer run,
  32-bit cross-build, or proxy/request-smuggling exploit test was performed.

Logs and baseline failure artifacts are in `/tmp/kc3-f03.Zo4TCRVd/`:
`fixture-setup.log`, `before.log`, `before.out`, `before.diff`,
`after-release.log`, `after-debug.log`, and `full-release.log`.
The pre-existing `test/kc3.dump` was preserved and restored. Tests also
regenerated the tracked HTTPS/TLS client `.kc3c` caches; their generated
versions were preserved in the same temporary directory and the originally
clean tracked versions restored. No generated binary changes remain in the
review diff.

### F04 — Medium: JSON strings use KC3 escapes and interpolation semantics

**Locations:** `json/json.c:389`, `json/json.c:469–473`,
`libkc3/buf_parse.c:4419–4435`, `libkc3/buf_parse.c:4456–4475`,
`libkc3/buf_parse.c:4478–4490`, `libkc3/buf_parse.c:5677–5681`,
`libkc3/str.c:1627–1697`.

Both JSON string values and object keys call `buf_parse_tag_str`. That is the
KC3 language string parser: it applies KC3 escape rules and builds call
objects for interpolation expressions.

Examples of valid JSON strings that receive the wrong interpretation:

- `"\u0041"` becomes `u0041`, not `A`: the KC3 Unicode parser requires a
  `+` after `u`, and absence of that plus is not treated as an error here.
- `"\b\f"` becomes `bf`, not backspace followed by form feed.
- `"#{1 + 1}"` becomes a `TAG_PCALL` tree instead of a literal string.
  The same problem applies to object keys containing interpolation syntax.

**Impact:** ordinary JSON text can change value or type during decoding.
Code expecting string values or keys can fail or process different data.
Passing the returned call tree to `env_eval_tag` would execute it
(`libkc3/env_eval.c:1196–1197`), but this review does **not** establish that
`JSON.from_str` executes interpolation by itself. Remote JSON reaches this
parser through `HTTPS.Client.response`
(`lib/kc3/0.1/https/client.kc3:172–178`).

**Suggested fix:** implement a JSON-specific string decoder that always
returns `TAG_STR`, implements JSON escapes and UTF-16 surrogate handling,
and treats interpolation syntax as ordinary text. Use it for keys and values.

**Original validation:** manual trace through string decoding and call construction;
no successful runtime reproduction or code-execution demonstration.

**Remediation — implemented, awaiting review (2026-10-10):**

- Started from `c5c742381` with no tracked source changes. Replaced the KC3
  string parser in `json_buf_parse_str` with a JSON-specific decoder, and routed
  object keys through it too. Successful decoding always produces `TAG_STR`;
  neither raw nor Unicode-escaped interpolation markers are interpreted as KC3.
- Implemented the JSON short escapes and exactly four ASCII hexadecimal digits
  per `\uXXXX` escape, including UTF-16 surrogate pairs. Escapes are decoded
  once, so an escaped backslash cannot introduce another decoding pass.
- Raw input must be shortest-form UTF-8 representing Unicode scalar values.
  Unescaped ASCII controls, unknown/KC3-only escapes, malformed UTF-8,
  truncated strings/escapes, and unpaired surrogates are rejected. The
  surrogate policy is deliberately strict: the interoperability warning in
  [RFC 8259 §8.2](https://www.rfc-editor.org/rfc/rfc8259.html#section-8.2)
  discusses unpaired surrogates admitted by the grammar. JSON escape handling
  follows [RFC 8259 §7](https://www.rfc-editor.org/rfc/rfc8259.html#section-7).
- The decoder validates and measures saved input before allocating exactly
  the decoded length plus its NUL terminator, then decodes it. It retains the
  existing `STR_MAX` decoded-size limit (16 MiB), bounds writes against the
  measured capacity, and assigns the destination only on success. Failure
  frees temporary output, restores the input position, and removes the save
  record. Embedded escaped NUL is retained as a string byte.
- Removed the array/object parsers' input-error assertions so a rejected string
  inside a container returns failure and cleans up in debug builds too.
  Existing diagnostics remain. Container delimiter grammar, document-boundary
  handling, number parsing, core KC3 parsing, and JSON serialization are
  unchanged by F04. F05's subsequent fix is recorded below. In particular,
  `JSON.to_str` still uses KC3
  string inspection and can emit KC3-only escapes; this decoding fix is not
  a claim of full JSON compliance or universal round-trip compatibility.

**Regression coverage:** added `test/json/strings.kc3` and
`test/json/strings_invalid.kc3`, with expected output/return fixtures, to the
existing JSON harness. There are 23 valid-result checks and 56 rejection
cases. Coverage includes every short escape, mixed-case hex digits, NUL,
UTF-8 width boundaries, valid surrogate pairs through U+10FFFF, literal
interpolation text in keys and values, nested containers, and a 128 KiB decoded
string. Streaming checks verify that a valid string leaves subsequent bytes
unread and that invalid input restores a nonzero starting position. Three
negative cases reject strings after partial array/object allocation.

**Fix validation:**

- Before changing production code, both new fixtures failed: valid JSON
  escapes decoded incorrectly, literal interpolation returned a non-string,
  and malformed escapes/control characters were accepted. The baseline
  outputs and diffs were preserved; no code-execution exploit is claimed.
- During validation, found that the fixture's `Buf.write_str` normalizes
  malformed UTF-8. The negative fixture now binds the existing byte-oriented
  `buf_write` through a local Cfn, preserving the exact test bytes. No core
  buffer code was changed. The initial baseline run is not evidence that
  every original malformed UTF-8 byte sequence reached the JSON parser.
- `. ./env && make test_json` and `. ./env && make test_json_debug` both
  rebuilt the relevant libraries and passed all six JSON fixtures, including
  all 79 new valid/rejection checks. Invalid container strings return normally
  in debug mode rather than triggering the removed assertions.
- `. ./env && make test` passed with exit status 0: 421 reported tests across
  all 12 suites, including 300 core C tests and all HTTP/TLS integration tests.
  The root target restored release library links after debug testing.
- `git diff --check` passed. No full debug integration run, sanitizer run,
  fuzzing campaign, allocation-failure injection, or network exploit test
  was performed for F04.

Logs are in `/tmp/kc3-f04.9qHpbxnf/`: `before.log`,
`before-strings.out`, `before-strings.diff`, `before-invalid.out`,
`before-invalid.diff`, `after-release.log`, `after-debug.log`, and
`full-release.log`. The original `test/kc3.dump` was preserved and restored;
the newly generated dump is retained in the same directory. Work resumed
after the user's interruption; no audit test process remained running when
the workspace was rechecked.

### F05 — Medium: JSON document parsing does not enforce document boundaries or separators

**Locations:** `json/json.c:275–304`, `json/json.c:307–314`,
`json/json.c:404–414`, `json/json.c:476–488`.

`json_from_str` parses one value and returns without checking whether all
input was consumed. For example, `trueXYZ` is accepted as `true` because the
Boolean parser consumes the prefix and the caller ignores the remainder.
Conversely, leading JSON whitespace is not skipped before dispatch, so
` true` is rejected.

Within objects, if neither a comma nor a closing brace follows a value, the
loop continues to parse another key. Therefore `{"a": 1 "b": 2}` is accepted
despite its missing comma. Trailing commas are also allowed by the container
loops.

**Impact:** invalid documents can be treated as valid, valid whitespace-prefixed
documents fail, and different consumers can disagree about the document being
processed. These are parser correctness problems; an application-specific
security bypass has not been established.

**Suggested fix:** distinguish streaming value parsing from complete-document
parsing. The latter should consume permitted leading/trailing whitespace and
require EOF. Container parsers should explicitly require either their closing
delimiter or a comma followed by another member.

**Original validation:** source review of parser transitions and return paths only.

**Remediation — implemented, awaiting review (2026-10-10):**

- Started from `221a98c31` with no tracked source changes. During this work,
  the user updated the baseline to `e4c5ca46b`; the final diff remains limited
  to the F05 parser changes and fixtures. No commits were made by the assistant.
- Added a private byte-oriented whitespace helper accepting only space, tab,
  carriage return, and line feed, as specified by
  [RFC 8259 §2](https://www.rfc-editor.org/rfc/rfc8259.html#section-2).
  JSON container parsing no longer accepts KC3's broader Unicode whitespace.
- `JSON.buf_parse` skips leading JSON whitespace and parses one value, leaving
  following bytes, including trailing whitespace, unread. It saves the input
  before skipping whitespace, stages the result in a temporary tag, and
  restores the position and preserves the destination on failure. Dispatch
  now checks byte-peek success before using the result, including at EOF.
  Existing streaming prefix behavior is retained: `trueXYZ` reads `true`
  and leaves `XYZ` for the caller.
- `JSON.from_str` requires one value with only JSON whitespace before and
  after it. Any remaining non-whitespace byte rejects the document. Rejection
  cleans the temporary tag, including when a successfully allocated value
  is followed by an invalid suffix.
- Arrays and objects handle empty containers separately, then require a
  closing delimiter or a comma after each value/member. A comma requires
  another value/member, rejecting missing, repeated, and trailing commas.
  Object keys still require a colon before their values. These transitions
  follow [RFC 8259 §4](https://www.rfc-editor.org/rfc/rfc8259.html#section-4)
  and [§5](https://www.rfc-editor.org/rfc/rfc8259.html#section-5).
- Existing string-decoder diagnostics and fixtures are preserved. No core
  KC3 parser, numeric conversion, or serialization code was changed. Numbers
  still use the existing KC3 numeric parser, so this fix does not establish
  complete JSON number-grammar compliance. The F04 serialization limitation
  also remains. At the user's request, the newly added header comments were
  removed; parser contracts are documented here instead.

**Regression coverage:** added `test/json/framing.kc3` and
`test/json/framing_invalid.kc3`, with expected output/return fixtures, to the
existing JSON harness. Together they contain 73 result checks: 22 covering
complete documents, streaming results, consecutive values, and failure
rollback; plus 51 complete-document rejection cases. Coverage includes all
four JSON whitespace bytes, empty/nested containers, unread suffixes,
missing documents, multiple values, non-JSON whitespace, malformed UTF-8
whitespace, comments, NUL, missing separators/colons, trailing/repeated commas,
truncation, and nested failures. Streaming tests start at a nonzero buffer
position and verify the exact remaining bytes. Expected rejection diagnostics
distinguish failure from successful parsing of `null`.

**Fix validation:**

- Before changing production code, both new fixtures failed, demonstrating
  the document-boundary, whitespace, and separator defects. Baseline outputs
  and diffs are retained in the log directory below.
- `. ./env && make test_json` and `. ./env && make test_json_debug` both
  passed all eight JSON fixtures, including the 73 new checks and all F04
  string regressions. The final header-only change removes comments and has
  no runtime effect.
- `. ./env && make test` passed with exit status 0: 424 reported tests across
  all 12 suites, including 300 core C tests and all HTTP/TLS integration
  tests. This total includes the baseline's additional SHA-512 HMAC fixture,
  which is not an F05 change. The root target restored release library links.
- `git diff --check` passed. No full debug integration run, sanitizer run,
  fuzzing campaign, allocation-failure injection, or network exploit test
  was performed for F05. The new JSON fixtures use in-memory input buffers.

Logs are in `/tmp/kc3-f05.bJSp7SEs/`: `before.log`,
`before-framing.out`, `before-framing.diff`, `before-invalid.out`,
`before-invalid.diff`, `after-release.log`, `after-debug.log`, and
`full-release.log`. The original `test/kc3.dump` was preserved and restored;
the newly generated dump is retained in the same directory.

### F06 — Medium: chunked HTTP response parsing leaks one allocation per chunk

**Locations:** `http/http_response.c:181–189`,
`libkc3/list_init.c:1939–1954`, `libkc3/tag_init.c:628–637`,
`libkc3/str.c:459–469`, `libkc3/buf.c:882–889`.

For each nonempty chunk, `http_response_buf_parse` creates a list node with
`list_new_str_alloc(chunk_size, NULL)`, allocating storage for the chunk.
It then passes that initialized string to `buf_read`.

`buf_read` independently allocates another string and assigns it to `*dest`.
It does not release the previous destination allocation. After a successful
read, the first allocation is unreachable; deleting the chunk list frees only
the replacement allocation.

**Impact:** every successfully read nonempty chunk leaks memory, including in
otherwise valid responses. Repeated chunked responses can grow the memory
usage of a long-running HTTP/HTTPS client until it exhausts available memory.
For ordinary chunk sizes, the leaked allocation is `chunk_size + 1` bytes.

**Suggested fix:** initialize an empty string in each chunk node and let
`buf_read` allocate it once, or use a read operation that fills the already
allocated storage. Preserve cleanup behavior on failed reads.

**Original validation:** allocation-ownership trace through the producer,
reader, and list cleanup; no leak checker was run during source review.

**Remediation — implemented, awaiting review (2026-10-10):**

- Started from `78a813ff3` with no tracked source changes. Replaced
  `list_new_str_alloc(chunk_size, NULL)` with `list_new_str_empty(NULL)`.
  The node starts with a valid, non-owning empty string; `buf_read` now
  performs the only allocation for that node's chunk bytes.
- If the body read fails, the empty node is safe to delete. If a later read
  or delimiter check fails, existing list cleanup frees the successfully
  read chunks. Successful concatenation and response cleanup are unchanged.
- No core allocation/buffer code, header comments, chunk-size grammar,
  response framing policy, or other findings were changed.

**Regression coverage:** added `test/http/17_response_chunked.kc3` and its
expected output/return fixtures to the existing HTTP harness. Its 16 cases
cover empty, single, and multiple chunks; embedded NUL and CRLF body bytes;
hexadecimal sizes; exact unread suffixes; header-only parsing; consecutive
responses; short first/second chunks; missing/wrong chunk CRLF; missing zero
chunk; and missing/wrong final CRLF. Failure cases verify rollback from a
nonzero starting position. These are behavioral regressions: the finalized
fixture passes before and after the leak fix, and does not itself count
allocations.

**Scoped allocation validation:**

- A temporary native probe uses KC3's existing `memleak` tracker, enabled only
  around buffer construction, `http_response_buf_parse`, response cleanup,
  and buffer cleanup. Interpreter initialization/teardown are outside that
  window. It exercises 13 scenarios 32 times each (416 checks), checks the
  returned body or failure rollback/destination preservation, and requires
  no outstanding tracked allocations after each cleanup.
- Linked against the original release response object, the probe failed
  320 of 416 checks and counted 480 outstanding allocations totaling 2,272
  bytes. The empty response, header-only response, and short first chunk
  controls had none. Each successful nonempty chunk read lost exactly its
  original `chunk_size + 1` allocation, including when parsing later failed.
- Linked against the rebuilt release object and then the rebuilt debug
  object, the same probe passed 416/416 each time with zero outstanding
  allocations/bytes. The assertion is local to these measured parser paths,
  not a claim that the entire interpreter is leak-free.
- An earlier interpreter-wide `KC3_MEMLEAK=1` control using the existing
  `05_response` fixture reported one 16-byte allocation without exercising
  chunk parsing. Its cause was not investigated in F06; it motivated the
  scoped measurement above. The probe and allocation assertions remain
  under `/tmp`, not in the default test target.

**Fix validation:**

- `. ./env && make test_http` and `. ./env && make test_http_debug` passed
  all 17 HTTP fixtures, including the 16 new behavioral cases.
- `. ./env && make test` passed with exit status 0: 425 reported tests across
  all 12 suites, including 300 core C tests and all HTTP/TLS integration
  tests. The root target restored release library links after debug testing.
- `git diff --check` passed. The configured build reports ASan unavailable;
  Valgrind is not installed. No full debug integration run, sanitizer run,
  allocation-failure injection, or network exploit test was performed.
- During fixture development, a ten-byte chunk with size `a` was accepted
  but size `A` was rejected by the baseline hexadecimal reader. This separate
  numeric parsing limitation is unchanged and outside F06; the finalized
  fixture does not claim uppercase chunk-size support.

Artifacts are in `/tmp/kc3-f06.38DvPmJR/`: `chunk_leak_probe.c`,
`chunk_leak_probe_before`, `chunk_leak_probe_after`, `chunk_leak_probe_debug`,
`before-leak.log`, `after-leak-release.log`, `after-leak-debug.log`,
`memleak-control.log`, `before-http.log` (initial uppercase-size failure),
`before-functional.log`, `after-http-release.log`, `after-http-debug.log`,
and `full-release.log`. The original `test/kc3.dump` was preserved and restored;
the newly generated dump is retained in the same directory.

### F07 — High: unchecked container allocation sizes reach the replication authentication parser

**Locations:** `libkc3/ht.c:172–181`, `libkc3/ht.c:66–75`,
`libkc3/marshall_read.c:1455–1468`,
`libkc3/facts_connection.c:80–100`, `libkc3/map.c:140–154`,
`libkc3/tuple.c:73–84`.

Several constructors calculate `count * sizeof(element)` without checking
overflow. `alloc` receives the already-wrapped byte count and calls
`calloc(1, size)` (`libkc3/alloc.c:23–33`), so the allocator cannot recover
or check the original multiplication. The objects retain the original count
and subsequently iterate or index using it.

The hash-table case is reachable from the serialization header:

1. `marshall_read_header` copies the wire `heap_count` into the reader and
   passes it to `ht_init` without a maximum or a multiplication check.
2. On a 64-bit host with eight-byte pointers, a count of
   `2305843009213693953` (`2^61 + 1`) makes the allocation size wrap to eight
   bytes, while `ht->size` remains `2^61 + 1`.
3. `ht_clean` walks that claimed number of buckets. It reads beyond the
   allocation and may treat unrelated memory as list pointers. Lookup and
   insertion also index by the unvalidated logical size.

**Exposure:** the server calls `marshall_read_header` while reading the HMAC
response, before comparing it with the expected HMAC. A header with the count
above and zero heap/body sizes passes the existing heap-count consistency
check. A subsequent failed authentication read reaches
`facts_connection_delete` -> `marshall_read_clean` -> `marshall_read_ht_clean`
-> `ht_clean` (`facts_connection.c:166–169,227–236`;
`marshall_read.c:627,1540–1544`). Thus the unsafe size and cleanup are reachable
before proof of the facts shared secret. Establishing the TLS connection is
still a prerequisite; deployment-specific TLS access restrictions were not
assessed.

The same allocation pattern affects maps and tuples loaded from serialized
counts (`marshall_read.c:1851–1858,3305–3313`). `tuple_init` starts writing tag
initializers across its logical count immediately, before any serialized
tuple elements need to be read.

**Impact:** out-of-bounds access, invalid cleanup, and potentially heap
corruption from small malformed serialized inputs. This is separate from F02:
the binary count can be decoded correctly and still overflow during allocation.

**Suggested fix:** reject counts above `UW_MAX / sizeof(element)` in each
constructor before multiplying; impose practical per-message allocation limits;
and constrain the pre-authentication message to the small fixed-size challenge
response actually needed. Keep partially initialized objects safe to clean.

**Original validation:** source trace and unsigned arithmetic only. No malformed
replication message was sent and no network exploit was executed during review.

**Remediation — present in baseline `b5a995142` (implemented 2026-10-10):**

- Started from `8f6e72d66` with no tracked source changes. `ht_init`,
  `map_init`, and `tuple_init` reject counts exceeding
  `UW_MAX / sizeof(element)` before multiplying or allocating. Hash tables
  reject zero buckets and tuples reject counts below two in release as well
  as debug builds; empty maps remain valid.
- Hash-table and tuple backing arrays are allocated before publishing their
  counts/pointers to the destination. Rejected counts leave an existing
  destination unchanged, and zero-initialized destinations remain safe to
  clean. Locks are initialized in their final storage, not copied from a
  temporary object. The global allocator and its failure policy are unchanged.
- `marshall_read_header` delegates to the new
  `marshall_read_header_limit(mr, heap_count_max, heap_size_max, buf_size_max)`.
  Header fields are decoded into `u64` and checked before narrowing to `uw`:
  bucket multiplication must fit, offsets and combined extents must fit the
  signed buffer-size range, and heap counts must be consistent with heap
  bytes. Caller-supplied limits are applied before table allocation.
- Invalid headers return failure without replacing the existing reader
  metadata/cache. A complete rejected header is consumed, as before; this is
  not a rewind contract. The temporary header string is freed on every
  completed read, including validation failures. Input-validation assertions
  in this header function were removed so malformed authentication headers
  can return failure in debug builds too.
- Both authentication directions require zero heap count, heap bytes, and
  heap offset. They require exactly 32 challenge bytes or 64 SHA-512 HMAC
  bytes in the existing typed-byte encoding: respectively 256 or 512 body
  bytes, plus the 40-byte header. The bounded header reader runs before any
  table allocation; the authentication helper reads the fixed sequence
  directly, without calling the peer-sized chunk allocator. Wrong byte
  markers and truncated bodies return failure. Valid wire format, HMAC
  calculation/comparison, and subsequent replication messages are unchanged.
- No arbitrary global dump-size cap was introduced. General deserialization
  and authenticated replication retain their representable size range;
  caller-specific resource quotas, nesting limits, and protection against
  all large-but-non-overflowing allocations remain additional hardening.
  F08 and F09 are not resolved. No header comments were added.

**Regression coverage:** added three always-enabled core test cases in
`test/marshall_read_test.c`:

- `marshall_read_container_limits`: checks the overflow boundary, a small
  wrapped-allocation count, and `UW_MAX` for all three constructors; also
  verifies invalid minimum counts, preservation of live destinations,
  cleanup after rejection on zero-initialized destinations, and empty maps.
- `marshall_read_container_counts`: exercises oversized map/tuple counts
  in both heap and body buffers, plus invalid tuple counts zero and one.
  A real encoded value follows each count; rejection must occur before
  consuming it. Sixteen reader rejections are checked.
- `marshall_read_header_limits`: checks 20 header layouts/limit combinations
  and bad-magic/truncated headers. Coverage includes unsigned wire extremes,
  bucket overflow, combined-size/offset overflow, inconsistent counts,
  individual caller limits, valid empty/boundary headers, and preservation
  of an already allocated cache on validation failure.

**Fix validation:**

- A temporary native baseline probe observed all three tested overflowing
  inputs accepted: a table count of `2305843009213693953` with an eight-byte
  wrapped allocation, a map count of `128102389400760777` with 272-byte
  wrapped arrays, and the forged empty-payload header from the original
  finding. Before cleanup, the probe reset the invalid logical counts to
  zero so it did not walk beyond those allocations. It did not execute the
  unsafe tuple-initialization loop or send a malformed network message.
  After rebuilding, the same probe rejected all three inputs.
- Rebuilt release/debug core libraries and test binaries. With
  `KC3_TEST='marshall_read tuple'`, the focused binaries passed 56/56 release
  and 51/51 debug reported tests. The difference is the pre-existing
  release-only negative tests; all three F07 cases run in both builds.
  Initial positional-argument runs selected only the four tuple tests;
  the environment-based selection above includes the new marshalling tests.
- A separate native probe includes the actual `facts_connection.c` source
  to call its private authentication reader, without making that helper a
  public API. It exercises 30 in-memory cases across both authentication
  sizes: valid frames, overflowing/nonzero heap counts, nonzero heap bytes,
  oversized/undersized body lengths, nonzero/extreme offsets, malformed
  headers/byte markers, and truncation. Valid frames preserve the next bytes;
  invalid headers are rejected before body reads or table allocation.
  All 30 cases passed in release and debug with zero outstanding allocations
  in the scoped KC3 allocation tracker. These temporary probes are separate
  from the default test target and are not live network exploit tests.
- `. ./env && make test` passed with exit status 0: 428 reported tests across
  all 12 suites, including 303 core C tests and normal TLS facts replication.
  `. ./env && make test_kc3s_debug` also passed all seven fixtures, including
  the three TLS facts-replication fixtures. Release library links were
  restored afterward with `. ./env && make lib_links`.
- `git diff --check` passed. No sanitizer run, 32-bit execution,
  allocation-failure injection, fuzzing campaign, or full debug integration
  run was performed. Rejection cleanup was exercised; general allocator
  exhaustion and all malformed serialized object types were not.

Artifacts are in `/tmp/kc3-f07.ipzZBKLo/`: `count_probe.c`, `count_probe`,
`before-count.log`, `after-count.log`, `build-release.log`, `build-debug.log`,
`tuple-release.log`, `tuple-debug.log`, `focused-release.log`,
`focused-debug.log`, `auth_probe.c`, `auth_probe_release`, `auth_probe_debug`,
`auth-release.log`, `auth-debug.log`, `full-release.log`, and
`kc3s-debug.log`. The original `test/kc3.dump` was preserved and restored;
the newly generated dump is retained in the same directory.

### F08 — High: serialized struct layouts permit writes outside the data allocation

**Locations:** `libkc3/marshall_read.c:3013–3045`,
`libkc3/marshall_read.c:3061–3063`,
`libkc3/marshall_read.c:2978–2995`.

The `_KC3STRUCTTYPE1_` reader accepts field offsets, alignment, and total
struct size directly from the stream. It does not recompute the layout or
check each field's end against the declared size. `marshall_read_struct`
then allocates that size and decodes each field at `data + offset`.

For example, a serialized type can declare one `U64` field at offset 16 and a
total size of 8. With `has_data` set, decoding allocates eight bytes and
performs the field's eight-byte write starting sixteen bytes from the base.
No arithmetic overflow is needed. Both the offset and the value written come
from the serialized input.

**Impact:** out-of-bounds typed writes when malformed struct values are loaded.
The cleanup path can also revisit these offsets for types that require cleanup
(`libkc3/struct.c:171–176`). This needs a separate fix from the array-stride
issue in F01.

**Suggested fix:** validate the field schema and derive a canonical layout,
or verify every offset, alignment, and field extent with checked arithmetic
before allocating or writing. For decoding into `dest->data`, also ensure
the incoming type matches the destination's actual layout and capacity.

**Original validation:** source trace of serialized layout metadata through allocation
and typed writes. No malformed struct was executed.

**Remediation — implemented, awaiting review (2026-10-11):**

- Before allocating struct data, validate field names and resolvable field
  types, reject null variable/struct defaults, and obtain native field sizes
  and alignments. Unknown types are rejected without calling the type helpers'
  assertion/abort paths. Already decoded nested struct defaults supply their
  layouts when their modules have not yet been registered during dump restore.
- `_KC3STRUCTTYPE1_` requires ordered, non-overlapping, aligned field extents
  within the declared size, a nonzero power-of-two alignment at least as large
  as each field's alignment, and a representable, aligned total size. Padding
  remains allowed. Extent checks use subtraction to avoid overflow.
- The older `_KC3STRUCTTYPE_` format derives offsets and final padding with
  checked arithmetic instead of ignoring `struct_type_update_map` failure.
  Both formats derive `must_clean` from the field types rather than trusting
  the wire flag. Empty types remain supported.
- Existing `dest->data` requires the same module, size, alignment, ordered
  field names, field types and offsets as the caller's declared native type.
  Inline destinations require a data-backed payload. Decoding uses temporary
  storage and copies it to the destination only on success, preserving its
  wrapper, ownership and bytes on rejection. The caller remains responsible
  for providing storage of its declared native type's size.
- `marshall_read_data` propagates nested struct failures instead of reporting
  success. Rejection cleans completed fields individually, without invoking
  an incomplete struct's custom cleanup callback. Rejected type construction
  frees the outer allocation without deleting an uninitialized refcount/mutex.
- Struct types retained by the reader use its existing owned-tag list. This
  keeps repeated type references alive after temporary inline wrappers are
  released; it works in reference and copy modes. `struct_type_init_copy`
  now retains `align_max`. This limited lifetime handling is needed by F08;
  it does **not** add cache-kind checks or close F09.
- No header or wire-format changes were made. No commits were made.

**Regression coverage:** five always-enabled C cases cover 17 malformed
layouts/schemas, nine destination/type rejections with byte-for-byte canary
checks, four nested success/failure combinations, and three legacy layout
arithmetic overflows. Valid cases cover mixed-width padding, owned string
fields, data/tag/both/unallocated storage, both type wire formats, empty
types, and repeated inline references in both reference and copy modes.
Nested rejection occurs after a preceding string has already been decoded,
exercising partial-read cleanup and failure propagation.

**Validation performed:**

- Before the production change, the metadata-only regression failed because
  the baseline reader accepted an out-of-bounds layout. It did not decode
  struct data or deliberately execute the out-of-bounds write.
- Rebuilt release/debug core libraries and test binaries with `. ./env`
  sourced. Focused `KC3_TEST='marshall_read marshall tuple'` runs from `test/`
  passed: **84/84 release**, **77/77 debug**.
- A native probe warmed the five regression groups, then repeated each ten
  times with KC3 allocation tracking scoped around each run. Both release
  and debug reported **50 runs, zero failures, zero outstanding allocations**.
- `. ./env && make test` passed all **433/433** reported tests across 12
  suites, including **308/308** core C tests and dump/restore coverage.
- `. ./env && make test_kc3s_debug` passed **7/7**, including replication
  fixtures. Release library links and the original `test/kc3.dump` were
  restored afterward; the dump was checked with `cmp`.
- An initial focused invocation from the repository root crashed in the
  existing writer fixture's `test_file_compare`: its expected file is
  relative to `test/`, and the helper closes a null file handle on the error
  path. Running from `test/` passed. That unrelated helper was not changed.
- `git diff --check` passed. No sanitizer, 32-bit, fuzzing, allocation-failure
  injection, or full debug integration run was performed. These checks do
  not validate all malformed field payloads, serialized callables, or shared
  object graphs; F09 remains open.

Artifacts are in `/tmp/kc3-f08.z5SzYUDY/`: baseline/focused/full-release logs,
build logs, `kc3s-debug.log`, the native `allocation_probe.c` and release/debug
binaries and logs, wrong-working-directory logs, and the original/generated
test dumps.

### F09 — High: shared-object deserialization cache does not retain or check object types

**Locations:** `libkc3/marshall_read.c:1480–1537`,
`libkc3/marshall.c:1420–1444`,
`libkc3/marshall_read.c:2685–2708`,
`libkc3/marshall_read.c:3332–3342`.

`marshall_read_ht_add` stores only `{global_offset, raw_pointer}`. Its compare
and hash functions use the offset alone. `marshall_read_heap_pointer` has no
expected-type argument and returns the cached address without type validation.

A stream can first decode a symbol at a valid offset, then encode a tuple
reference to that same offset. The tuple reader validates the outer
`_KC3PTUPLE_` marker, finds the cached symbol pointer, and calls
`tuple_new_ref` on it without reading or validating a tuple body.

`s_sym` contains only an `s_str` (`libkc3/types.h:788–790`), whereas
`tuple_new_ref` accesses a tuple mutex and reference count
(`libkc3/tuple.c:179–195`). Those fields do not exist in the cached symbol.
The struct and callable readers have equivalent unchecked cache-hit paths.

**Impact:** type confusion, invalid mutex operations, out-of-bounds accesses,
or reference-count corruption while reconstructing a malformed object graph.
Validating object bodies only on cache misses does not protect this path.

**Suggested fix:** record the decoded object kind and, where needed, concrete
type alongside every cached pointer; require each reader to provide its
expected type and reject incompatible references before copying or increasing
reference counts. Validate offset ranges and object construction state too.

**Original validation:** source trace of a symbol cache insertion followed by a tuple
cache hit. No crafted object graph was run. As with F01, this does not by
itself establish access to tag decoding before replication authentication.

**Remediation — implemented, awaiting review (2026-10-12):**

- Reader cache entries now store `{global_offset, raw_pointer, object_kind}`.
  Every implemented cached pointer reader supplies its expected kind, checked
  before returning an address to copy/reference-count code. Struct wrappers,
  struct types, symbols, tuples, calls, callables, complex values, facts,
  frames, lists, set items, skiplists/nodes, tags, and variables are distinct.
- Both typed-pointer wire formats use the complete pointer type, so a cached
  `Mutex*` cannot satisfy a request for `U8*`. Custom pointees use negative
  cache keys, separate from positive native-object keys: the writer may put
  a custom pointee and its native reader callable at the same wire offset.
  Completed custom pointees are now cached rather than reconstructed on each
  repeated reference.
- Nonzero offsets must be outside the header and within the current heap;
  signed conversion and global-offset addition are checked before seeking or
  hashing. Custom hooks may use the heap-end position because their payload
  can be empty. Null wire references remain valid.
- A cache miss reserves a typed entry with a zero pointer. Completion must
  match that reservation and may happen only once. References to objects
  still being constructed are rejected; skiplist readers no longer publish
  partially initialized allocations. A pointer-read failure disables further
  cache lookups until the next valid header, preventing reuse of entries
  invalidated by failure cleanup.
- Tuple cache hits now use `ptuple_init_copy`, matching the other owning
  pointer readers. The new valid-reference tests exposed the old unconditional
  `tuple_new_ref` path: copy-mode cleanup asserted on the resulting shared
  tuple. The corrected path passes both copy and reference modes.
- No wire-format change or header comments were introduced. This is not a
  general redesign of decoded-object ownership: callers must still respect
  existing object lifetimes. It does not validate every malformed nested
  payload, serialized callable, or custom marshalling hook.

**Regression coverage:** eight always-enabled groups in
`test/marshall_read_test.c`. They cover a cached symbol presented to 15
incompatible native readers, all 324 pairs of 18 native/typed-pointer kinds,
incomplete and duplicate completion, null completion, poisoned-cache reuse,
offset boundaries and arithmetic overflow, frame and skiplist self-cycles,
valid repeated tuple/list/symbol references in both modes, and repeated
current/legacy `Mutex*` pointers. A forged legacy `U8*` reference to the
cached mutex is rejected without changing the destination.

**Validation performed:**

- A baseline probe on `957c3febe` reported
  `tuple reference resolved to cached symbol: yes`. It stopped at the cache
  lookup and did not execute invalid tuple mutex/reference-count accesses.
- Rebuilt release/debug core libraries and test binaries with `. ./env`.
  Focused `KC3_TEST='marshall_read marshall tuple'` runs from `test/` passed
  **104/104 release** and **97/97 debug**.
- Scoped KC3 allocation tracking, after warming the eight regression groups,
  reported **80 runs, zero failures, zero outstanding allocations** in each
  of the release and debug probes.
- Final `. ./env && make test` passed **453/453** reported tests across all
  12 suites, including **328/328** core C tests and dump/restore coverage.
- `. ./env && make test_kc3s_debug` passed **7/7**, including replication
  fixtures. Release library links and the original `test/kc3.dump` were
  restored afterward; the dump was checked with `cmp`.
- `git diff --check` passed. No sanitizer, 32-bit, fuzzing, allocation-failure
  injection, or full debug integration run was performed.

Artifacts are in `/tmp/kc3-f09.KeQvtoLV/`: the baseline probe and `before.log`,
build/focused/full-release/debug-replication logs, scoped allocation probe
source and release/debug binaries and logs, and original/generated test dumps.

### F10 — Medium: local bindings override explicitly qualified identifiers

**Locations:** `libkc3/env_eval.c:916–934`,
`libkc3/env_eval.c:954–968`, `libkc3/env.c:1808–1814`.

`env_eval_ident` searches local/global frames using `ident->sym` before
checking whether `ident->module` is set. If it finds a binding, it returns
that value immediately and never resolves the explicitly requested module.
`env_eval_ident_is_bound` has the same omission.

For example, given a loaded definition `Const.a = 1` and a current-frame
binding `a = 2`, evaluating an identifier with module `Const` and symbol `a`
returns 2. The source parser preserves those as separate identifier fields
(`libkc3/buf_parse.c:2167–2179`), so the qualifier is available but ignored.

**Impact:** module-qualified constants and function values can change meaning
because of an unrelated local name. Boundness checks can also report that a
qualified identifier exists merely because a local variable shares its name.
This disagrees with function-call resolution, which explicitly requires
`tmp.ident.module == NULL` before consulting frames
(`libkc3/env_eval.c:719–721`).

**Suggested fix:** restrict frame lookup to unqualified identifiers in both
value evaluation and boundness checks, matching the call-resolution rule.
Cover a local variable shadowing an explicitly qualified constant and a
module-qualified function used as a value.

**Original validation:** source-only comparison of identifier and call resolution.
The example was not executed.

**Remediation — implemented, awaiting review (2026-10-12):**

- `env_eval_ident` and `env_eval_ident_is_bound` now consult `env_frames_get`
  only when `ident->module == NULL`, matching the existing frame-lookup rule
  in `env_eval_call_resolve`. Explicitly qualified names continue through
  module lookup, even when a local or global-frame binding has the same name.
- Unqualified lookup retains its existing precedence: the nearest local
  frame, enclosing frames, the global frame, then module resolution. Missing
  qualified names return failure/false rather than borrowing a frame value.
- Added three always-enabled C regression groups in `test/env_test.c`:
  constant lookup under global/enclosing/local shadows in copy and reference
  modes; qualified functions used as values and direct calls under local and
  global shadows; and missing qualified names with matching frame bindings.
  Controls exercise unqualified lookup, module fallback after removing frame
  bindings, and an unchanged destination on failed value lookup.
- Extended `test/ikc3/defmodule.kc3` and its expected output with local and
  top-level constant shadows, non-callable and callable shadows of a module
  function, and calling the qualified function after assigning it to a local
  variable. Production changes are limited to the two lookup guards; no
  headers or unrelated resolver paths were changed.

**Validation performed:**

- Before changing production code, a native runner executed each new C group
  separately against `482153eeb`. All three failed at the intended checks:
  the qualified constant returned the frame value, the qualified function
  returned **99 instead of 42**, and a missing qualified identifier reported
  bound. All three pass after the fix.
- Rebuilt release/debug libraries and core tests with `. ./env` sourced.
  `KC3_TEST='env ident'` from `test/` passed **13/13 release** and
  **13/13 debug**, including environment dump/restore.
- `. ./env && IKC3_TEST='defmodule ident let fn call' make test_ikc3_debug`
  passed **5/5**, including the extended module fixture.
- `. ./env && make test` passed **456/456** reported tests across all 12
  suites, including **331/331** core tests and **71/71** interpreter fixtures.
- An initial test setup left copy mode enabled during environment teardown
  and crashed in operator cleanup. Restoring the saved mode before teardown
  fixed the test setup. The interpreter fixture also needed KC3's pin syntax
  (`f = ^ Double.double`) when assigning an identifier's value. Neither
  adjustment required expanding the production patch.
- Restored release library links and the original `test/kc3.dump`, checked
  with `cmp`. `git diff --check` passed. No sanitizer, fuzzing, allocation
  tracking, or full debug integration run was performed for F10.

Artifacts are in `/tmp/kc3-f10.PE4h9YTH/`: the native regression runner and
before/after logs, build/focused/interpreter-debug/full-release logs, the
test-setup backtrace, and original/generated test dumps.

### F11 — Medium: multipart file uploads flush each payload byte separately

**Locations:** `http/http_request.c:518–520`,
`libkc3/buf.c:1288–1327`, `libkc3/buf.c:1337–1368`,
`libkc3/buf_fd.c:185–213,237–248`.

The multipart upload path writes files using `buf_read_until_str_into_file`.
Its byte-copy loop calls `buf_flush(dest)` before every `buf_write_u8`.
For a file-backed destination, that flush writes the preceding byte and resets
the buffer's write position, preventing data from accumulating into a block.

The file-descriptor flush first attempts `send`, then falls back to `write` on
failure. For a regular upload file, each nonempty flush therefore attempts a
socket send on a file descriptor and then performs a one-byte write. The final
byte is flushed on close. A successful N-byte upload results in approximately
N such flushes, rather than one write per buffer-sized block.

**Impact:** avoidable syscall and CPU cost for ordinary uploads, with greater
resource-exhaustion risk under concurrent requests. The 64 MiB request-body
limit still permits tens of millions of these operations for one request.
No throughput measurement or load test was performed.

**Suggested fix:** accumulate bytes in a reasonably sized output buffer and
flush only when necessary and at completion, retaining enough input lookahead
to recognize the multipart boundary. In the same loop, correct
`r = buf_flush(dest) <= 0` so an error is not converted to the positive value
1 before return; preserve the actual flush result and error status.

**Original validation:** source trace through the multipart caller, copy loop, and
file-descriptor flush implementation only.

**Remediation — implemented, awaiting review (2026-10-12):**

- `buf_read_until_str_into_buf` flushes only when the destination is full,
  before consuming the next payload byte. Delimiter recognition still happens
  first, so an exact-fit destination succeeds without an unnecessary flush.
  Negative refill/flush errors are preserved; a zero-progress flush or one
  that leaves no writable space returns failure instead of positive success.
- `buf_read_until_str_into_file` now allocates `BUF_SIZE` (64 KiB), independent
  of delimiter length. It explicitly checks the final flush and descriptor
  close, and disables automatic flushing before cleanup so a failed write is
  not retried or reported as success. Earlier successful writes are not rolled
  back on failure; this is not an atomic-file-write change.
- Both helpers reject empty delimiters and delimiter lengths incompatible
  with signed refill sizes or the input buffer's capacity. The file helper
  checks these before opening/truncating the output file.
- Input lookahead and bytewise boundary matching are retained. Only payload
  bytes reach the destination; the delimiter is consumed and following bytes
  remain available. The return value still counts payload plus delimiter,
  including its existing use by HTTP upload metadata. The generic buffer
  helper retains buffered output at completion; the file wrapper drains it.
  No HTTP parser, wire format, public signature, or header was changed.

**Regression coverage:** six always-enabled groups in `test/buf_test.c` and
`test/buf_file_test.c` cover empty/binary payloads, partial delimiter prefixes,
exact-fit/prefilled/too-small destinations, invalid and truncated delimiters,
trailing input, callback failures and flush counts. Twenty fragmented-input
cases exercise boundary recognition across refills with input capacities
4–8 and fragments of 1–4 bytes. File checks verify exact contents and lengths
for payloads of 0, 1, 65536, 65537, and 131079 bytes. Child processes with a
zero file-size limit exercise real file-write failures both during copying
and at the final flush, without changing the parent's resource limits.

**Validation performed:**

- Baseline probes reproduced a flush callback on every payload byte
  (**257 calls**, versus **16 during copying** with a 16-byte destination
  after the fix) and a flush error of **-7 reported as positive 1**. One
  remaining byte is explicitly drained after the fixed helper returns,
  making **17 total flushes** for that regression.
- A probe linking the two original helpers extracted from `f25e6223a`
  also confirmed that a final file-write failure was reported as success.
  All three baseline probes fail their intended assertions; the fixed
  regressions pass. The resource-limit harness redirects child stderr to
  `/dev/null` before applying the limit so error logging to a regular test
  log does not itself hit the limit. Initial test setup also needed to avoid
  a comparison macro's loop-variable shadowing and to check empty files by
  `stat` rather than the existing nonempty-only `file_read_all` behavior.
  These were test-harness corrections, not additional production changes.
- Rebuilt release/debug core libraries and test binaries with `. ./env`.
  `KC3_TEST='buf buf_file'` from `test/` passed **44/44 release** and
  **44/44 debug**. `. ./env && make test_http_debug` passed **17/17**.
- Scoped KC3 allocation tracking, after warming the six new groups, reported
  **60 runs, zero failures, zero outstanding allocations** in each of the
  release and debug probes.
- A retained native release probe parsed real multipart requests containing
  1-byte and 131079-byte binary uploads. Both produced exact file contents
  and lengths and preserved the following pipelined `GET /next` request.
- `. ./env && make test` passed **462/462** reported tests across all 12
  suites, including **337/337** core C tests and **17/17** HTTP fixtures.
- Restored release library links and the original `test/kc3.dump`, checked
  with `cmp`. `git diff --check` passed. No throughput/load benchmark,
  sanitizer, 32-bit, fuzzing, or full debug integration run was performed.
  The bytewise boundary matcher and broader multipart behavior were not
  redesigned or exhaustively audited.

Artifacts are in `/tmp/kc3-f11.3Bg3liXB/`: baseline helper/runner sources and
failure logs, build/focused/HTTP-debug/full-release logs, scoped allocation
and multipart probe sources/binaries/logs, and original/generated test dumps.

## Investigation log

1. Confirmed the initial Git worktree is clean and reviewed the project overview
   and test entry points. No applicable `AGENTS.md` was found in the repository
   or its parent directories.
2. Inspected README, build configuration, test entry points, native parsing
   helpers, HTTP handling, JSON handling, array allocation, and deserialization.
3. Before the source-only instruction, two selected runs of the existing
   `test/libkc3_test` binary reported 59/59 and 50/50 passing test cases.
   These covered selected primitive, buffer, array, string, parser, inspector,
   and environment groups; they were not the full suite and do not validate
   the findings above. The generated configuration reports ASan unavailable.
4. Before the source-only instruction, a temporary C probe was created and
   compiled under `/tmp/kc3-audit.Q0cfXuJc`. Its observed runs failed during
   module-path initialization. The subsequent invocation was interrupted;
   no successful reproduction is claimed. A process check after the user's
   instruction found no remaining audit probe or unit-test process.
5. Continued with source inspection only, traced caller contracts and cleanup
   paths, and documented six findings. In particular, JSON call construction
   is distinguished from immediate execution, and HTTP framing risk is
   distinguished from a demonstrated proxy exploit.
6. The untracked `kc3.dump` from the earlier environment test was moved to
   `/tmp/kc3-audit.Q0cfXuJc/baseline-test-kc3.dump`, preserving it outside the
   repository. It was not deleted.
7. Resumed at the user's request with source inspection only. Traced container
   allocation sizes, replication authentication and cleanup, serialized struct
   layouts, shared-object type handling, qualified identifier lookup, and
   multipart file writes. Added F07–F11. No builds, tests, or probes were run
   during this continuation.
8. Started remediation at the user's request, restricted to F01 for the first
   reviewable step. Read the testing/structure guides, array documentation,
   array KC3 module, constructors, writer, reader, and test harness. Added the
   layout checks and regression tests without committing or changing F02–F11.
9. The first new round-trip test used `TAG_TEST_EQ` on a tag-backed array.
   That reached `compare_array`, which calls `memcmp` on the null data pointers,
   and crashed. The core backtrace confirmed the comparison as the cause. The
   test now compares metadata and inspected values; production comparison code
   remains unchanged. The crash regenerated the ignored
   `test/libkc3_test.core`, replacing its prior diagnostic contents. Subsequent
   test runs disabled core dumps. Final test outcomes are recorded under F01.
10. Preserved the pre-existing `test/kc3.dump` outside the repository before
    running the Makefile targets that remove it, then restored the original
    file after testing. Build outputs and test logs are not source changes.
11. On 2026-10-09, ran the user's exact `. ./env && make test` command.
    Recorded the passing core/interpreter suites and the three TLS-fixture
    permission failures under F01. That run wrote failure diffs to
    `test/kc3s/facts_acceptor_loop.diff`, `facts_connection.diff`, and
    `facts_secret.diff`; subsequent runs may overwrite those artifacts.
    No additional code fixes or commits were made.
12. Repeated the same command twice more at the user's request on 2026-10-09.
    The first repeat reproduced the TLS-key permission failures and exited 2.
    The latest repeat passed every suite (415/415 reported tests) and exited 0.
    Updated this log without implementing another finding or making a commit.
13. Implemented F02 at the user's request on 2026-10-09, starting from
    `180f93a01`. Added always-enabled parser regressions, observed the expected
    failures before the fix, corrected the template, and regenerated its five
    unsigned parsers. Focused release/debug tests and the full release target
    passed. Restored the original test dump and left all F02 changes uncommitted
    for review. F03–F11 were not modified.
14. Implemented F03 at the user's request on 2026-10-09, starting from
    `66dda63ab`. Added the HTTP regression fixture, reproduced the old parser's
    incorrect acceptance of malformed lengths, and installed strict checked
    decimal parsing. The focused release fixture, complete HTTP debug suite,
    and full release target passed. Updated F03's completion status and left
    the patch uncommitted for review. F04–F11 were not modified.
15. Implemented F04 at the user's request, starting from `c5c742381` on
    2026-10-09 and completing validation after resuming on 2026-10-10.
    Added dedicated JSON string decoding and regression fixtures, verified
    the baseline failures, corrected raw-byte test setup, and passed the
    release/debug JSON suites and full release target. Updated F04's status
    and left the changes uncommitted for review. F05–F11 were not implemented.
16. Implemented F05 at the user's request on 2026-10-10. Added document and
    streaming regressions, verified baseline failures, and corrected JSON
    whitespace, complete-document boundaries, and container separators.
    Release/debug JSON suites and the full release target passed. Removed
    the new header comments at the user's request. Updated F05's status and
    left its changes uncommitted for review. F06–F11 were not implemented.
17. Implemented F06 at the user's request on 2026-10-10, starting from
    `78a813ff3`. Added chunked-response behavior tests and reproduced the
    leak with a scoped native allocation probe. Changed only the chunk-node
    constructor in production code. Release/debug allocation checks, HTTP
    suites, and the full release target passed. Restored the original test
    dump and left all changes uncommitted for review. F07–F11 were not changed.
18. Implemented F07 at the user's request on 2026-10-10, starting from
    `8f6e72d66`. Reproduced wrapped allocations with a safe native probe,
    guarded container counts and header arithmetic, and bounded both
    pre-authentication message directions. Added three core regression cases;
    focused release/debug tests, authentication probes, and the full release
    target passed. Debug replication tests passed too. Restored the original
    test dump and release library links. F08–F11 were not changed; no commits
    were made.
19. After the user's acceptance of F07, implemented F08 from baseline
    `b5a995142`, completing validation on 2026-10-11. Reproduced the invalid
    layout acceptance without a data write, added layout and destination
    checks, and exercised nested failure cleanup and repeated references.
    Full release, focused release/debug, debug replication, and scoped
    allocation checks passed. Restored the original test dump and release
    links. Updated this log and left the patch uncommitted for review;
    F09–F11 remain open.
20. Implemented F09 at the user's request from baseline `957c3febe`, completing
    validation on 2026-10-12. Confirmed cross-kind cache reuse with a safe
    baseline probe, added type/construction/offset checks and eight regression
    groups, and corrected tuple cache hits in copy mode. Full release,
    focused release/debug, debug replication, and scoped allocation checks
    passed. Restored the original test dump and release library links. Left
    the patch uncommitted for review; F10–F11 remain open.
21. Implemented F10 at the user's request on 2026-10-12 from baseline
    `482153eeb`. Reproduced all three lookup failures before adding the two
    unqualified-name guards. Added C and interpreter regressions; focused
    release/debug, selected interpreter debug, and all 12 release suites
    passed. Restored the original test dump and release links. Updated this
    log and left changes uncommitted for review; F11 remains open.
22. Implemented F11 at the user's request on 2026-10-12 from baseline
    `f25e6223a`. Reproduced per-byte flushing and false-success write errors,
    buffered file output in 64 KiB blocks, and added six regression groups.
    Full release, focused release/debug, HTTP debug, scoped allocation, and
    native multipart checks passed. Restored the original test dump and
    release links. Updated this log and left the patch uncommitted for review.
    All eleven numbered findings are now implemented and tested.

## Validation and limitations

- All eleven findings originated in static review with concrete source paths.
  F01–F11 have been fixed and tested. F06's
  default-harness fixture checks behavior; its allocation checks use the
  separately retained native probe.
- Core release/debug libraries and test binaries were rebuilt for the fixes.
  The full release suite, focused core debug groups, and HTTP/JSON debug suites
  pass.
  The earlier full debug run had the module-loading limitation described under
  F01 and was not rerun in full.
- The latest project-wide release test command completed successfully across
  all 12 suites. Earlier TLS-fixture failures are retained above as history.
  Passing these tests does not establish exploit resistance or an exhaustive
  audit.
  No sanitizer run, fuzzing campaign, or network exploit validation was
  completed. The replication tests used their local listeners; no external
  systems were targeted.
- The earlier selected baseline tests used existing binaries; those results
  are separate from the freshly rebuilt F01–F11 validation.
- Graphics, GUI bindings, compiler output, dependencies, all concurrency paths,
  and the complete facts database implementation are outside the completed
  review scope.
