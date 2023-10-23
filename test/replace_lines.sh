#!/bin/sh
grep -E "^TEST_CASE\(" *.c | while read L; do F="$(echo "$L" | cut -d : -f 1)"; S="$(echo "$L" | cut -d : -f 2)"; N="TEST_CASE($(echo "$S" | cut -c 17-)"; echo "['$F', '$S', '$N']"; done > test.rb
