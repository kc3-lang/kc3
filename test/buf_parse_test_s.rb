File.open("buf_parse_test_s.out", "w") do |out|
  ["S", "U"].each do |su|
    [8].each do |bits|
      out.puts "void buf_parse_test_#{su.downcase}#{bits} ()"
      out.puts "{"
      [[0..1, "0b"],
       [0..7, "0o"],
       [0..15, "0x"],
       [0..10, ""]].each do |b|
        base = b[0]
        base_prefix = b[1]
        signs = if (su == "S")
                  ["-", ""]
                else
                  [""]
                end
        signs.each do |negative|
          result = 0
          base.each do |i|
            base.each do |j|
              base.each do |k|
                if (i == 0 && j == 0)
                  out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{k}\",   #{negative}#{result});"
                end
                if (i == 0)
                  out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{j}#{k}\",  #{negative}#{result});"
                end
                out.puts "  BUF_PARSE_TEST_#{su}(#{bits}, \"#{negative}#{base_prefix}#{i}#{j}#{k}\", #{negative}#{result});"
                result += 1
              end
            end
          end
        end
      end
      out.puts "}"
    end
  end
end
