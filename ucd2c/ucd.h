/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef UCD_H
#define UCD_H

#define UCD_MAX     0x110000
#define UCD_SUB_MAX 0x010000

#define UCD_LETTER                    0x0000000001
#define UCD_LETTER_LOWERCASE          0x0000000002
#define UCD_LETTER_MODIFIER           0x0000000004
#define UCD_LETTER_OTHER              0x0000000008
#define UCD_LETTER_TITLECASE          0x0000000010
#define UCD_LETTER_UPPERCASE          0x0000000020
#define UCD_MARK                      0x0000000040
#define UCD_MARK_COMBINING            0x0000000080
#define UCD_MARK_ENCLOSING            0x0000000100
#define UCD_MARK_NONSPACING           0x0000000200
#define UCD_NUMBER                    0x0000000400
#define UCD_NUMBER_DECIMAL_DIGIT      0x0000000800
#define UCD_NUMBER_LETTER             0x0000001000
#define UCD_NUMBER_OTHER              0x0000002000
#define UCD_PUNCTUATION               0x0000004000
#define UCD_PUNCTUATION_CLOSE         0x0000008000
#define UCD_PUNCTUATION_CONNECTOR     0x0000010000
#define UCD_PUNCTUATION_DASH          0x0000020000
#define UCD_PUNCTUATION_FINAL_QUOTE   0x0000040000
#define UCD_PUNCTUATION_INITIAL_QUOTE 0x0000080000
#define UCD_PUNCTUATION_OPEN          0x0000100000
#define UCD_PUNCTUATION_OTHER         0x0000200000
#define UCD_SYMBOL                    0x0000400000
#define UCD_SYMBOL_CURRENCY           0x0000800000
#define UCD_SYMBOL_MATH               0x0001000000
#define UCD_SYMBOL_MODIFIER           0x0002000000
#define UCD_SYMBOL_OTHER              0x0004000000
#define UCD_SEPARATOR                 0x0008000000
#define UCD_SEPARATOR_LINE            0x0010000000
#define UCD_SEPARATOR_PARAGRAPH       0x0020000000
#define UCD_SEPARATOR_SPACE           0x0040000000
#define UCD_OTHER                     0x0080000000
#define UCD_OTHER_CONTROL             0x0100000000
#define UCD_OTHER_FORMAT              0x0200000000
#define UCD_OTHER_SURROGATE           0x0400000000
#define UCD_OTHER_PRIVATE_USE         0x0800000000
#define UCD_OTHER_NOT_ASSIGNED        0x1000000000

typedef unsigned long long ucd_flags;

typedef struct ucd {
  ucd_flags flags;
  char *name;
} s_ucd;

extern const s_ucd g_ucd[UCD_MAX];

#endif /* UCD_H */
