/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
/**
 * @file serialize.h
 * @brief Serialization from tag to blob.
 *
 * Serialization is transforming memory data to a binary blob.
 */
#ifndef LIBKC3_SERIALIZE_H
#define LIBKC3_SERIALIZE_H

#include "types.h"

#define PROTO_SERIALIZE(type)                                          \
  s_serialize * serialize_ ## type (s_serialize *serialize,            \
                                    type src)

/* Stack-allocation compatible functions, call serialize_clean
   after use. */
void          serialize_clean (s_serialize *serialize);
s_serialize * serialize_init (s_serialize *serialize);

/* Heap-allocation functions, call serialize_delete after use. */
void          serialize_delete (s_serialize *serialize);
s_serialize * serialize_new (void);

/* Operators. */
PROTO_SERIALIZE(bool);
PROTO_SERIALIZE(character);
s_serialize * serialize_list (s_serialize *serialize,
                              const s_list *list);
s_serialize * serialize_tag (s_serialize *serialize,
                             const s_tag *tag);
sw            serialize_to_buf (s_serialize *serialize,
                                s_buf *buf);
s_str *       serialize_to_str (s_serialize *serialize,
                                s_str *dest);
s_serialize * serialize_tuple (s_serialize *serialize,
                               const s_tuple *tuple);
PROTO_SERIALIZE(s8);
PROTO_SERIALIZE(s16);
PROTO_SERIALIZE(s32);
PROTO_SERIALIZE(s64);
s_serialize * serialize_str (s_serialize *serialize, const s_str *str);
PROTO_SERIALIZE(sw);
PROTO_SERIALIZE(u8);
PROTO_SERIALIZE(u16);
PROTO_SERIALIZE(u32);
PROTO_SERIALIZE(u64);
PROTO_SERIALIZE(uw);

#endif /* LIBKC3_SERIALIZE_H */
