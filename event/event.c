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
#include <event2/event.h>
#include <libkc3/kc3.h>
#include <signal.h>
#include "event.h"

s32 kc3_event_add (struct event **ev, s_time *time)
{
  struct timeval tv;
  if (! time->tv_sec && ! time->tv_nsec)
    return event_add(*ev, NULL);
  tv.tv_sec = time->tv_sec;
  tv.tv_usec = time->tv_nsec / 1000;
  return event_add(*ev, &tv);
}

s32 kc3_event_del (struct event **ev)
{
  return event_del(*ev);
}

/* kc3_event_callback
     expects a tag of the form
     {fn ((S32) fd, (List) events, (Ptr) ev, (Tag) Arg) {void},
      arg} */
void kc3_event_callback (int fd, short events, void *tag_tuple)
{
  s_tag  *arg;
  s_list *arguments;
  struct event *ev;
  s_list       *events_list;
  s_fn *fn;
  s_tag *tag;
  s_tag tmp;
  tag = tag_tuple;
  if (tag->type != TAG_TUPLE ||
      tag->data.tuple.count != 3 ||
      tag->data.tuple.tag[0].type != TAG_FN ||
      tag->data.tuple.tag[1].type != TAG_PTR) {
    err_puts("kc3_event_callback: invalid arg");
    assert(! "kc3_event_callback: invalid arg");
    abort();
  }
  fn = &tag->data.tuple.tag[0].data.fn;
  ev = tag->data.tuple.tag[1].data.ptr.p;
  arg = tag->data.tuple.tag + 2;
  events_list = NULL;
  if (events & EV_PERSIST)
    events_list = list_new_sym(&g_sym_persist, events_list);
  if (events & EV_WRITE)
    events_list = list_new_sym(&g_sym_write, events_list);
  if (events & EV_READ)
    events_list = list_new_sym(&g_sym_read, events_list);
  if (events & EV_SIGNAL)
    events_list = list_new_sym(&g_sym_signal, events_list);
  if (events & EV_TIMEOUT)
    events_list = list_new_sym(&g_sym_timeout, events_list);
  arguments = list_new_s32(fd, list_new_list
                           (events_list, list_new_ptr
                            (ev, list_new_tag_copy
                             (arg, NULL))));
  if (! env_eval_call_fn_args(&g_kc3_env, fn, arguments, &tmp)) {
    err_puts("kc3_event_callback: callback failed");
    assert(! "kc3_event_callback: callback failed");
    abort();
  }
  tag_clean(&tmp);
}

s32 kc3_event_base_dispatch (struct event_base **eb)
{
  return event_base_dispatch(*eb);
}

struct event_base * kc3_event_base_new (void)
{
  return event_base_new();
}

struct event * kc3_event_new (struct event_base **event_base, s32 fd,
                               const s_list * const *events,
                               const s_fn *callback, s_tag *arg)
{
  const s_list *e;
  struct event *ev;
  s16 events_s16;
  s_tag *tag;
  events_s16 = 0;
  e = *events;
  while (e) {
    if (e->tag.type != TAG_SYM)
      goto invalid_event_list;
    if (e->tag.data.sym == &g_sym_read)
      events_s16 |= EV_READ;
    else if (e->tag.data.sym == &g_sym_write)
      events_s16 |= EV_WRITE;
    else if (e->tag.data.sym == &g_sym_persist)
      events_s16 |= EV_PERSIST;
    else
      goto invalid_event_list;
    e = list_next(e);
  }
  tag = tag_new_tuple(3);
  ev = event_new(*event_base, fd, events_s16, kc3_event_callback, tag);
  if (! ev) {
    tag_delete(tag);
    err_puts("kc3_event_new: event_new");
    assert(! "kc3_event_new: event_new");
    return NULL;
  }
  tag_init_fn_copy(tag->data.tuple.tag, callback);
  tag_init_copy(tag->data.tuple.tag + 2, arg);
  tag_init_ptr(tag->data.tuple.tag + 1, ev);
  return ev;
 invalid_event_list:
  err_write_1("kc3_event_new: invalid event list: ");
  err_inspect_list(events);
  assert(! "kc3_event_new: invalid event list");
  return NULL;
}

s_str * kc3_event_version (s_str *dest)
{
  return str_init_1(dest, NULL, event_get_version());
}
