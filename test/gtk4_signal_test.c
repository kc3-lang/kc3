#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

#if HAVE_GTK4

#include "../gtk4/signal.h"

static GObject *g_signal_object;
static s_tag g_signal_callback_tag;
static s_callable *g_signal_callable;
static s_tag g_signal_data;
static s_tuple *g_signal_tuple;
static uw g_signal_live;
static uw g_signal_names;
static uw g_signal_calls;
static uw g_signal_live_during_disconnect;
static u8 g_signal_fail;
static bool g_signal_args_ok;
static bool g_signal_disconnect;
static gulong g_signal_handler;
static guint g_signal_ping;
static guint g_signal_detail;
static guint g_signal_parameter;
static guint g_signal_result;

static s_tuple * signal_tuple_new (uw count)
{
  s_tuple *tuple;
  if (g_signal_fail == 1)
    return NULL;
  if ((tuple = tuple_new(count))) {
    g_signal_tuple = tuple;
    g_signal_live++;
  }
  return tuple;
}

static void signal_tuple_delete (s_tuple *tuple)
{
  g_signal_live--;
  if (tuple == g_signal_tuple)
    g_signal_tuple = NULL;
  tuple_delete(tuple);
}

static p_callable * signal_callable_copy (p_callable *dest,
                                          p_callable *src)
{
  if (g_signal_fail == 2)
    return NULL;
  return pcallable_init_copy(dest, src);
}

static s_tag * signal_tag_copy (s_tag *dest, s_tag *src)
{
  if (g_signal_fail == 3)
    return NULL;
  return tag_init_copy(dest, src);
}

static s_str * signal_name_copy (s_str *dest, const s_str *src)
{
  if (g_signal_fail == 4 || ! str_init_copy(dest, src))
    return NULL;
  g_signal_names++;
  return dest;
}

static void signal_name_clean (s_str *str)
{
  g_signal_names--;
  str_clean(str);
}

static bool signal_eval (s_callable *callable, s_list *args, s_tag *dest)
{
  s_list *data;
  g_signal_calls++;
  g_signal_args_ok = false;
  if (! args || args->tag.type != TAG_PTR ||
      args->tag.data.td_ptr.p_pvoid != g_signal_object ||
      ! (data = list_next(args)) || list_next(data) ||
      data->tag.type != TAG_STR || data->tag.data.td_str.size != 7 ||
      memcmp(data->tag.data.td_str.ptr.p_pchar, "payload", 7))
    return false;
  g_signal_args_ok = true;
  if (g_signal_disconnect) {
    g_signal_handler_disconnect(g_signal_object, g_signal_handler);
    g_signal_live_during_disconnect = g_signal_live;
  }
  return eval_callable_call(callable, args, dest);
}

#define tuple_new signal_tuple_new
#define tuple_delete signal_tuple_delete
#define pcallable_init_copy signal_callable_copy
#define tag_init_copy signal_tag_copy
#define str_init_copy signal_name_copy
#define str_clean signal_name_clean
#define eval_callable_call signal_eval
#define kc3_gtk4_signal_callback signal_test_callback
#define kc3_gtk4_signal_connect signal_test_connect

#include "../gtk4/signal.c"

#undef tuple_new
#undef tuple_delete
#undef pcallable_init_copy
#undef tag_init_copy
#undef str_init_copy
#undef str_clean
#undef eval_callable_call
#undef kc3_gtk4_signal_callback
#undef kc3_gtk4_signal_connect

