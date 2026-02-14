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
#include "../libkc3/kc3.h"
#include "pdf_file.h"
#include "pdf_name.h"
#include "pdf_trailer.h"

s_pdf_file * pdf_file_init (s_pdf_file *pdf_file)
{
  s_pdf_file tmp = {0};
  *pdf_file = tmp;
  return pdf_file;
}

void pdf_file_clean (s_pdf_file *pdf_file)
{
  pdf_trailer_clean(&pdf_file->trailer);
  str_clean(&pdf_file->header);
  map_clean(&pdf_file->body);
  map_clean(&pdf_file->xref);
  pdf_name_list_delete_all(&pdf_file->name_list);
}

s_tag * pdf_file_get_indirect_object (s_pdf_file *pdf_file,
                                      s_tag *ref, s_tag *dest)
{
  u32 object_number;
  u16 generation_number;
  uw i;
  if (! pdf_file)
    return tag_init_void(dest);
  if (ref->type != TAG_PTUPLE ||
      ref->data.ptuple->count != 3 ||
      ref->data.ptuple->tag[0].type != TAG_PSYM ||
      ref->data.ptuple->tag[0].data.psym != sym_1("indirect_object") ||
      ref->data.ptuple->tag[1].type != TAG_U32 ||
      ref->data.ptuple->tag[2].type != TAG_U16) {
    err_puts("pdf_file_get_indirect_object: invalid indirect object"
             " reference:");
    err_inspect_tag(ref);
    assert(!("pdf_file_get_indirect_object: invalid indirect object"
             " reference"));
    return NULL;
  }
  object_number = ref->data.ptuple->tag[1].data.u32;
  generation_number = ref->data.ptuple->tag[2].data.u16;
  i = 0;
  while (i < pdf_file->body.count) {
    if (pdf_file->body.key[i].type != TAG_PTUPLE ||
        pdf_file->body.key[i].data.ptuple->count != 2 ||
        pdf_file->body.key[i].data.ptuple->tag[0].type != TAG_U32 ||
        pdf_file->body.key[i].data.ptuple->tag[1].type != TAG_U16) {
      err_puts("pdf_file_get_indirect_object: invalid pdf file body");
      assert(! "pdf_file_get_indirect_object: invalid pdf file body");
      return NULL;
    }
    if (pdf_file->body.key[i].data.ptuple->tag[0].data.u32 ==
        object_number &&
        pdf_file->body.key[i].data.ptuple->tag[1].data.u16 ==
        generation_number)
      return tag_init_copy(dest, pdf_file->body.value + i);
    i++;
  }
  return tag_init_void(dest);
}
