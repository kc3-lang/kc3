File.open("buf_parse_test_s.out", "w") do |out|
  result = 0
  base = 0..7
  base_prefix = "0o"
  negative = "-"
  base.each do |i|
    base.each do |j|
      base.each do |k|
        if (i == 0 && j == 0)
          out.puts "  BUF_PARSE_TEST_S(8, \"#{negative}#{base_prefix}#{k}\", #{negative}#{result});"
        end
        if (i == 0)
          out.puts "  BUF_PARSE_TEST_S(8, \"#{negative}#{base_prefix}#{j}#{k}\", #{negative}#{result});"
        end
        out.puts "  BUF_PARSE_TEST_S(8, \"#{negative}#{base_prefix}#{i}#{j}#{k}\", #{negative}#{result});"
        result += 1
      end
    end
  end
end
