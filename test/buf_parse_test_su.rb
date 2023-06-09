## c3
## Copyright 2022,2023 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

def one(out, su, bits, base_prefix, negative, digits, result)
  r = if result != 0 then "#{negative}#{result}" else "#{result}" end
  if r == "-9223372036854775808"
    # XXX bug in clang integer litterals
    r = "-9223372036854775807 - 1"
  end
  if su == "U"
    r = "#{r}U"
  end
  output = Proc.new do |digits|
    zeros = 0
    spaces = ""
    digits.each do |digit|
      if digit != 0 || zeros >= digits.count - 1
        break
      end
      zeros += 1
    end
    i = zeros
    while i >= 0
      test_digits = digits[i..(digits.count - 1)]
      test_digits = test_digits.join
      spaces = " " * (i + ((result.zero? && negative == "-") ? 1 : 0))
      out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{test_digits}\", #{spaces}#{r});"
      i -= 1
    end
  end
  output.(digits)
  string_digits = false
  digits_upcase = digits.map do |digit|
    if digit.is_a?(String)
      string_digits = true
      digit.upcase
    else
      digit
    end
  end
  output.(digits_upcase) if string_digits
end

def count(out, su, bits, base, base_prefix, negative, max)
  result = 0
  base.each do |i|
    i = 0 if i == "0"
    base.each do |j|
      j = 0 if j == "0"
      base.each do |k|
        one(out, su, bits, base_prefix, negative, [i, j, k], result)
        result += 1
        if result > max
          return result
        end
      end
    end
  end
  return result
end

def number_to_base(m, base)
  digits = []
  radix = base.count
  while m > 0
    digits.push(base[m % radix])
    m = m / radix
  end
  digits.reverse
end

def integer_max(su, bits, negative)
  if negative == "-"
    2 ** (if su == "S" then bits - 1; else bits; end)
  else
    2 ** (if su == "S" then bits - 1; else bits; end) - 1
  end
end

BITS = [8, 16, 32, 64]

licence = File.read("../licence.h")

["S", "U"].each do |su|
  BITS.each do |bits|
    su_downcase = su.downcase
    filename = "buf_parse_test_#{su_downcase}#{bits}.c"
    filename_tmp = "#{filename}.tmp"
    File.open(filename_tmp, "w") do |out|
      out.puts licence
      out.puts '#include "buf_parse_test.h"'
      out.puts "#ifdef C3_TEST_BUF_PARSE_SU"
      [[[0, 1], "0b", "binary"],
       [[0, 1, 2, 3, 4, 5, 6, 7], "0o", "octal"],
       [[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, "a", "b", "c", "d", "e", "f"],
        "0x", "hexadecimal"],
       [[0, 1, 2, 3, 4, 5, 6, 7, 8, 9], "", "decimal"]].each do |b|
        base = b[0]
        base_prefix = b[1]
        base_name = b[2]
        radix = base.count
        signs = if (su == "S")
                  ["", "-"]
                else
                  [""]
                end
        signs.each do |negative|
          function_name = "buf_parse_#{su_downcase}#{bits}_#{base_name}"
          if negative == "-"
            function_name += "_negative"
          end
          out.puts ''
          out.puts "TEST_CASE(#{function_name})"
          out.puts '{'
          max = integer_max(su, bits, negative)
          result = count(out, su, bits, base, base_prefix, negative,
                         max)
          BITS.each do |b|
            if b <= bits
              max = integer_max(su, b, negative)
              if result < max - 1
                digits = number_to_base(max - 1, base)
                one(out, su, bits, base_prefix, negative, digits, max - 1)
              end
              if result < max
                digits = number_to_base(max, base)
                one(out, su, bits, base_prefix, negative, digits, max)
              end
            end
          end
          out.puts "}"
          out.puts "TEST_CASE_END(#{function_name})"
        end
      end
      out.puts "#endif /* ifdef C3_TEST_BUF_PARSE_SU */"
    end # File.open
    cmd = "if ! diff -u #{filename} #{filename_tmp}; then mv #{filename_tmp} #{filename}; echo #{filename}; else rm #{filename_tmp}; fi"
    raise "command failed" unless system(cmd)
  end
end
