/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
 * All rights reserved.
 */
#include <string.h>
#include "dns.h"
#include "list.h"
#include "tag.h"

#if defined(WIN32) || defined(WIN64)

s_tag * dns_comp (const s_str *name, s_tag *dest)
{
  (void) name;
  return tag_void(dest);
}

s_tag * dns_expand (const s_str *message, uw offset, s_tag *dest)
{
  (void) message;
  (void) offset;
  return tag_void(dest);
}

s32 dns_init (void)
{
  return -1;
}

s_tag * dns_mkquery (s32 op, const s_str *name, s32 class, s32 type,
                     const s_tag *data, const s_tag *newrr, s_tag *dest)
{
  (void) op;
  (void) name;
  (void) class;
  (void) type;
  (void) data;
  (void) newrr;
  return tag_void(dest);
}

s_tag * dns_query (const s_str *name, s32 class, s32 type, s_tag *dest)
{
  (void) name;
  (void) class;
  (void) type;
  return tag_void(dest);
}

s_tag * dns_search (const s_str *name, s32 class, s32 type, s_tag *dest)
{
  (void) name;
  (void) class;
  (void) type;
  return tag_void(dest);
}

s_tag * dns_send (const s_str *message, s_tag *dest)
{
  (void) message;
  return tag_void(dest);
}

s_tag * dns_txt (const s_str *name, s_tag *dest)
{
  (void) name;
  return tag_void(dest);
}

s_tag * dns_txt_packet (const unsigned char *packet, uw size,
                        const s_str *name, s_tag *dest)
{
  (void) packet;
  (void) size;
  (void) name;
  return tag_void(dest);
}

#else

#include <arpa/nameser.h>
#include <limits.h>
#include <netdb.h>
#include <resolv.h>
#include <strings.h>

#define DNS_MESSAGE_MAX 65535

static bool dns_name (const s_str *name, char *dest)
{
  if (! name || ! name->size || name->size >= MAXDNAME ||
      memchr(name->ptr.p_pchar, 0, name->size))
    return false;
  memcpy(dest, name->ptr.p_pchar, name->size);
  dest[name->size] = 0;
  return true;
}

static s_tag * dns_resolve (const s_str *name, s32 class, s32 type,
                            bool search, s_tag *dest)
{
  unsigned char answer[DNS_MESSAGE_MAX];
  char hostname[MAXDNAME];
  int size;
  if (! dns_name(name, hostname))
    return tag_void(dest);
  size = search ?
    res_search(hostname, class, type, answer, sizeof(answer)) :
    res_query(hostname, class, type, answer, sizeof(answer));
  if (size < 0 || (size_t) size > sizeof(answer))
    return tag_void(dest);
  return tag_init_str_alloc_copy(dest, size, (const char *) answer);
}

static unsigned int dns_u16 (const unsigned char *p)
{
  return (unsigned int) p[0] << 8 | p[1];
}

s_tag * dns_comp (const s_str *name, s_tag *dest)
{
  unsigned char compressed[MAXDNAME];
  char hostname[MAXDNAME];
  int size;
  if (! dns_name(name, hostname))
    return tag_void(dest);
  size = dn_comp(hostname, compressed, sizeof(compressed), NULL, NULL);
  if (size < 0)
    return tag_void(dest);
  return tag_init_str_alloc_copy(dest, size, (const char *) compressed);
}

s_tag * dns_expand (const s_str *message, uw offset, s_tag *dest)
{
  char name[MAXDNAME];
  int size;
  if (! message || offset >= message->size)
    return tag_void(dest);
  size = dn_expand(message->ptr.p_pu8,
                   message->ptr.p_pu8 + message->size,
                   message->ptr.p_pu8 + offset, name, sizeof(name));
  if (size < 0 || ! tag_init_ptuple(dest, 2) ||
      ! tag_init_str_alloc_copy(dest->data.td_ptuple->tag,
                                strlen(name), name) ||
      ! tag_init_s32(dest->data.td_ptuple->tag + 1, size))
    return tag_void(dest);
  return dest;
}

s32 dns_init (void)
{
  return res_init();
}

s_tag * dns_mkquery (s32 op, const s_str *name, s32 class, s32 type,
                     const s_tag *data, const s_tag *newrr, s_tag *dest)
{
  unsigned char message[DNS_MESSAGE_MAX];
  char hostname[MAXDNAME];
  const s_str *data_str;
  const s_str *newrr_str;
  int size;
  if (! data || ! newrr || ! dns_name(name, hostname))
    return tag_void(dest);
  data_str = data->type == TAG_STR ? &data->data.td_str : NULL;
  newrr_str = newrr->type == TAG_STR ? &newrr->data.td_str : NULL;
  if ((data->type != TAG_VOID && ! data_str) ||
      (newrr->type != TAG_VOID && ! newrr_str) ||
      (data_str && data_str->size > INT_MAX))
    return tag_void(dest);
  size = res_mkquery(op, hostname, class, type,
                     data_str ? data_str->ptr.p_pu8 : NULL,
                     data_str ? data_str->size : 0,
                     newrr_str ? newrr_str->ptr.p_pu8 : NULL,
                     message, sizeof(message));
  if (size < 0)
    return tag_void(dest);
  return tag_init_str_alloc_copy(dest, size, (const char *) message);
}

