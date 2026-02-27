/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include "md4c.h"
#include "md4c-html.h"
#include "../libkc3/kc3.h"
#include "markdown.h"

s32 markdown_titles_enter_block (MD_BLOCKTYPE type, void *detail,
                                 void *data);
s32 markdown_titles_leave_block (MD_BLOCKTYPE type, void *detail,
                                 void *data);
s32 markdown_titles_enter_span (MD_SPANTYPE type, void *detail,
                                void *data);
s32 markdown_titles_leave_span (MD_SPANTYPE type, void *detail,
                                void *data);
s32 markdown_titles_text (MD_TEXTTYPE type, const MD_CHAR *text,
                          MD_SIZE size, void *data);
void markdown_to_html_str_callback (const MD_CHAR *, MD_SIZE, void *);

s_str * markdown_to_html_str (const s_str *markdown, s_str *dest)
{
  s_list *list;
  s_list **l;
  s_str tmp;
  assert(markdown);
  assert(dest);
  list = NULL;
  l = &list;
  md_html(markdown->ptr.p_pchar, markdown->size,
          markdown_to_html_str_callback,
          &l, 0, 0);
  if (! str_init_concatenate_list(&tmp, list)) {
    list_delete_all(list);
    return NULL;
  }
  list_delete_all(list);
  *dest = tmp;
  return dest;
}

void markdown_to_html_str_callback (const MD_CHAR *p, MD_SIZE size,
                                    void *data)
{
  s_list **l;
  l = *(s_list ***) data;
  if (! (*l = list_new_str_alloc_copy(size, p, NULL))) {
    err_puts("markdown_to_html_str_callback: list_new_str_cpy");
    assert(! "markdown_to_html_str_callback: list_new_str_cpy");
    return;
  }
  l = &(*l)->next.data.td_plist;
  *(s_list ***) data = l;
}

s_list ** markdown_titles (const s_str *markdown, s_list **dest)
{
  MD_PARSER parser = {0};
  s_tuple tuple;
  s_tag  *tuple_count;
  s_tag  *tuple_last;
  s_tag  *tuple_list;
  s_tag  *tuple_str;
  assert(markdown);
  assert(dest);
  tuple_init(&tuple, 9);
  tuple_last = tuple.tag;
  tuple_list = tuple.tag + 1;
  tuple_str = tuple.tag + 2;
  tuple_count = tuple.tag + 3;
  tag_init_u8(tuple_last, 0);
  tag_init_plist(tuple_list, NULL);
  tag_init_str_empty(tuple_str);
  tag_init_u8(tuple_count, 0);
  tag_init_u8(tuple_count + 1, 0);
  tag_init_u8(tuple_count + 2, 0);
  tag_init_u8(tuple_count + 3, 0);
  tag_init_u8(tuple_count + 4, 0);
  tag_init_u8(tuple_count + 5, 0);
  parser.enter_block = markdown_titles_enter_block;
  parser.leave_block = markdown_titles_leave_block;
  parser.enter_span = markdown_titles_enter_span;
  parser.leave_span = markdown_titles_leave_span;
  parser.text = markdown_titles_text;
  md_parse(markdown->ptr.p_pchar, markdown->size,
           &parser, &tuple);
  *dest = tuple_list->data.td_plist;
  tuple_list->data.td_plist = NULL;
  tuple_clean(&tuple);
  return dest;
}

s32 markdown_titles_enter_block (MD_BLOCKTYPE type, void *detail,
                                 void *data)
{
  s_tuple *tuple;
  s_tag   *tuple_str;
  (void) detail;
  assert(data);
  tuple = data;
  if (tuple->count != 9) {
    err_puts("markdown_titles_enter_block: invalid tuple");
    assert(! "markdown_titles_enter_block: invalid tuple");
    return -1;
  }
  tuple_str = tuple->tag + 2;
  if (type == MD_BLOCK_H)
    tag_void(tuple_str);
  return 0;
}