static char signal_setup (bool copy)
{
  GType type;
  env_global()->pass_by_copy = copy;
  g_log_set_always_fatal(G_LOG_FATAL_MASK | G_LOG_LEVEL_CRITICAL |
                         G_LOG_LEVEL_WARNING);
  type = g_type_register_static_simple(G_TYPE_OBJECT, "KC3SignalTest",
                                       sizeof(GObjectClass), NULL,
                                       sizeof(GObject), NULL, 0);
  TEST_ASSERT(type);
  g_signal_ping = g_signal_new("ping", type, G_SIGNAL_RUN_LAST, 0,
                               NULL, NULL, NULL, G_TYPE_NONE, 0);
  g_signal_detail = g_signal_new("detail", type,
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                                 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
  g_signal_parameter = g_signal_new("parameter", type, G_SIGNAL_RUN_LAST,
                                    0, NULL, NULL, NULL, G_TYPE_NONE,
                                    1, G_TYPE_INT);
  g_signal_result = g_signal_new("result", type, G_SIGNAL_RUN_LAST, 0,
                                 NULL, NULL, NULL, G_TYPE_BOOLEAN, 0);
  TEST_ASSERT((g_signal_object = g_object_new(type, NULL)));
  TEST_EQ(tag_init_1(&g_signal_callback_tag,
                     "fn (object, data) { data }"),
           &g_signal_callback_tag);
  TEST_EQ(g_signal_callback_tag.type, TAG_PCALLABLE);
  g_signal_callable = g_signal_callback_tag.data.td_pcallable;
  TEST_EQ(tag_init_str_1(&g_signal_data, NULL, "payload"),
           &g_signal_data);
  return 0;
}

static void signal_connect (const char *name)
{
  s_str str;
  str_init_1(&str, NULL, name);
  signal_test_connect(&g_signal_object, &str, &g_signal_callable,
                       &g_signal_data);
}

static char signal_finish (void)
{
  if (g_signal_object) {
    g_object_unref(g_signal_object);
    g_signal_object = NULL;
  }
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(g_signal_names, 0);
  TEST_EQ(g_signal_tuple, NULL);
  TEST_EQ(g_signal_callable->ref_count, 1);
  tag_clean(&g_signal_callback_tag);
  tag_clean(&g_signal_data);
  return 0;
}

static char signal_supported (void)
{
  TEST_EQ(signal_setup(false), 0);
  signal_connect("ping");
  TEST_EQ(g_signal_live, 1);
  TEST_EQ(g_signal_callable->ref_count, 2);
  TEST_ASSERT((g_signal_handler = g_signal_handler_find
                (g_signal_object, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL,
                 g_signal_tuple)));
  tag_clean(&g_signal_data);
  tag_init(&g_signal_data);
  g_signal_emit(g_signal_object, g_signal_ping, 0);
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(g_signal_args_ok, true);
  g_signal_handler_disconnect(g_signal_object, g_signal_handler);
  TEST_EQ(g_signal_live, 0);
  g_signal_emit(g_signal_object, g_signal_ping, 0);
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_detailed (void)
{
  TEST_EQ(signal_setup(false), 0);
  signal_connect("detail::chosen");
  g_signal_emit(g_signal_object, g_signal_detail,
                 g_quark_from_static_string("other"));
  TEST_EQ(g_signal_calls, 0);
  g_signal_emit(g_signal_object, g_signal_detail,
                 g_quark_from_static_string("chosen"));
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(g_signal_args_ok, true);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_notify (void)
{
  GParamSpec *property;
  TEST_EQ(signal_setup(false), 0);
  property = g_param_spec_ref_sink
    (g_param_spec_int("value", "value", "value", 0, 100, 0,
                       G_PARAM_READWRITE));
  signal_connect("notify::value");
  g_signal_emit_by_name(g_signal_object, "notify::value", property);
  TEST_EQ(g_signal_calls, 0);
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(g_signal_has_handler_pending
           (g_signal_object, g_signal_lookup("notify", G_TYPE_OBJECT),
            0, TRUE), false);
  g_param_spec_unref(property);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_parameter (void)
{
  TEST_EQ(signal_setup(false), 0);
  signal_connect("parameter");
  g_signal_emit(g_signal_object, g_signal_parameter, 0, 37);
  TEST_EQ(g_signal_calls, 0);
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_result (void)
{
  gboolean result = TRUE;
  TEST_EQ(signal_setup(false), 0);
  signal_connect("result");
  g_signal_emit(g_signal_object, g_signal_result, 0, &result);
  TEST_EQ(g_signal_calls, 0);
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(result, FALSE);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_names (void)
{
  s_str name;
  const char embedded[] = "ping\0ignored";
  const char bounded[] = {'p', 'i', 'n', 'g', 'X', 0};
  TEST_EQ(signal_setup(false), 0);
  signal_connect("unknown");
  signal_connect("ping::invalid");
  signal_connect("");
  str_init(&name, NULL, sizeof(embedded) - 1, embedded);
  signal_test_connect(&g_signal_object, &name, &g_signal_callable,
                       &g_signal_data);
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(g_signal_callable->ref_count, 1);
  str_init(&name, NULL, 4, bounded);
  signal_test_connect(&g_signal_object, &name, &g_signal_callable,
                       &g_signal_data);
  g_signal_emit(g_signal_object, g_signal_ping, 0);
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(g_signal_args_ok, true);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_copy_failures (void)
{
  u8 i;
  TEST_EQ(signal_setup(false), 0);
  for (i = 1; i <= 4; i++) {
    g_signal_fail = i;
    signal_connect("ping");
    TEST_EQ(g_signal_live, 0);
    TEST_EQ(g_signal_names, 0);
    TEST_EQ(g_signal_callable->ref_count, 1);
    TEST_EQ(g_signal_has_handler_pending
             (g_signal_object, g_signal_ping, 0, TRUE), false);
  }
  g_signal_fail = 0;
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_arguments (void)
{
  GObject *missing = NULL;
  s_callable empty = {0};
  s_callable *callback = &empty;
  s_str name = STR("ping");
  TEST_EQ(signal_setup(false), 0);
  signal_test_connect(NULL, &name, &g_signal_callable, &g_signal_data);
  signal_test_connect(&missing, &name, &g_signal_callable,
                       &g_signal_data);
  signal_test_connect(&g_signal_object, NULL, &g_signal_callable,
                       &g_signal_data);
  signal_test_connect(&g_signal_object, &name, NULL, &g_signal_data);
  signal_test_connect(&g_signal_object, &name, &callback,
                       &g_signal_data);
  callback = NULL;
  signal_test_connect(&g_signal_object, &name, &callback,
                       &g_signal_data);
  signal_test_connect(&g_signal_object, &name, &g_signal_callable, NULL);
  name.ptr.p_pchar = NULL;
  signal_test_connect(&g_signal_object, &name, &g_signal_callable,
                       &g_signal_data);
  name = (s_str) STR("ping");
  name.size = S32_MAX;
  signal_test_connect(&g_signal_object, &name, &g_signal_callable,
                       &g_signal_data);
  TEST_EQ(g_signal_has_handler_pending
           (g_signal_object, g_signal_ping, 0, TRUE), false);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_destroy (void)
{
  TEST_EQ(signal_setup(false), 0);
  signal_connect("ping");
  TEST_EQ(g_signal_live, 1);
  TEST_EQ(g_signal_callable->ref_count, 2);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_copy_mode (void)
{
  TEST_EQ(signal_setup(true), 0);
  signal_connect("ping");
  TEST_EQ(g_signal_live, 1);
  TEST_EQ(g_signal_callable->ref_count, 1);
  g_signal_emit(g_signal_object, g_signal_ping, 0);
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(g_signal_args_ok, true);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static char signal_self_disconnect (void)
{
  TEST_EQ(signal_setup(false), 0);
  signal_connect("ping");
  TEST_ASSERT((g_signal_handler = g_signal_handler_find
                (g_signal_object, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL,
                 g_signal_tuple)));
  g_signal_disconnect = true;
  g_signal_emit(g_signal_object, g_signal_ping, 0);
  TEST_EQ(g_signal_calls, 1);
  TEST_EQ(g_signal_args_ok, true);
  TEST_EQ(g_signal_live_during_disconnect, 1);
  TEST_EQ(g_signal_live, 0);
  TEST_EQ(signal_finish(), 0);
  return 0;
}

static int signal_child (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(10);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

#define SIGNAL_CASE(name)                                      \
  TEST_CASE(gtk4_signal_ ## name)                               \
  {                                                           \
    TEST_EQ(signal_child(signal_ ## name), 0);                  \
  }                                                           \
  TEST_CASE_END(gtk4_signal_ ## name)

SIGNAL_CASE(supported)
SIGNAL_CASE(detailed)
SIGNAL_CASE(notify)
SIGNAL_CASE(parameter)
SIGNAL_CASE(result)
SIGNAL_CASE(names)
SIGNAL_CASE(copy_failures)
SIGNAL_CASE(arguments)
SIGNAL_CASE(destroy)
SIGNAL_CASE(copy_mode)
SIGNAL_CASE(self_disconnect)

#endif

void gtk4_signal_test (void)
{
#if HAVE_GTK4
  TEST_CASE_RUN(gtk4_signal_supported);
  TEST_CASE_RUN(gtk4_signal_detailed);
  TEST_CASE_RUN(gtk4_signal_notify);
  TEST_CASE_RUN(gtk4_signal_parameter);
  TEST_CASE_RUN(gtk4_signal_result);
  TEST_CASE_RUN(gtk4_signal_names);
  TEST_CASE_RUN(gtk4_signal_copy_failures);
  TEST_CASE_RUN(gtk4_signal_arguments);
  TEST_CASE_RUN(gtk4_signal_destroy);
  TEST_CASE_RUN(gtk4_signal_copy_mode);
  TEST_CASE_RUN(gtk4_signal_self_disconnect);
#endif
}
