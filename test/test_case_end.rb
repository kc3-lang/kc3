require 'fileutils'
Dir["*.c"].each do |file|
  file_tmp = file + ".tmp"
  File.open(file_tmp, "w") do |out|
    test_case_name = nil
    File.foreach(file) do |line|
      line.match(/^TEST_CASE\((.*)\)$/) do |m|
        test_case_name = m[1]
      end
      out.write line
      out.puts "TEST_CASE_END(#{test_case_name})" if line == "}\n"
    end
  end
  FileUtils.cp(file, file + "~")
  FileUtils.mv(file_tmp, file)
end
