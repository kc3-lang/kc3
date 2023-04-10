
def count(out, su, bits, base, base_prefix, negative)
  result = 0
  if negative == "-"
    max = 2 ** (if su == "S" then bits - 1; else bits; end)
  else
    max = 2 ** (if su == "S" then bits - 1; else bits; end) - 1
  end
  base.each do |i|
    i = 0 if i == "0"
    base.each do |j|
      j = 0 if j == "0"
      base.each do |k|
        r = if result != 0 then "#{negative}#{result}" else result end
        output = Proc.new do |i, j, k|
          if (i == 0 && j == 0)
            out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{k}\",   #{r});"
          end
          if (i == 0)
            out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{j}#{k}\",  #{r});"
          end
          out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{i}#{j}#{k}\", #{r});"
        end
        output.(i, j, k)
        if i.is_a?(String) || j.is_a?(String) || k.is_a?(String)
          output.(i.is_a?(String) ? i.upcase : i,
                  j.is_a?(String) ? j.upcase : j,
                  k.is_a?(String) ? k.upcase : k)
        end
        result += 1
        if result > max
          return
        end
      end
    end
  end
end

File.open("buf_parse_test_s.out", "w") do |out|
  ["S", "U"].each do |su|
    [8].each do |bits|
      out.puts ""
      out.puts "void buf_parse_test_#{su.downcase}#{bits} ()"
      out.puts "{"
      [[0..1, "0b"],
       [0..7, "0o"],
       [[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, "a", "b", "c", "d", "e", "f"], "0x"],
       [0..9, ""]].each do |b|
        base = b[0]
        base_prefix = b[1]
        signs = if (su == "S")
                  ["-", ""]
                else
                  [""]
                end
        signs.each do |negative|
          count(out, su, bits, base, base_prefix, negative)
        end
      end
      out.puts "}"
    end
  end
end
