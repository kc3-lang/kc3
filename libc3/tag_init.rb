#!/usr/bin/env ruby

class Arg
  attr_reader :name
  attr_reader :type
  attr_reader :type_space

  def initialize(type, name)
    @type = type
    @name = name
    @type_space = type.match?(/\*$/) ? type : "#{type} "
  end
end

class TagInit
  attr_reader :args
  attr_reader :args_list
  attr_reader :args_proto
  attr_reader :args_proto_comma
  attr_reader :args_ptr
  attr_reader :comma_args
  attr_reader :comma_args_proto
  attr_reader :data_name
  attr_reader :first_arg_deref
  attr_reader :init_mode
  attr_reader :name
  attr_reader :name_suffix
  attr_reader :suffix
  attr_reader :tag_type

  def initialize(name, suffix = nil, tag_type, init_mode, args_list)
    @name = name
    @suffix = suffix ? "_#{suffix}" : nil
    @tag_type = tag_type
    @init_mode = init_mode
    @args_list = args_list

    @args = ""
    @args_ptr = ""
    @args_proto = ""
    @args_proto_comma = ""
    @comma_args = ""
    @comma_args_proto = ""
    @data_name = @name == "struct" ? "struct_" : @name

    @args_list.each do |arg|
      @first_arg_deref ||= arg.type != "const s_sym *" &&
                           arg.type != "s_list *" &&
                           arg.type.match?(/\*$/) ?
                             "*#{arg.name}" : arg.name
      @args = @args == "" ? arg.name : "#{@args}, #{arg.name}"
      arg_ptr = "&#{arg.name}"
      @args_ptr = @args_ptr == "" ? arg_ptr : "#{@args_ptr}, #{arg_ptr}"
      arg_proto = "#{arg.type_space}#{arg.name}"
      @args_proto = @args_proto == "" ? arg_proto :
                      "#{@args_proto}, #{arg_proto}"
      @comma_args = "#{@comma_args}, #{arg.name}"
      @comma_args_proto = "#{@comma_args_proto}, #{arg_proto}"
      @args_proto_comma = "#{@args_proto_comma}#{arg_proto}, "
    end

    @args_proto = "void" if @args_proto == ""

    @name_suffix = "#{name}#{@suffix}"
end

  def tag_proto
    "s_tag * tag_#{name_suffix} (s_tag *tag#{comma_args_proto});"
  end

  def tag_init_proto
    "s_tag * tag_init_#{name_suffix} (s_tag *tag#{comma_args_proto});"
  end

  def tag_new_proto
    "s_tag * tag_new_#{name_suffix} (#{args_proto});"
  end

  def with_list?
    ! ["1", "copy", "list"].include?(name)
  end

  def list_proto
    if with_list?
      "s_list * list_#{name_suffix} (s_list *list#{comma_args_proto});"
    else
      ""
    end
  end
  
  def list_init_proto
    if with_list?
      "s_list * list_init_#{name_suffix} (s_list *list#{comma_args_proto}, s_list *next);"
    else
      ""
    end
  end

  def list_new_proto
    if with_list?
      "s_list * list_new_#{name_suffix} (#{args_proto_comma}s_list *next);"
    else
      ""
    end
  end

  def def_tag
    <<EOF

#{tag_proto[0..-2]}
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  #{tag_type ? "tmp.type = #{tag_type};\n" : ""}#{def_tag_init_init}  *tag = tmp;
  return tag;
}
EOF
  end

  def def_tag_init
    <<EOF

#{tag_init_proto[0..-2]}
{
  s_tag tmp = {0};
  assert(tag);
  #{tag_type ? "tmp.type = #{tag_type};\n" : ""}#{def_tag_init_init}  *tag = tmp;
  return tag;
}
EOF
  end

  def def_tag_init_init
    case init_mode
    when :init_mode_none
      ""
    when :init_mode_init
      "  if (! #{name}_init#{suffix}(#{name == "list" ? "" : "&"}tmp.data.#{data_name}#{comma_args}))
    return NULL;
"
    when :init_mode_direct
      "  tmp.data.#{name} = #{first_arg_deref};
