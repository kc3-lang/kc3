/* c3
 * Copyright 2022 Thomas de Grivel <thodg@kmx.io>
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ucd.h"

#define BUFSZ 1024

unsigned long read_hex (const char **src) {
  unsigned long ul = 0;
  char c;
  while ((c = **src) != '\0' && c != ';') {
    if ('0' <= c && c <= '9') {
      ul *= 16;
      ul += c - '0';
      (*src)++;
    }
    else if ('A' <= c && c <= 'F') {
      ul *= 16;
      ul += c - 'A' + 10;
      (*src)++;
    }
    else if ('a' <= c && c <= 'f') {
      ul *= 16;
      ul += c - 'a' + 10;
      (*src)++;
    }
    else {
      errx(1, "invalid character in index: %c", c);
    }
  }
  if (c == ';')
    (*src)++;
  return ul;
}

void ucd_parse (s_ucd ucd[UCD_MAX], char *line,
                unsigned long lineno) {
  unsigned long i;
  char *p = NULL;
  char *sep;
#define UCD_PARSE_UNKNOWN_CATEGORY \
  warnx("line %lu: unknown category : %c%c", lineno, p[0], p[1])
  p = line;
  i = read_hex((const char **) &p);
  if (i >= UCD_MAX) {
    warnx("%lu > UCD_MAX", i);
    goto error;
  }
  ucd[i].flags = 0;
  if (! (sep = strchr(p, ';')))
    goto error;
  *sep = '\0';
  ucd[i].name = strdup(p);
  p = sep + 1;
  if (! (sep = strchr(p, ';')))
    goto error;
  while (p < sep) {
    switch (p[0]) {
    case 'L':
      ucd[i].flags |= UCD_LETTER;
      switch (p[1]) {
      case 'l': ucd[i].flags |= UCD_LETTER_LOWERCASE; break;
      case 'm': ucd[i].flags |= UCD_LETTER_MODIFIER; break;
      case 'o': ucd[i].flags |= UCD_LETTER_OTHER; break;
      case 't': ucd[i].flags |= UCD_LETTER_TITLECASE; break;
      case 'u': ucd[i].flags |= UCD_LETTER_UPPERCASE; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'M':
      ucd[i].flags |= UCD_MARK;
      switch (p[1]) {
      case 'c': ucd[i].flags |= UCD_MARK_COMBINING; break;
      case 'e': ucd[i].flags |= UCD_MARK_ENCLOSING; break;
      case 'n': ucd[i].flags |= UCD_MARK_NONSPACING; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'N':
      ucd[i].flags |= UCD_NUMBER;
      switch (p[1]) {
      case 'd': ucd[i].flags |= UCD_NUMBER_DECIMAL_DIGIT; break;
      case 'l': ucd[i].flags |= UCD_NUMBER_LETTER; break;
      case 'o': ucd[i].flags |= UCD_NUMBER_OTHER; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'P':
      ucd[i].flags |= UCD_PUNCTUATION;
      switch (p[1]) {
      case 'c': ucd[i].flags |= UCD_PUNCTUATION_CONNECTOR; break;
      case 'd': ucd[i].flags |= UCD_PUNCTUATION_DASH; break;
      case 's': ucd[i].flags |= UCD_PUNCTUATION_OPEN; break;
      case 'e': ucd[i].flags |= UCD_PUNCTUATION_CLOSE; break;
      case 'i': ucd[i].flags |= UCD_PUNCTUATION_INITIAL_QUOTE; break;
      case 'f': ucd[i].flags |= UCD_PUNCTUATION_FINAL_QUOTE; break;
      case 'o': ucd[i].flags |= UCD_PUNCTUATION_OTHER; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'S':
      ucd[i].flags |= UCD_SYMBOL;
      switch (p[1]) {
      case 'c': ucd[i].flags |= UCD_SYMBOL_CURRENCY; break;
      case 'k': ucd[i].flags |= UCD_SYMBOL_MODIFIER; break;
      case 'm': ucd[i].flags |= UCD_SYMBOL_MATH; break;
      case 'o': ucd[i].flags |= UCD_SYMBOL_OTHER; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'Z':
      ucd[i].flags |= UCD_SEPARATOR;
      switch (p[1]) {
      case 'l': ucd[i].flags |= UCD_SEPARATOR_LINE; break;
      case 'p': ucd[i].flags |= UCD_SEPARATOR_PARAGRAPH; break;
      case 's': ucd[i].flags |= UCD_SEPARATOR_SPACE; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    case 'C':
      ucd[i].flags |= UCD_OTHER;
      switch (p[1]) {
      case 'c': ucd[i].flags |= UCD_OTHER_CONTROL; break;
      case 'f': ucd[i].flags |= UCD_OTHER_FORMAT; break;
      case 's': ucd[i].flags |= UCD_OTHER_SURROGATE; break;
      case 'o': ucd[i].flags |= UCD_OTHER_PRIVATE_USE; break;
      case 'n': ucd[i].flags |= UCD_OTHER_NOT_ASSIGNED; break;
      default: UCD_PARSE_UNKNOWN_CATEGORY;
      }
      break;
    default: warnx("line %lu: unknown category: '%c'", lineno, *p);
    }
    if (*p != ';')
      p++;
    if (*p != ';')
      p++;
  }
  return;
 error:
  warnx("invalid entry line %lu: %s", lineno, line);
}

void ucd_write_c (s_ucd ucd[UCD_MAX])
{
  unsigned long i = 0;
  printf("/* generated by ucd2c */\n");
  printf("#include \"ucd.h\"\n");
  printf("const s_ucd g_ucd[UCD_MAX] = {\n");
  while (i < UCD_MAX) {
    if (i > 0)
      printf(",\n");
    printf("  {0x%lx, ",
           ucd[i].flags);
    if (ucd[i].name)
      printf("\"%s\"", ucd[i].name);
    else
      printf("0");
    printf("}");
    i++;
  }
  printf("\n};\n");
}

int main (int argc, char **argv)
{
  char buf[BUFSZ];
  unsigned long lineno;
  s_ucd *ucd = calloc(sizeof(s_ucd), UCD_MAX);
  (void) argc;
  (void) argv;
  lineno = 0;
  while (fgets(buf, BUFSZ, stdin)) {
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
      buf[len - 1] = 0;
    ucd_parse(ucd, buf, ++lineno);
  }
  ucd_write_c(ucd);
  free(ucd);
  return 0;
}
