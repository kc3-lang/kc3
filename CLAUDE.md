# Claude Assistant Notes for KC3 Project

## Search Tool Limitations
- Always test Grep with a simple pattern first since rg might break at any moment
- Use BSD-style conventions: options come before arguments (e.g., `Grep -n -A 5 pattern path`)
- Workaround: Use Read tool with specific line numbers when given locations
- Use Task tool for broader searches when Grep fails
- Always verify search results by asking for confirmation

## C String Semantics Issues
- Do not confidently interpret C string literal byte counts or memory layouts
- Ask for verification: "This is X bytes total, right?" instead of assuming
- Be tentative about endianness and binary data interpretations
- Let the user be the authoritative source on technical details

## General Debugging Approach
- Be less confident in technical assertions
- Ask more clarifying questions instead of flip-flopping between wrong answers
- Use the human as the source of truth rather than making assumptions
- Help think through problems without overconfident analysis

## KC3 Project Specifics
- Marshall/unmarshall system: Fixed string handling and header reading issues
- Tests: `make test_debug` runs all tests, `KC3_TEST=name make test_libkc3_debug` for specific tests
- On OpenBSD: Stack variables are zero-initialized
- Buffer overflow protection added to marshall_read_header with size validation