/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "alloc.h"
#include "cow.h"
#include "tag.h"

void cow_clean (s_cow *cow)
{
  assert(cow);
  tag_clean(&cow->r);
  tag_clean(&cow->w);
}

void cow_delete (s_cow *cow)
{
  assert(cow);
  cow_clean(cow);
  free(cow);
}

s_cow * cow_freeze (s_cow *cow)
{
  assert(cow);
  if (cow->w_is_set) {
    tag_clean(&cow->r);
    tag_init_copy(&cow->r, &cow->w);
    cow->w_is_set = false;
    tag_clean(&cow->w);
  }
  return cow;
}

s_cow * cow_freeze_tag (s_cow *cow, const s_tag *src)
{
  assert(cow);
  assert(src);
  cow->w_is_set = false;
  tag_clean(&cow->w);
  tag_clean(&cow->r);
  if (! tag_init_copy(&cow->r, src))
    return NULL;
  return cow;
}

s_cow * cow_init (s_cow *cow)
{
  s_cow tmp = {0};
  assert(cow);
  *cow = tmp;
  return cow;
}

s_cow * cow_init_1 (s_cow *cow, const char *utf8)
{
  assert(cow);
  assert(utf8);
  (void) cow;
  (void) utf8;
  err_puts("cow_init_1: FIXME: not implemented");
  assert(! "cow_init_1: FIXME: not implemented");
  return NULL;
}

s_cow * cow_init_cast (s_cow *cow, const s_sym * const *type,
                       const s_tag *tag)
{
  assert(cow);
  assert(type);
  assert(tag);
  (void) type;
  if (tag->type == TAG_COW)
    return cow_init_copy(cow, tag->data.cow);
  return cow_freeze_tag(cow, tag);
}

s_cow * cow_init_copy (s_cow *cow, const s_cow *src)
{
  assert(cow);
  assert(src);
  if (! tag_init_copy(&cow->r, &src->r) ||
      ! tag_init_copy(&cow->w, &src->w))
    return NULL;
  return cow;
}

s_cow * cow_init_tag_copy (s_cow *cow, const s_tag *src)
{
  s_cow tmp = {0};
  assert(cow);
  assert(src);
  if (! tag_init_copy(&tmp.r, src))
    return NULL;
  *cow = tmp;
  return cow;
}

s_str * cow_inspect (const s_cow *cow, s_str *dest)
{
  assert(cow);
  assert(dest);
  (void) cow;
  (void) dest;
  err_puts("cow_inspect: FIXME: not implemented");
  assert(! "cow_inspect: FIXME: not implemented");
  return NULL;
}

s_cow * cow_new (void)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  cow_init(cow);
  return cow;
}

s_cow * cow_new_cast (const s_sym * const *type, const s_tag *tag)
{
  s_cow *cow;
  assert(type);
  assert(tag);
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_cast(cow, type, tag)) {
    free(cow);
    return NULL;
  }
  return cow;
}

s_cow * cow_new_copy (const s_cow *src)
{
  s_cow *cow;
  assert(src);
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_copy(cow, src)) {
    free(cow);
    return NULL;
  }
  return cow;
}

const s_tag * cow_ro (const s_cow *cow)
{
  assert(cow);
  if (cow->w_is_set) {
    // FIXME: get a read lock ?
    return &cow->w;
  }
  return &cow->r;
}

s_tag * cow_rw (s_cow *cow)
{
  if (! cow->w_is_set &&
      ! cow_thaw(cow))
    return NULL;
  return &cow->w;
}

s_cow * cow_thaw (s_cow *cow)
{
  assert(cow);
  tag_clean(&cow->w);
  if (! tag_init_copy(&cow->w, &cow->r))
    return NULL;
  cow->w_is_set = true;
  return cow;
}

s_cow * cow_thaw_tag (s_cow *cow, const s_tag *src)
{
  assert(cow);
  assert(src);
  tag_clean(&cow->w);
  if (! tag_init_copy(&cow->w, src))
    return NULL;
  cow->w_is_set = true;
  return cow;
}