"
    when :init_mode_buf_parse
      "  s_buf buf;
  uw len;
  sw r;
  len = strlen(p);
  if (! buf_init(&buf, false, len, p))
    return NULL;
  if ((r = buf_parse_#{name}(&buf, &tmp.data.#{data_name}) < 0)
    return NULL;
  else if (r != len) {
    #{name}_clean(&tmp);
    return NULL;
  }
"
    else
      raise "unknown init mode: #{init_mode}"
    end
  end

  def def_tag_new
    <<EOF

#{tag_new_proto[0..-2]}
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_#{name_suffix}: calloc");
    return NULL;
  }
  #{tag_type ? "tag->type = #{tag_type};\n" : ""}#{def_tag_new_init}  return tag;
}
EOF
  end

  def def_tag_new_init
    case init_mode
    when :init_mode_none
      ""
    when :init_mode_init
      "  if (! #{name}_init#{suffix}(#{name == "list" ? "" : "&"}tag->data.#{data_name}#{comma_args})) {
    free(tag);
    return NULL;
  }
"
    when :init_mode_direct
      "  tag->data.#{data_name} = #{first_arg_deref};
"
    when :init_mode_buf_parse
      "  s_buf buf;
  uw len;
  sw r;
  len = strlen(p);
  if (! buf_init(&buf, false, len, p))
    return NULL;
  if ((r = buf_parse_#{name}(&buf, &tmp.data.#{data_name}) < 0)
    return NULL;
  else if (r != len) {
    #{name}_clean(&tmp);
    return NULL;
  }
"
    else
      raise "unknown init mode: #{init_mode}"
    end
  end

  def def_list_init
    if with_list?
      <<EOF

#{list_init_proto[0..-2]}
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_#{name_suffix}(&tmp.tag#{comma_args}))
    return NULL;
  *list = tmp;
  return list;
}
EOF
    end
  end

  def def_list_new
    if with_list?
      <<EOF

#{list_new_proto[0..-2]}
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_#{name_suffix}(&list->tag#{comma_args})) {
    free(list);
    return NULL;
  }
  return list;
}
EOF
    end
  end
end

class TagInitProto < TagInit
  def def_tag_init
    ""
  end

  def def_tag_new
    ""
  end

  def def_tag
    ""
  end

  def def_list_init
    ""
  end

  def def_list_new
    ""
  end
end

class TagInit1 < TagInit
  def initialize(name, suffix, tag_type, init_mode = :init_mode_init,
                 args_list = [Arg.new("const s8 *", "p")])
    super(name, suffix, tag_type, init_mode, args_list)
  end
end

class FileUpdate
  attr_reader   :path
  attr_accessor :content
  attr_reader   :content_old

  def initialize(path)
    @path = path
    @content_old = (File.read(path) rescue "")
  end

  def commit
    if @content != @content_old
      puts @path
      File.rename(@path, "#{@path}~") if File.exist?(@path)
      File.write(@path, @content)
    end
  end
end

