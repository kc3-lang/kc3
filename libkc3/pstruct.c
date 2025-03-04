
p_struct * pstruct_init_cast (p_struct *s, const s_sym * const *type,
                             const s_tag *tag)
{
  assert(s);
  assert(tag);
  switch (tag->type) {
  case TAG_PSTRUCT:
    if (*type == tag->data.pstruct->type->module)
      return pstruct_init_copy(&s, tag->data.pstruct);
  default:
    break;
  }
  err_write_1("struct_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to %");
  err_inspect_sym(type);
  err_write_1("{}\n");
  assert(! "struct_init_cast: cannot cast to Struct");
  return NULL;
}