s_tag * dns_query (const s_str *name, s32 class, s32 type, s_tag *dest)
{
  return dns_resolve(name, class, type, false, dest);
}

s_tag * dns_search (const s_str *name, s32 class, s32 type, s_tag *dest)
{
  return dns_resolve(name, class, type, true, dest);
}

s_tag * dns_send (const s_str *message, s_tag *dest)
{
  unsigned char answer[DNS_MESSAGE_MAX];
  int size;
  if (! message || ! message->size || message->size > INT_MAX)
    return tag_void(dest);
  size = res_send(message->ptr.p_pu8, message->size,
                  answer, sizeof(answer));
  if (size < 0 || (size_t) size > sizeof(answer))
    return tag_void(dest);
  return tag_init_str_alloc_copy(dest, size, (const char *) answer);
}

s_tag * dns_txt (const s_str *name, s_tag *dest)
{
  unsigned char answer[4096];
  char hostname[MAXDNAME];
  int error;
  int size;
  if (! dns_name(name, hostname))
    return tag_void(dest);
  size = res_query(hostname, C_IN, T_TXT, answer, sizeof(answer));
  error = h_errno;
  if (size < 0)
    return error == HOST_NOT_FOUND || error == NO_DATA ?
      tag_init_plist(dest, NULL) : tag_void(dest);
  if ((size_t) size > sizeof(answer))
    return tag_void(dest);
  return dns_txt_packet(answer, size, name, dest);
}

s_tag * dns_txt_packet (const unsigned char *packet, uw size,
                        const s_str *name, s_tag *dest)
{
  char owner[MAXDNAME];
  const unsigned char *end;
  const unsigned char *p;
  unsigned int answers;
  unsigned int i;
  int consumed;
  s_list *list = NULL;
  s_list **tail = &list;
  if (! packet || ! name || size < HFIXEDSZ)
    return tag_void(dest);
  end = packet + size;
  p = packet + HFIXEDSZ;
  /* QR must be set; opcode and TC must be zero. */
  if (! (packet[2] & 0x80) || (packet[2] & 0x7a) ||
      ((packet[3] & 0x0f) != NOERROR &&
       (packet[3] & 0x0f) != NXDOMAIN) ||
      dns_u16(packet + 4) != 1)
    goto error;
  consumed = dn_expand(packet, end, p, owner, sizeof(owner));
  if (consumed < 0 || (uw) (end - p) < (uw) consumed + QFIXEDSZ ||
      strlen(owner) != name->size ||
      strncasecmp(owner, name->ptr.p_pchar, name->size) ||
      dns_u16(p + consumed) != T_TXT ||
      dns_u16(p + consumed + 2) != C_IN)
    goto error;
  if ((packet[3] & 0x0f) == NXDOMAIN)
    return tag_init_plist(dest, NULL);
  p += consumed + QFIXEDSZ;
  answers = dns_u16(packet + 6);
  for (i = 0; i < answers; i++) {
    unsigned int type;
    unsigned int klass;
    unsigned int length;
    const unsigned char *record_end;
    consumed = dn_expand(packet, end, p, owner, sizeof(owner));
    if (consumed < 0 || (uw) (end - p) < (uw) consumed + RRFIXEDSZ)
      goto error;
    p += consumed;
    type = dns_u16(p);
    klass = dns_u16(p + 2);
    length = dns_u16(p + 8);
    p += RRFIXEDSZ;
    if ((uw) (end - p) < length)
      goto error;
    record_end = p + length;
    if (type == T_TXT && klass == C_IN &&
        strlen(owner) == name->size &&
        ! strncasecmp(owner, name->ptr.p_pchar, name->size)) {
      const unsigned char *q = p;
      uw value_size = 0;
      uw offset = 0;
      while (q < record_end) {
        unsigned int chunk = *q++;
        if ((uw) (record_end - q) < chunk)
          goto error;
        value_size += chunk;
        q += chunk;
      }
      if (! (*tail = list_new_str_alloc(value_size, NULL)))
        goto error;
      q = p;
      while (q < record_end) {
        unsigned int chunk = *q++;
        memcpy((*tail)->tag.data.td_str.free.p_pchar + offset, q, chunk);
        offset += chunk;
        q += chunk;
      }
      tail = &(*tail)->next.data.td_plist;
    }
    p = record_end;
  }
  return tag_init_plist(dest, list);
 error:
  list_delete_all(list);
  return tag_void(dest);
}

#endif
