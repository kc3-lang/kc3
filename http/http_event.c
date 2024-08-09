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
#include "http_event.h"

s32 http_event_add (struct event *ev, s_time *time)
{
  struct timeval tv;
  tv.tv_sec = time->tv_sec;
  tv.tv_usec = time->tv_nsec / 1000;
  return event_add(ev, &tv);
}

/* http_event_callback
     expects a tag of the form
     {fn ((S32) fd, (List) events, (Ptr) ev, (Tag) Arg) {void},
      arg} */
void http_event_callback (int fd, short events, void *tag_tuple)
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
    err_puts("http_event_callback: invalid arg");
    assert(! "http_event_callback: invalid arg");
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
    err_puts("http_event_callback: callback failed");
    assert(! "http_event_callback: callback failed");
    abort();
  }
  tag_clean(&tmp);
}

s32 http_event_base_dispatch (struct event_base *eb)
{
  return event_base_dispatch(eb);
}

struct event_base * http_event_base_new (void)
{
  return event_base_new();
}

struct event * http_event_new (struct event_base *event_base, s32 fd,
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
  tag_init_fn_copy(tag->data.tuple.tag, callback);
  tag_init_copy(tag->data.tuple.tag + 2, arg);
  ev = event_new(event_base, fd, events_s16, http_event_callback, tag);
  tag_init_ptr(tag->data.tuple.tag + 1, ev);
  return ev;
 invalid_event_list:
  err_write_1("http_event_new: invalid event list: ");
  err_inspect_list(events);
  assert(! "http_event_new: invalid event list");
  return NULL;
}

/*
struct event * http_event_new (s32 fd, const s_list *events,
                               const s_fn *fn, s_tag *arg)
{
  static s_list *arg_types = NULL;
  s_list *e;
  struct event *ev;
  ev = alloc(sizeof(*ev));
  if (! ev)
    return NULL;
  if (! arg_types)
    arg_types = list_new_sym(&g_sym_S32, list_new_sym
                             (&g_sym_S16, list_new_sym
                              (&g_sym_Tag, NULL)));
  if (compare_list(arg_types, cfn->arg_types)) {
    err_write_1("http_event_new: invalid argument types for Cfn ");
    err_inspect_str(&cfn->name->str);
    err_write_1(": expected ");
    err_inspect_list((const s_list * const *) &arg_types);
    err_write_1(" got ");
    err_inspect_list((const s_list * const *) &cfn->arg_types);
    assert(! "http_event_new: invalid argument types for Cfn");
    return NULL;
  }
  events_s16 = 0;
  e = events;
  while (e) {
    if (e->tag.type != TAG_SYM) {
      err_write_1("http_event_new: invalid event list: ");
      err_inspect_list(&events);
      assert(! "http_event_new: invalid event list");
      return NULL;
    }
    if (e->tag.data.sym == 
    e = list_next(e);
  }
  event_set(ev, fd, events_s16, (void (*) (int, short, void *)) cfn->ptr.f,
            arg);
  return ev;
}
*/

s_str * http_event_version (s_str *dest)
{
  return str_init_1(dest, NULL, event_get_version());
}
