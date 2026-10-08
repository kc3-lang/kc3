/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
 * All rights reserved.
 */
#ifndef LIBKC3_DNS_H
#define LIBKC3_DNS_H

#include "types.h"

s_tag * dns_comp (const s_str *name, s_tag *dest);
s32     dns_dn_comp (const char *name, unsigned char *dest, s32 size,
                     unsigned char **pointers, unsigned char **last);
s_tag * dns_expand (const s_str *message, uw offset, s_tag *dest);
s32     dns_init (void);
s_tag * dns_mkquery (s32 op, const s_str *name, s32 class, s32 type,
                     const s_tag *data, const s_tag *newrr, s_tag *dest);
s_tag * dns_query (const s_str *name, s32 class, s32 type, s_tag *dest);
s32     dns_res_send (const unsigned char *message, s32 size,
                      unsigned char *answer, s32 capacity);
s_tag * dns_search (const s_str *name, s32 class, s32 type, s_tag *dest);
s_tag * dns_send (const s_str *message, s_tag *dest);

/* A List of decoded TXT values, or Void on resolver/packet error. */
s_tag * dns_txt (const s_str *name, s_tag *dest);
s_tag * dns_txt_packet (const unsigned char *packet, uw size,
                        const s_str *name, s_tag *dest);

#endif