class TagInitList
  attr_accessor :items

  def self.all
    new(
      [TagInit.new("array", "TAG_ARRAY", :init_mode_init,
                   [Arg.new("const s_sym *", "type"),
                    Arg.new("uw", "dimension"),
                    Arg.new("const uw *", "dimensions")]),
       TagInit.new("bool", "TAG_BOOL", :init_mode_direct,
                   [Arg.new("bool", "b")]),
       TagInit.new("call", "TAG_CALL", :init_mode_init, []),
       TagInit.new("character", "TAG_CHARACTER", :init_mode_direct,
                   [Arg.new("character", "c")]),
       TagInitProto.new("copy", nil, :init_mode_none,
                        [Arg.new("const s_tag *", "src")]),
       TagInit.new("f32", "TAG_F32", :init_mode_direct,
                   [Arg.new("f32", "f")]),
       TagInit.new("f64", "TAG_F64", :init_mode_direct,
                   [Arg.new("f64", "f")]),
       TagInit.new("ident", "TAG_IDENT", :init_mode_direct,
                   [Arg.new("const s_ident *", "ident")]),
       TagInit1.new("ident", "1", "TAG_IDENT", :init_mode_init),
#       TagInit.new("integer", "TAG_INTEGER", :init_mode_init, []),
       TagInit1.new("integer", "1", "TAG_INTEGER", :init_mode_init),
       TagInit.new("integer", "copy", "TAG_INTEGER", :init_mode_init,
                   [Arg.new("const s_integer *", "i")]),
       TagInit.new("integer", "zero", "TAG_INTEGER", :init_mode_init,
                   []),
       TagInit.new("list", "TAG_LIST", :init_mode_direct,
                   [Arg.new("s_list *", "list")]),
#       TagInit1.new("list", "1", "TAG_LIST", :init_mode_init,
#                    [Arg.new("const s8 *", "p"),
#                     Arg.new("s_list *", "next")]),
       TagInit.new("map", "TAG_MAP", :init_mode_init,
                   [Arg.new("uw", "count")]),
       TagInit1.new("map", "1", "TAG_MAP", :init_mode_init),
       TagInit.new("ptr", "TAG_PTR", :init_mode_init,
                   [Arg.new("void *", "p")]),
       TagInit.new("ptr_free", "TAG_PTR_FREE", :init_mode_init,
                   [Arg.new("void *", "p")]),
       TagInit.new("s8", "TAG_S8", :init_mode_direct,
                   [Arg.new("s8", "i")]),
       TagInit.new("s16", "TAG_S16", :init_mode_direct,
                   [Arg.new("s16", "i")]),
       TagInit.new("s32", "TAG_S32", :init_mode_direct,
                   [Arg.new("s32", "i")]),
       TagInit.new("s64", "TAG_S64", :init_mode_direct,
                   [Arg.new("s64", "i")]),
       TagInit.new("str", "TAG_STR", :init_mode_init,
                   [Arg.new("s8 *", "p_free"),
                    Arg.new("uw", "size"),
                    Arg.new("const s8 *", "p")]),
       TagInit.new("str", "1", "TAG_STR", :init_mode_init,
                   [Arg.new("s8 *", "p_free"),
                    Arg.new("const s8 *", "p")]),
       TagInit.new("struct", "TAG_STRUCT", :init_mode_init,
                   [Arg.new("const s_sym *", "module")]),
       TagInit.new("struct", "with_data", "TAG_STRUCT", :init_mode_init,
                   [Arg.new("const s_sym *", "module"),
                    Arg.new("bool", "free_data"),
                    Arg.new("void *", "data")]),
       TagInit.new("sw", "TAG_SW", :init_mode_direct,
                   [Arg.new("sw", "i")]),
       TagInit.new("sym", "TAG_SYM", :init_mode_direct,
                   [Arg.new("const s_sym *", "sym")]),
       TagInit1.new("sym", "1", "TAG_SYM", :init_mode_init),
       TagInit.new("tuple", "TAG_TUPLE", :init_mode_init,
                   [Arg.new("uw", "count")]),
       TagInit.new("tuple", "2", "TAG_TUPLE", :init_mode_init,
                   [Arg.new("const s_tag *", "a"),
                    Arg.new("const s_tag *", "b")]),
       TagInitProto.new("time", "TAG_TIME", :init_mode_none, []),
       TagInit.new("u8", "TAG_U8", :init_mode_direct,
                   [Arg.new("u8", "i")]),
       TagInit.new("u16", "TAG_U16", :init_mode_direct,
                   [Arg.new("u16", "i")]),
       TagInit.new("u32", "TAG_U32", :init_mode_direct,
                   [Arg.new("u32", "i")]),
       TagInit.new("u64", "TAG_U64", :init_mode_direct,
                   [Arg.new("u64", "i")]),
       TagInit.new("uw", "TAG_UW", :init_mode_direct,
                   [Arg.new("uw", "i")]),
       TagInit.new("var", "TAG_VAR", :init_mode_none, []),
       TagInit.new("void", "TAG_VOID", :init_mode_none, [])])
  end

  def initialize(items)
    @items = items
  end

  def tag_proto
    protos = @items.map &:tag_proto
    protos.join "\n"
  end

  def tag_init_proto
    protos = @items.map &:tag_init_proto
    protos.join "\n"
  end

  def tag_new_proto
    protos = @items.map &:tag_new_proto
    protos.join "\n"
  end

  def list_proto
    protos = @items.map &:list_proto
    protos.join "\n"
  end

  def list_init_proto
    protos = @items.map &:list_init_proto
    protos.join "\n"
  end

  def list_new_proto
    protos = @items.map &:list_new_proto
    protos.join "\n"
  end

  def def_tag
    protos = @items.map &:def_tag
    protos.join
  end

  def def_tag_init
    protos = @items.map &:def_tag_init
    protos.join
  end

  def def_tag_new
    protos = @items.map &:def_tag_new
    protos.join
  end

  def def_list_init
    protos = @items.map &:def_list_init
    protos.join
  end

  def def_list_new
    protos = @items.map &:def_list_new
    protos.join
  end
