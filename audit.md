# KC3 v0.2

## libkc3 re-audit — 2026-10-18

**Baseline:** `295b496b6` (`audit`); tracked worktree clean at the start.
The earlier F01–F11 report and restored F12–F23 report were consulted to
avoid renumbering or presenting their documented fixes as new work.

**Method:** source inspection, type/size arithmetic, ownership tracing, and
caller tracing. All examples below are static derivations, not executed
reproductions. No builds, tests, probes, sanitizers, or network sessions were
run. OpenBSD manual pages were consulted for the system-call contracts in
F29/F31. Suggested regressions are future work in the existing `test/`
infrastructure, not standalone probes or tests already added.

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
| F24 | High | UTF-8 decoder uses a stale pointer after refill compaction | Open; source-confirmed |
| F25 | High | Shell escaping returns an unquoted suffix after invalid UTF-8 | Open; source-confirmed |
| F26 | High | Base64 output-size arithmetic wraps before allocation | Open; source-confirmed |
| F27 | Medium | String substitution spins on malformed UTF-8 | Open; source-confirmed |
| F28 | Medium | Environment cleanup permanently enables the teardown bypass | Open; source-confirmed |
| F29 | Medium | Socket flush fallback defeats SIGPIPE suppression | Open; source-confirmed |
| F30 | Medium | TLS flush does not move the saved unwritten tail | Open; source-confirmed |
| F31 | Medium | FD/TLS flush errors forget partial-write progress | Open; source-confirmed |
| F32 | Low | Substitution sizing omits an unmatched trailing suffix | Open; source-confirmed |
| F33 | Medium | RPC and Base64 discard temporary buffers without freeing their locks | Open; source-confirmed |

### F24 — High: UTF-8 decoding uses a pointer invalidated by compaction

**Locations:** `libkc3/buf.c:633–728`, `libkc3/buf.c:1505–1592`.

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

### F25 — High: malformed UTF-8 bypasses shell escaping

**Locations:** `libkc3/sh.c:48–64,80–92`,
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

### F28 — Medium: teardown mode survives after a nested environment is gone

**Locations:** `libkc3/env.c:498–499,522–529,569–575,1847–1857,2154–2169`,
`libkc3/facts.c:2115–2120,2155–2160,2335–2340`,
`libkc3/data.c:403–411`.

`env_cleaning` holds a process-global static boolean. Passing true sets it
permanently; passing false only reads it. `env_clean` sets this flag when
cleaning the current environment, but never restores it when returning to
`parent_env`. `env_init`/`env_global_set` explicitly support installing an
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

### F29 — Medium: the socket error fallback reintroduces SIGPIPE

**Locations:** `libkc3/buf_fd.c:185–235`,
`libkc3/buf_rw.c:37–50`, `libkc3/socket_buf.c:133–163`.

`buf_fd_open_w_flush` first uses `send(..., MSG_NOSIGNAL)`, but falls back to
`write` after **every** negative result, not just the non-socket case. For
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

### F30 — Medium: successful TLS flush loses a saved suffix

**Locations:** `libkc3/tls_buf.c:127–175`,
`libkc3/buf_save.c:26–66`. Compare `libkc3/buf_fd.c:239–246`.

The TLS adapter flushes only `buf_save_min_wpos(buf)` bytes, respecting
saved write positions. On success it reduces `buf->wpos` and the saved
positions, but never moves the unwritten suffix to the start of the buffer.
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

### F31 — Medium: partial flush errors leave sent bytes queued for replay

**Locations:** `libkc3/buf_fd.c:40–47,207–246`,
`libkc3/tls_buf.c:28–37,148–173`.

Both adapters track successfully written bytes only in a local `bytes`
variable until the whole permitted prefix is sent. Any later error or
timeout returns immediately without removing that completed prefix from
the buffer or adjusting saved positions. Partial successful writes are
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

### F32 — Low: substitution sizing drops the unmatched trailing bytes

**Locations:** `libkc3/str.c:1365–1428`, `libkc3/buf.c:2123–2148`.

The sizing loop stops when fewer than `search->size` bytes remain, then
returns `result` without adding that remaining suffix. The emitting pass
does try to copy the suffix with `buf_xfer`, but its fixed output buffer
was sized using the incomplete total.

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

### F33 — Medium: temporary buffers leak their allocated rwlocks

**Locations:** `libkc3/rpc.c:23–85`, `libkc3/base64.c:39–79,82–127`,
`libkc3/buf.c:188–198,400–432,1105–1151`, `libkc3/rwlock.c:86–108`.

With `HAVE_PTHREAD`, `buf_init` and `buf_init_const` allocate a separate
rwlock even when the byte storage is borrowed or stack-backed. That lock
must be released with buffer cleanup; merely copying out the bytes does
not transfer or destroy it.

`rpc_request` initializes stack-backed error/output buffers, but calls
`buf_clean` on neither buffer on any ordinary return path. Each successful
request therefore loses two allocated locks. Restoring `env->out` and
`env->err`, or cleaning the returned `RPC.Response`, does not own/free those
temporary locks. The response contains strings and the result, not buffers.

Both `base64_encode` (for nonempty input) and `base64_decode` similarly
initialize an input buffer and return without cleaning it. Decode cleans
its output buffer, but not its input. These are small-input, normal-success
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
supported. No leak measurement was run during this audit.

### Re-audit disposition and limits

- F24–F33 remain open. No fixes or regression tests were added in this turn.
- Historical F12–F23 completion records were preserved, not independently
  revalidated by running their suites. These new findings do not revoke
  those earlier findings' narrowly documented fixes.
- Static examples establish the source-level failures described above;
  exploitability of a particular network service, resource thresholds in
  deployment, and platform-specific runtime behavior require separate work.
- Only `audit.md` was restored/updated. No source, headers, test fixtures,
  manifests, or build configuration were changed; no commit was made.
