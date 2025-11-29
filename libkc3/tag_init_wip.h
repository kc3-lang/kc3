
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
