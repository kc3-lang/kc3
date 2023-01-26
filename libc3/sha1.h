#ifndef LIBC3_SHA1_H
#define LIBC3_SHA1_H

#if HAVE_SHA1_H
# include <sha1.h>
#else
# if HAVE_SHA_H
#  include <sha.h>
# endif
#endif

#if HAVE_SHA_H
#define SHA1_DIGEST_LENGTH SHA_DIGEST_LENGTH
#define SHA1Final SHA1_Final
#define SHA1Init SHA1_Init
#define SHA1Update SHA1_Update
#endif

#endif /* LIBC3_SHA1_H */
