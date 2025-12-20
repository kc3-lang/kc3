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
#ifndef LIBKC3_ENDIAN_H
#define LIBKC3_ENDIAN_H

#ifdef htobe16
# undef htobe16
# undef htole16
# undef be16toh
# undef le16toh
# undef htobe32
# undef htole32
# undef be32toh
# undef le32toh
# undef htobe64
# undef htole64
# undef be64toh
# undef le64toh
# undef htobe128
# undef htole128
# undef be128toh
# undef le128toh
#endif

#ifdef htons
# undef htons
# undef ntohs
# undef htonl
# undef ntohl
#endif

#if ! defined(__APPLE__)
# define BSWAP(b, x) __builtin_bswap ## b (x)
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define htobe16(x) BSWAP(16, x)
#  define htole16(x) (x)
#  define be16toh(x) BSWAP(16, x)
#  define le16toh(x) (x)
#  define htobe32(x) BSWAP(32, x)
#  define htole32(x) (x)
#  define be32toh(x) BSWAP(32, x)
#  define le32toh(x) (x)
#  define htobe64(x) BSWAP(64, x)
#  define htole64(x) (x)
#  define be64toh(x) BSWAP(64, x)
#  define le64toh(x) (x)
#  define htobe128(x) BSWAP(128, x)
#  define htole128(x) (x)
#  define be128toh(x) BSWAP(128, x)
#  define le128toh(x) (x)
#  if ! (defined(WIN32) || defined(WIN64))
#   define htons(x)   BSWAP(16, x)
#   define ntohs(x)   BSWAP(16, x)
#   define htonl(x)   BSWAP(32, x)
#   define ntohl(x)   BSWAP(32, x)
#  endif
# else
#  define htobe16(x) (x)
#  define htole16(x) BSWAP(16, x)
#  define be16toh(x) (x)
#  define le16toh(x) BSWAP(16, x)
#  define htobe32(x) (x)
#  define htole32(x) BSWAP(32, x)
#  define be32toh(x) (x)
#  define le32toh(x) BSWAP(32, x)
#  define htobe64(x) (x)
#  define htole64(x) BSWAP(64, x)
#  define be64toh(x) (x)
#  define le64toh(x) BSWAP(64, x)
#  define htobe128(x) (x)
#  define htole128(x) BSWAP(128, x)
#  define be128toh(x) (x)
#  define le128toh(x) BSWAP(128, x)
#  if ! (defined(WIN32) || defined(WIN64))
#   define htons(x)   (x)
#   define ntohs(x)   (x)
#   define htonl(x)   (x)
#   define ntohl(x)   (x)
#  endif
# endif
#else
# include <libkern/OSByteOrder.h>
# define htobe16(x) OSSwapHostToBigInt16(x)
# define htons(x) OSSwapHostToBigInt16(x)
# define htole16(x) OSSwapHostToLittleInt16(x)
# define be16toh(x) OSSwapBigToHostInt16(x)
# define ntohs(x) OSSwapBigToHostInt16(x)
# define le16toh(x) OSSwapLittleToHostInt16(x)
# define htobe32(x) OSSwapHostToBigInt32(x)
# define htole32(x) OSSwapHostToLittleInt32(x)
# define be32toh(x) OSSwapBigToHostInt32(x)
# define le32toh(x) OSSwapLittleToHostInt32(x)
# define htobe64(x) OSSwapHostToBigInt64(x)
# define htole64(x) OSSwapHostToLittleInt64(x)
# define be64toh(x) OSSwapBigToHostInt64(x)
# define le64toh(x) OSSwapLittleToHostInt64(x)
# define htobe128(x) OSSwapHostToBigInt128(x)
# define htole128(x) OSSwapHostToLittleInt128(x)
# define be128toh(x) OSSwapBigToHostInt128(x)
# define le128toh(x) OSSwapLittleToHostInt128(x)
#endif

#endif /* LIBKC3_ENDIAN_H */
