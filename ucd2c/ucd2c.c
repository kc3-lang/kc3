/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ucd.h"

#define BUFSZ 1024

unsigned long read_hex (const char **src) {
  unsigned long result = 0;
  char c;
  while ((c = **src) != '\0' && c != ';') {
    result *= 16;
    if ('0' <= c && c <= '9')
      result += c - '0';
    else if ('A' <= c && c <= 'F')
      result += c - 'A' + 10;
    else if ('a' <= c && c <= 'f')
      result += c - 'a' + 10;
    else {
      fprintf(stderr, "invalid character in index: %c", c);
      exit(1);
    }
    (*src)++;
  }
  if (c == ';')
    (*src)++;
  return result;
}

void ucd_case (s_ucd ucd[UCD_MAX])
{
  char *capital;
  unsigned long i;
  unsigned long j;
  char name[1024];
  for (i = 0; i < UCD_MAX; i++) {
    if (ucd[i].name &&
        (capital = strstr(ucd[i].name, " CAPITAL "))) {
      strlcpy(name, ucd[i].name, (capital - ucd[i].name) + 1);
      strlcat(name, " SMALL ", sizeof(name));
      strlcat(name, capital + strlen(" CAPITAL "), sizeof(name));
      for (j = 0; j < UCD_MAX; j++) {
        if (ucd[j].name &&
            strcmp(ucd[j].name, name) == 0) {
          ucd[i].to_lower = j;
          ucd[j].to_upper = i;
        }
      }
    }
  }
}

void ucd_parse (s_ucd ucd[UCD_MAX], char *line,
                unsigned long lineno) {
  unsigned long i;
  char *p = NULL;
  char *sep;
  size_t size;
#define UCD_PARSE_UNKNOWN_CATEGORY \
  fprintf(stderr, "line %lu: unknown category : %c%c", lineno, p[0], p[1])
  p = line;
  i = read_hex((const char **) &p);
  if (i >= UCD_MAX) {
    fprintf(stderr, "%lu > UCD_MAX", i);
    goto error;
  }
  ucd[i].flags = 0;
  if (! (sep = strchr(p, ';')))
    goto error;
  *sep = '\0';
  size = strlen(p) + 1;
  ucd[i].name = malloc(size);
  strlcpy(ucd[i].name, p, size);
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
    default:
      fprintf(stderr, "line %lu: unknown category: '%c'", lineno, *p);
    }
    if (*p != ';')
      p++;
    if (*p != ';')
      p++;
  }
  return;
 error:
  fprintf(stderr, "invalid entry line %lu: %s", lineno, line);
}

void ucd_write_c (s_ucd ucd[UCD_MAX])
{
  unsigned long i = 0;
  printf("/* c3\n"
         " * Copyright 2022 kmx.io <contact@kmx.io>\n"
         " *\n"
         " * Permission is hereby granted to use this software granted\n"
         " * the above copyright notice and this permission paragraph\n"
         " * are included in all copies and substantial portions of this\n"
         " * software.\n"
         " *\n"
         " * THIS SOFTWARE IS PROVIDED \"AS-IS\" WITHOUT ANY GUARANTEE OF\n"
         " * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE\n"
         " * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF\n"
         " * THIS SOFTWARE.\n"
         " */\n");
  printf("/* generated by ucd2c */\n");
  printf("#include \"ucd.h\"\n");
  printf("const s_ucd g_ucd[UCD_MAX] = {\n");
  while (i < UCD_MAX) {
    if (i > 0)
      printf(",\n");
    printf("  {0x%llx, ",
           ucd[i].flags);
    if (ucd[i].name)
      printf("\"%s\"", ucd[i].name);
    else
      printf("0");
    printf(", ");
    printf("0x%lx", ucd[i].to_lower);
    printf(", ");
    printf("0x%lx", ucd[i].to_upper);
    printf("}");
    i++;
  }
  printf("\n};\n");
}

int main (int argc, char **argv)
{
  char buf[BUFSZ];
  unsigned long lineno;
  s_ucd *ucd = calloc(UCD_MAX, sizeof(s_ucd));
  (void) argc;
  (void) argv;
  lineno = 0;
  while (fgets(buf, BUFSZ, stdin)) {
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
      buf[len - 1] = 0;
    ucd_parse(ucd, buf, ++lineno);
  }
  ucd_case(ucd);
  ucd_write_c(ucd);
  free(ucd);
  return 0;
}
