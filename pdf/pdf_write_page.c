
s_pdf_write_page * pdf_write_page_init (s_pdf_write_page *page,
                                        s_pdf_write *pdf,
                                        s_pdf_rect *box,
                                        s_map *dest)
{
  u32 content_stream_object_number;
  s_pdf_write_page tmp = {0};
  if (! map_init(&tmp.map, 5))
    return NULL;
  tmp.object_number = pdf_write_object_number_register(pdf);
  content_stream_object_number = pdf_write_object_number_register(pdf);
  tag_init_psym(tmp.map.key,   sym_1("Type"));
  tag_init_psym(tmp.map.value, sym_1("Page"));
  tag_init_psym(tmp.map.key       + 1, sym_1("Parent"));
  pdf_tag_init_xref(tmp.map.value + 1, PDF_OBJECT_NUMBER_PAGES, 0);
  tag_init_psym(tmp.map.key       + 2, sym_1("Resources"));
  pdf_tag_init_xref(tmp.map.value + 2, PDF_OBJECT_NUMBER_RESOURCES, 0);
  tag_init_psym(tmp.map.key                + 3, sym_1("MediaBox"));
  tag_init_pstruct_copy_data(tmp.map.value + 3, sym_1("PDF.Rect"), box);
  tag_init_psym(tmp.map.key       + 4, sym_1("Contents"));
  pdf_tag_init_xref(tmp.map.value + 4, content_stream_object_number, 0);
  *dest = tmp;
  return dest;
}
