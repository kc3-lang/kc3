
#include "error.h"

#if HAVE_F80
# define TAG_TYPE_F80 f80:    TAG_F80,
#else
# define TAG_TYPE_F80
#endif


#if HAVE_F128
# define TAG_TYPE_F128 \
  f128: TAG_F128,
#else
# define TAG_TYPE_F128
#endif

#if HAVE_GENERIC_SW_UW
# define TAG_TYPE_SW_UW       \
    sw:        TAG_SW,        \
    uw:        TAG_UW,
#else
# define TAG_TYPE_SW_UW
#endif

/* conflicts :
    bool:      TAG_BOOL,                         \
    character: TAG_CHARACTER, \
*/

#define TAG_TYPE(x)                                                   \
  _Generic((x),                                                       \
           TAG_TYPE_SW_UW                                             \
           s_array*:      TAG_ARRAY,                                  \
           s_do_block*:   TAG_DO_BLOCK,                               \
           f32:           TAG_F32,                                    \
           f64:           TAG_F64,                                    \
           TAG_TYPE_F80                                               \
           TAG_TYPE_F128                                              \
           s_fact*:       TAG_FACT,                                   \
           s_ident*:      TAG_IDENT,                                  \
           s_integer*:    TAG_INTEGER,                                \
           s_ratio*:      TAG_RATIO,                                  \
           s64:           TAG_S64,                                    \
           s32: ((x > U16_MAX) ? TAG_U32 :                            \
                 (x > U8_MAX) ? TAG_U16 :                             \
                 (x >= 0) ? TAG_U8 :                                  \
                 (x >= S8_MIN) ? TAG_S8 :                             \
                 (x >= S16_MIN) ? TAG_S16 :                           \
                 S32_MIN),                                            \
           s16:           TAG_S16,                                    \
           s8:            TAG_S8,                                     \
           u8:            TAG_U8,                                     \
           u16:           TAG_U16,                                    \
           u32:           TAG_U32,                                    \
           u64:           TAG_U64,                                    \
           p_list:        TAG_PLIST,                                  \
           s_map*:        TAG_MAP,                                    \
           p_call:        TAG_PCALL,                                  \
           p_callable:    TAG_PCALLABLE,                              \
           p_complex:     TAG_PCOMPLEX,                               \
           p_cow:         TAG_PCOW,                                   \
           p_facts:       TAG_PFACTS,                                 \
           s_pointer*:    TAG_POINTER,                                \
           p_struct:      TAG_PSTRUCT,                                \
           p_struct_type: TAG_PSTRUCT_TYPE,                           \
           p_sym:         TAG_PSYM,                                   \
           p_tag:         TAG_PTAG,                                   \
           u_ptr:         TAG_PTR,                                    \
           u_ptr_w:       TAG_PTR_FREE,                               \
           p_var:         TAG_PVAR,                                   \
           s_quote*:      TAG_QUOTE,                                  \
           char*:         TAG_STR,                                    \
           s_str*:        TAG_STR,                                    \
           s_time*:       TAG_TIME,                                   \
           s_tuple*:      TAG_TUPLE,                                  \
           s_unquote*:    TAG_UNQUOTE,                                \
           default: (err_puts("unknown tag type"), TAG_VOID))

/*
  
#define KEEP_FIRST_ARG(first, ...) first
#define EXTRA_ARGS(...) __VA_OPT__(,) __VA_ARGS__

#if HAVE_F80
    #define TAG_INIT_F80 f80: \
        tag_init_f80(tag EXTRA_ARGS(__VA_ARGS__)),
#else
    #define TAG_INIT_F80
#endif

#if HAVE_F128
    #define TAG_INIT_F128 f128: \
        tag_init_f128(tag EXTRA_ARGS(__VA_ARGS__)),
#else
    #define TAG_INIT_F128
#endif

#define TAG_INIT(tag, ...) _Generic((KEEP_FIRST_ARG(__VA_ARGS__, 0)), \
    default:       NULL,                                              \
    s_array:       tag_init_array(tag EXTRA_ARGS(__VA_ARGS__)),       \
    f32:           tag_init_f32(tag EXTRA_ARGS(__VA_ARGS__)),         \
    f64:           tag_init_f64(tag EXTRA_ARGS(__VA_ARGS__)),         \
    TAG_INIT_F80                                                      \
    TAG_INIT_F128                                                     \
    s_ident:       tag_init_ident(tag EXTRA_ARGS(__VA_ARGS__)),       \
    s_integer:     tag_init_integer_copy(tag EXTRA_ARGS(__VA_ARGS__)),\
    s_ratio:       tag_init_ratio(tag EXTRA_ARGS(__VA_ARGS__)),       \
    s64:           tag_init_s64(tag EXTRA_ARGS(__VA_ARGS__)),         \
    s32:           tag_init_s32(tag EXTRA_ARGS(__VA_ARGS__)),         \
    s16:           tag_init_s16(tag EXTRA_ARGS(__VA_ARGS__)),         \
    s8:            tag_init_s8(tag EXTRA_ARGS(__VA_ARGS__)),          \
    u8:            tag_init_u8(tag EXTRA_ARGS(__VA_ARGS__)),          \
    u16:           tag_init_u16(tag EXTRA_ARGS(__VA_ARGS__)),         \
    u32:           tag_init_u32(tag EXTRA_ARGS(__VA_ARGS__)),         \
    u64:           tag_init_u64(tag EXTRA_ARGS(__VA_ARGS__)),         \
    p_list:        tag_init_plist(tag EXTRA_ARGS(__VA_ARGS__)),       \
    s_map:         tag_init_map(tag EXTRA_ARGS(__VA_ARGS__)),         \
    p_call:        tag_init_pcall(tag EXTRA_ARGS(__VA_ARGS__)),       \
    p_callable:    tag_init_pcallable(tag EXTRA_ARGS(__VA_ARGS__)),   \
    p_complex:     tag_init_pcomplex(tag EXTRA_ARGS(__VA_ARGS__)),    \
    s_pointer:     tag_init_pointer(tag EXTRA_ARGS(__VA_ARGS__)),     \
    p_struct:      tag_init_pstruct(tag EXTRA_ARGS(__VA_ARGS__)),     \
    p_struct_type: tag_init_pstruct_type(tag EXTRA_ARGS(__VA_ARGS__)),\
    p_sym:         tag_init_psym(tag EXTRA_ARGS(__VA_ARGS__)),        \
    u_ptr:         tag_init_ptr(tag EXTRA_ARGS(__VA_ARGS__)),         \
    u_ptr_w:       tag_init_ptr_free(tag EXTRA_ARGS(__VA_ARGS__)),    \
    p_var:         tag_init_pvar(tag EXTRA_ARGS(__VA_ARGS__)),        \
    s_quote:       tag_init_quote(tag EXTRA_ARGS(__VA_ARGS__)),       \
    s_str:         tag_init_str(tag EXTRA_ARGS(__VA_ARGS__)),         \
    s_time:        tag_init_time(tag EXTRA_ARGS(__VA_ARGS__)),        \
    s_tuple:       tag_init_tuple(tag EXTRA_ARGS(__VA_ARGS__)),       \
    s_unquote:     tag_init_unquote_copy(tag EXTRA_ARGS(__VA_ARGS__)))

*/