s32 markdown_titles_leave_block (MD_BLOCKTYPE type, void *detail,
                                 void *data)
{
  MD_BLOCK_H_DETAIL *detail_h;
  u8 i;
  u8 level;
  s_list *str_list;
  p_list *str_list_tail;
  const s_sym *sym_Str;
  s_tuple *tuple;
  s_tag   *tuple_count;
  s_tag   *tuple_count_level;
  s_tag   *tuple_last;
  s_tag   *tuple_list;
  p_list  *tuple_list_tail;
  s_tag   *tuple_str;
  assert(data);
  sym_Str = &g_sym_Str;
  tuple = data;
  if (tuple->count != 9) {
    err_puts("markdown_titles_leave_block: invalid tuple");
    assert(! "markdown_titles_leave_block: invalid tuple");
    return -1;
  }
  tuple_last = tuple->tag;
  tuple_list = tuple->tag + 1;
  tuple_str = tuple->tag + 2;
  tuple_count = tuple->tag + 3;
  if (type == MD_BLOCK_H) {
    assert(detail);
    detail_h = detail;
    if (detail_h->level < 1 || detail_h->level > 6) {
      err_puts("markdown_titles_leave_block: invalid level");
      assert(! "markdown_titles_leave_block: invalid level");
      return -1;
    }
    level = detail_h->level - 1;
    tuple_count_level = tuple_count + level;
    if (tuple_count_level->type != TAG_U8) {
      err_puts("markdown_titles_leave_block: tuple_count_level"
               " is not a U8");
      assert(!("markdown_titles_leave_block: tuple_count_level"
               " is not a U8"));
      return -1;
    }
    tuple_count_level->data.td_u8++;
    if (tuple_last->type != TAG_U8) {
      err_puts("markdown_titles_leave_block: tuple_last is not a U8");
      assert(! "markdown_titles_leave_block: tuple_last is not a U8");
      return -1;
    }
    tuple_last->data.td_u8 = level;
    str_list = NULL;
    str_list_tail = &str_list;
    i = 0;
    while (i <= level) {
      *str_list_tail = list_new_str_cast(&sym_Str, tuple_count + i,
                                         NULL);
      str_list_tail = &(*str_list_tail)->next.data.td_plist;
      *str_list_tail = list_new_str_1(NULL, ".", NULL);
      str_list_tail = &(*str_list_tail)->next.data.td_plist;
      i++;
    }
    while (i < 6) {
      tag_init_u8(tuple_count + i, 0);
      i++;
    }
    *str_list_tail = list_new_str_1(NULL, " ", NULL);
    str_list_tail = &(*str_list_tail)->next.data.td_plist;
    if (tuple_str->type != TAG_STR) {
      err_puts("markdown_titles_leave_block: tuple_str is not a Str");
      assert(! "markdown_titles_leave_block: tuple_str is not a Str");
      return -1;
    }
    *str_list_tail = list_new_str_copy(&tuple_str->data.td_str, NULL);
    str_list_tail = &(*str_list_tail)->next.data.td_plist;
    tuple_list_tail = plist_tail(&tuple_list->data.td_plist);
    *tuple_list_tail = list_new(NULL);
    if (! tag_init_str_concatenate_list(&(*tuple_list_tail)->tag,
                                        str_list))
      return -1;
    list_delete_all(str_list);
    if (tuple_list->type != TAG_PLIST) {
      err_puts("markdown_titles_leave_block: tuple_list is not a List");
      assert(! "markdown_titles_leave_block: tuple_list is not a List");
      return -1;
    }
    tag_void(tuple_str);
  }
  return 0;
}

s32 markdown_titles_enter_span (MD_SPANTYPE type, void *detail,
                                void *data)
{
  (void) type;
  (void) detail;
  (void) data;
  return 0;
}

s32 markdown_titles_leave_span (MD_SPANTYPE type, void *detail,
                                void *data)
{
  (void) type;
  (void) detail;
  (void) data;
  return 0;
}

s32 markdown_titles_text (MD_TEXTTYPE type, const MD_CHAR *text,
                          MD_SIZE size, void *data)
{
  s_tuple *tuple;
  s_tag   *tuple_str;
  (void) type;
  tuple = data;
  if (tuple->count != 9) {
    err_puts("markdown_titles_text: invalid tuple");
    assert(! "markdown_titles_text: invalid tuple");
    return -1;
  }
  tuple_str = tuple->tag + 2;
  if (tuple_str->type == TAG_VOID)
    if (! tag_init_str_alloc_copy(tuple_str, size, text))
      return -1;
  return 0;
}
