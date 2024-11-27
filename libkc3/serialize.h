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

/* Stack-allocation compatible functions, call serialize_clean
   after use. */
void          serialize_clean (s_serialize *serialize);
s_serialize * serialize_init (s_serialize *serialize);

/* Observers. */
s_str * serialize_to_buf (const s_serialize *serialize, s_buf *buf);
s_str * serialize_to_str (const s_serialize *serialize, s_str *dest);

/* Operators. */
s_serialize * serialize_tag (s_serialize *serialize,
                             s_tag *tag);

#endif /* LIBKC3_SERIALIZE_H */