end

$license = File.read("../license.h").strip

def h_header(name)
  "#{$license}
#ifndef #{name}
#define #{name}

#include \"types.h\""
end

def h_footer(name)
  "#endif /* #{name} */"
end

class String
  def c_word_wrap
    lines = split("\n")
    wrapped_lines = []
    lines.each do |line|
      rest = line
      while rest.length > 72
        space_pos = rest[0..71].rindex(/,\s/)
        space_pos += 1 if space_pos
        space_pos ||= rest[0..71].rindex(/[\*\s]/)
        line1 = rest[0..space_pos]
        rest = rest[(space_pos + 1)..(rest.length - 1)]
        wrapped_lines += [line1]
        index = line1.rindex(/[\(\{\[]/)
        if index
          paren_pos = index + 1
        end
        rest = paren_pos ? (" " * paren_pos + rest) : rest
      end
      wrapped_lines += [rest]
    end
    wrapped_lines.join("\n")
  end
end

inits = TagInitList.all

tag_init_c = FileUpdate.new("tag_init.c")
tag_init_c.content = <<EOF
#{$license}
#include <assert.h>
#include <err.h>
#include <string.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "ptr.h"
#include "ptr_free.h"
#include "quote.h"
#include "str.h"
#include "struct.h"
#include "tag.h"
#include "tag_init.h"
#include "time.h"
#include "tuple.h"
#{inits.def_tag_init.c_word_wrap}
#{inits.def_tag_new.c_word_wrap}
#{inits.def_tag.c_word_wrap}
EOF
tag_init_c.commit

tag_init_h = FileUpdate.new("tag_init.h")
tag_init_h.content = """#{h_header("LIBC3_TAG_INIT_H")}

/* Stack-allocation compatible functions, call tag_clean after use. */
#{inits.tag_init_proto.c_word_wrap}

/* Heap-allocation functions, call tag_delete after use. */
#{inits.tag_new_proto.c_word_wrap}

/* Setters. */
#{inits.tag_proto.c_word_wrap}

#{h_footer("LIBC3_TAG_INIT_H")}
"""
tag_init_h.commit

list_init_c = FileUpdate.new("list_init.c")
list_init_c.content = <<EOF
#{$license}
#include <assert.h>
#include <err.h>
#include <string.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "ptr.h"
#include "ptr_free.h"
#include "quote.h"
#include "str.h"
#include "struct.h"
#include "tag.h"
#include "list_init.h"
#include "time.h"
#include "tuple.h"
#{inits.def_list_init.c_word_wrap}
#{inits.def_list_new.c_word_wrap}
EOF
list_init_c.commit

list_init_h = FileUpdate.new("list_init.h")
list_init_h.content = """#{h_header("LIBC3_LIST_INIT_H")}

/* Stack-allocation compatible functions, call list_clean after use. */
#{inits.list_init_proto.c_word_wrap}

/* Heap-allocation functions, call list_delete after use. */
#{inits.list_new_proto.c_word_wrap}

/* Setters. */
#{inits.list_proto.c_word_wrap}

#{h_footer("LIBC3_LIST_INIT_H")}
"""
list_init_h.commit
