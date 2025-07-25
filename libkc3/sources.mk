# sources.mk generated by update_sources
HEADERS = \
	"abs.h" \
	"alist.h" \
	"alloc.h" \
	"arg.h" \
	"array.h" \
	"assert.h" \
	"binding.h" \
	"block.h" \
	"bool.h" \
	"buf.h" \
	"buf_fd.h" \
	"buf_file.h" \
	"buf_getc.h" \
	"buf_getchar.h" \
	"buf_inspect.h" \
	"buf_inspect_s16.h" \
	"buf_inspect_s16_binary.h" \
	"buf_inspect_s16_decimal.h" \
	"buf_inspect_s16_hexadecimal.h" \
	"buf_inspect_s16_octal.h" \
	"buf_inspect_s32.h" \
	"buf_inspect_s32_binary.h" \
	"buf_inspect_s32_decimal.h" \
	"buf_inspect_s32_hexadecimal.h" \
	"buf_inspect_s32_octal.h" \
	"buf_inspect_s64.h" \
	"buf_inspect_s64_binary.h" \
	"buf_inspect_s64_decimal.h" \
	"buf_inspect_s64_hexadecimal.h" \
	"buf_inspect_s64_octal.h" \
	"buf_inspect_s8.h" \
	"buf_inspect_s8_binary.h" \
	"buf_inspect_s8_decimal.h" \
	"buf_inspect_s8_hexadecimal.h" \
	"buf_inspect_s8_octal.h" \
	"buf_inspect_sw.h" \
	"buf_inspect_sw_binary.h" \
	"buf_inspect_sw_decimal.h" \
	"buf_inspect_sw_hexadecimal.h" \
	"buf_inspect_sw_octal.h" \
	"buf_inspect_u16.h" \
	"buf_inspect_u16_binary.h" \
	"buf_inspect_u16_decimal.h" \
	"buf_inspect_u16_hexadecimal.h" \
	"buf_inspect_u16_octal.h" \
	"buf_inspect_u32.h" \
	"buf_inspect_u32_binary.h" \
	"buf_inspect_u32_decimal.h" \
	"buf_inspect_u32_hexadecimal.h" \
	"buf_inspect_u32_octal.h" \
	"buf_inspect_u64.h" \
	"buf_inspect_u64_binary.h" \
	"buf_inspect_u64_decimal.h" \
	"buf_inspect_u64_hexadecimal.h" \
	"buf_inspect_u64_octal.h" \
	"buf_inspect_u8.h" \
	"buf_inspect_u8_binary.h" \
	"buf_inspect_u8_decimal.h" \
	"buf_inspect_u8_hexadecimal.h" \
	"buf_inspect_u8_octal.h" \
	"buf_inspect_uw.h" \
	"buf_inspect_uw_binary.h" \
	"buf_inspect_uw_decimal.h" \
	"buf_inspect_uw_hexadecimal.h" \
	"buf_inspect_uw_octal.h" \
	"buf_parse.h" \
	"buf_parse_s16.h" \
	"buf_parse_s32.h" \
	"buf_parse_s64.h" \
	"buf_parse_s8.h" \
	"buf_parse_sw.h" \
	"buf_parse_u16.h" \
	"buf_parse_u32.h" \
	"buf_parse_u64.h" \
	"buf_parse_u8.h" \
	"buf_parse_uw.h" \
	"buf_rw.h" \
	"buf_save.h" \
	"call.h" \
	"callable.h" \
	"cast.h" \
	"ceiling.h" \
	"cfn.h" \
	"character.h" \
	"compare.h" \
	"complex.h" \
	"cow.h" \
	"crypt.h" \
	"data.h" \
	"do_block.h" \
	"endian.h" \
	"env.h" \
	"env_eval.h" \
	"env_eval_equal.h" \
	"env_eval_quote.h" \
	"env_fork.h" \
	"env_frame_capture.h" \
	"error.h" \
	"error_handler.h" \
	"eval.h" \
	"explicit_bzero.h" \
	"f128.h" \
	"f32.h" \
	"f64.h" \
	"fact.h" \
	"fact_action.h" \
	"fact_list.h" \
	"facts.h" \
	"facts_cursor.h" \
	"facts_spec.h" \
	"facts_spec_cursor.h" \
	"facts_transaction.h" \
	"facts_with.h" \
	"facts_with_cursor.h" \
	"fd.h" \
	"file.h" \
	"float.h" \
	"fn.h" \
	"fn_clause.h" \
	"frame.h" \
	"hash.h" \
	"ht.h" \
	"ident.h" \
	"inspect.h" \
	"integer.h" \
	"io.h" \
	"kc3.h" \
	"kc3_main.h" \
	"list.h" \
	"list_init.h" \
	"log.h" \
	"map.h" \
	"marshall.h" \
	"marshall_read.h" \
	"module.h" \
	"mutex.h" \
	"op.h" \
	"ops.h" \
	"pcallable.h" \
	"pcomplex.h" \
	"pcow.h" \
	"plist.h" \
	"pretty.h" \
	"pstruct.h" \
	"pstruct_type.h" \
	"psym.h" \
	"ptag.h" \
	"ptr.h" \
	"ptr_free.h" \
	"pvar.h" \
	"queue.h" \
	"quote.h" \
	"ratio.h" \
	"rwlock.h" \
	"s16.h" \
	"s32.h" \
	"s64.h" \
	"s8.h" \
	"sequence.h" \
	"set__fact.h" \
	"set__tag.h" \
	"set_cursor__fact.h" \
	"set_cursor__tag.h" \
	"set_item__fact.h" \
	"set_item__tag.h" \
	"sh.h" \
	"sha1.h" \
	"sign.h" \
	"skiplist__fact.h" \
	"skiplist_node__fact.h" \
	"special_operator.h" \
	"str.h" \
	"struct.h" \
	"struct_type.h" \
	"sw.h" \
	"sym.h" \
	"tag.h" \
	"tag_init.h" \
	"tag_type.h" \
	"time.h" \
	"timespec.h" \
	"to_lisp.h" \
	"tuple.h" \
	"types.h" \
	"u16.h" \
	"u32.h" \
	"u64.h" \
	"u8.h" \
	"ucd.h" \
	"unquote.h" \
	"uw.h" \
	"var.h" \
	"void.h" \
	"wait.h" \

SOURCES = \
	"abs.c" \
	"alist.c" \
	"alloc.c" \
	"arg.c" \
	"array.c" \
	"binding.c" \
	"block.c" \
	"bool.c" \
	"buf.c" \
	"buf_fd.c" \
	"buf_file.c" \
	"buf_getc.c" \
	"buf_getchar.c" \
	"buf_inspect.c" \
	"buf_inspect_s16.c" \
	"buf_inspect_s16_binary.c" \
	"buf_inspect_s16_decimal.c" \
	"buf_inspect_s16_hexadecimal.c" \
	"buf_inspect_s16_octal.c" \
	"buf_inspect_s32.c" \
	"buf_inspect_s32_binary.c" \
	"buf_inspect_s32_decimal.c" \
	"buf_inspect_s32_hexadecimal.c" \
	"buf_inspect_s32_octal.c" \
	"buf_inspect_s64.c" \
	"buf_inspect_s64_binary.c" \
	"buf_inspect_s64_decimal.c" \
	"buf_inspect_s64_hexadecimal.c" \
	"buf_inspect_s64_octal.c" \
	"buf_inspect_s8.c" \
	"buf_inspect_s8_binary.c" \
	"buf_inspect_s8_decimal.c" \
	"buf_inspect_s8_hexadecimal.c" \
	"buf_inspect_s8_octal.c" \
	"buf_inspect_sw.c" \
	"buf_inspect_sw_binary.c" \
	"buf_inspect_sw_decimal.c" \
	"buf_inspect_sw_hexadecimal.c" \
	"buf_inspect_sw_octal.c" \
	"buf_inspect_u16.c" \
	"buf_inspect_u16_binary.c" \
	"buf_inspect_u16_decimal.c" \
	"buf_inspect_u16_hexadecimal.c" \
	"buf_inspect_u16_octal.c" \
	"buf_inspect_u32.c" \
	"buf_inspect_u32_binary.c" \
	"buf_inspect_u32_decimal.c" \
	"buf_inspect_u32_hexadecimal.c" \
	"buf_inspect_u32_octal.c" \
	"buf_inspect_u64.c" \
	"buf_inspect_u64_binary.c" \
	"buf_inspect_u64_decimal.c" \
	"buf_inspect_u64_hexadecimal.c" \
	"buf_inspect_u64_octal.c" \
	"buf_inspect_u8.c" \
	"buf_inspect_u8_binary.c" \
	"buf_inspect_u8_decimal.c" \
	"buf_inspect_u8_hexadecimal.c" \
	"buf_inspect_u8_octal.c" \
	"buf_inspect_uw.c" \
	"buf_inspect_uw_binary.c" \
	"buf_inspect_uw_decimal.c" \
	"buf_inspect_uw_hexadecimal.c" \
	"buf_inspect_uw_octal.c" \
	"buf_parse.c" \
	"buf_parse_s16.c" \
	"buf_parse_s32.c" \
	"buf_parse_s64.c" \
	"buf_parse_s8.c" \
	"buf_parse_sw.c" \
	"buf_parse_u16.c" \
	"buf_parse_u32.c" \
	"buf_parse_u64.c" \
	"buf_parse_u8.c" \
	"buf_parse_uw.c" \
	"buf_rw.c" \
	"buf_save.c" \
	"call.c" \
	"callable.c" \
	"cast.c" \
	"ceiling.c" \
	"cfn.c" \
	"character.c" \
	"compare.c" \
	"complex.c" \
	"cow.c" \
	"crypt.c" \
	"crypt_sha512.c" \
	"data.c" \
	"do_block.c" \
	"env.c" \
	"env_eval.c" \
	"env_eval_equal.c" \
	"env_eval_quote.c" \
	"env_fork.c" \
	"env_frame_capture.c" \
	"error.c" \
	"error_handler.c" \
	"eval.c" \
	"f128.c" \
	"f32.c" \
	"f64.c" \
	"fact.c" \
	"fact_action.c" \
	"fact_list.c" \
	"facts.c" \
	"facts_cursor.c" \
	"facts_spec.c" \
	"facts_spec_cursor.c" \
	"facts_transaction.c" \
	"facts_with.c" \
	"facts_with_cursor.c" \
	"fd.c" \
	"file.c" \
	"fn.c" \
	"fn_clause.c" \
	"frame.c" \
	"hash.c" \
	"ht.c" \
	"ident.c" \
	"inspect.c" \
	"integer.c" \
	"io.c" \
	"kc3.c" \
	"license.c" \
	"list.c" \
	"list_init.c" \
	"log.c" \
	"map.c" \
	"marshall.c" \
	"marshall_read.c" \
	"module.c" \
	"mutex.c" \
	"op.c" \
	"ops.c" \
	"pcallable.c" \
	"pcomplex.c" \
	"pcow.c" \
	"plist.c" \
	"pretty.c" \
	"pstruct.c" \
	"pstruct_type.c" \
	"psym.c" \
	"ptag.c" \
	"ptr.c" \
	"ptr_free.c" \
	"pvar.c" \
	"queue.c" \
	"quote.c" \
	"ratio.c" \
	"rwlock.c" \
	"s16.c" \
	"s32.c" \
	"s64.c" \
	"s8.c" \
	"sequence.c" \
	"set__fact.c" \
	"set__tag.c" \
	"set_cursor__fact.c" \
	"set_cursor__tag.c" \
	"set_item__fact.c" \
	"set_item__tag.c" \
	"sh.c" \
	"sha1.c" \
	"sign.c" \
	"skiplist__fact.c" \
	"skiplist_node__fact.c" \
	"special_operator.c" \
	"str.c" \
	"struct.c" \
	"struct_type.c" \
	"sw.c" \
	"sym.c" \
	"tag.c" \
	"tag_add.c" \
	"tag_addi.c" \
	"tag_band.c" \
	"tag_bnot.c" \
	"tag_bor.c" \
	"tag_bxor.c" \
	"tag_div.c" \
	"tag_init.c" \
	"tag_mod.c" \
	"tag_mul.c" \
	"tag_neg.c" \
	"tag_shift_left.c" \
	"tag_shift_right.c" \
	"tag_sqrt.c" \
	"tag_sub.c" \
	"tag_type.c" \
	"time.c" \
	"timespec.c" \
	"to_lisp.c" \
	"tuple.c" \
	"u16.c" \
	"u32.c" \
	"u64.c" \
	"u8.c" \
	"ucd.c" \
	"unquote.c" \
	"uw.c" \
	"var.c" \
	"void.c" \
	"wait.c" \

LO_SOURCES = \
	"" \
	"../libtommath/bn_cutoffs.c" \
	"../libtommath/bn_mp_2expt.c" \
	"../libtommath/bn_mp_abs.c" \
	"../libtommath/bn_mp_add.c" \
	"../libtommath/bn_mp_add_d.c" \
	"../libtommath/bn_mp_and.c" \
	"../libtommath/bn_mp_clamp.c" \
	"../libtommath/bn_mp_clear.c" \
	"../libtommath/bn_mp_clear_multi.c" \
	"../libtommath/bn_mp_cmp.c" \
	"../libtommath/bn_mp_cmp_d.c" \
	"../libtommath/bn_mp_cmp_mag.c" \
	"../libtommath/bn_mp_cnt_lsb.c" \
	"../libtommath/bn_mp_complement.c" \
	"../libtommath/bn_mp_copy.c" \
	"../libtommath/bn_mp_count_bits.c" \
	"../libtommath/bn_mp_div.c" \
	"../libtommath/bn_mp_div_2.c" \
	"../libtommath/bn_mp_div_2d.c" \
	"../libtommath/bn_mp_div_3.c" \
	"../libtommath/bn_mp_div_d.c" \
	"../libtommath/bn_mp_dr_is_modulus.c" \
	"../libtommath/bn_mp_dr_reduce.c" \
	"../libtommath/bn_mp_dr_setup.c" \
	"../libtommath/bn_mp_error_to_string.c" \
	"../libtommath/bn_mp_exch.c" \
	"../libtommath/bn_mp_expt_u32.c" \
	"../libtommath/bn_mp_exptmod.c" \
	"../libtommath/bn_mp_gcd.c" \
	"../libtommath/bn_mp_get_double.c" \
	"../libtommath/bn_mp_get_i32.c" \
	"../libtommath/bn_mp_get_i64.c" \
	"../libtommath/bn_mp_get_mag_u32.c" \
	"../libtommath/bn_mp_get_mag_u64.c" \
	"../libtommath/bn_mp_grow.c" \
	"../libtommath/bn_mp_init.c" \
	"../libtommath/bn_mp_init_copy.c" \
	"../libtommath/bn_mp_init_multi.c" \
	"../libtommath/bn_mp_init_size.c" \
	"../libtommath/bn_mp_invmod.c" \
	"../libtommath/bn_mp_lcm.c" \
	"../libtommath/bn_mp_lshd.c" \
	"../libtommath/bn_mp_mod.c" \
	"../libtommath/bn_mp_mod_2d.c" \
	"../libtommath/bn_mp_montgomery_calc_normalization.c" \
	"../libtommath/bn_mp_montgomery_reduce.c" \
	"../libtommath/bn_mp_montgomery_setup.c" \
	"../libtommath/bn_mp_mul.c" \
	"../libtommath/bn_mp_mul_2.c" \
	"../libtommath/bn_mp_mul_2d.c" \
	"../libtommath/bn_mp_mul_d.c" \
	"../libtommath/bn_mp_mulmod.c" \
	"../libtommath/bn_mp_neg.c" \
	"../libtommath/bn_mp_or.c" \
	"../libtommath/bn_mp_radix_size.c" \
	"../libtommath/bn_mp_reduce.c" \
	"../libtommath/bn_mp_reduce_2k.c" \
	"../libtommath/bn_mp_reduce_2k_l.c" \
	"../libtommath/bn_mp_reduce_2k_setup.c" \
	"../libtommath/bn_mp_reduce_2k_setup_l.c" \
	"../libtommath/bn_mp_reduce_is_2k.c" \
	"../libtommath/bn_mp_reduce_is_2k_l.c" \
	"../libtommath/bn_mp_reduce_setup.c" \
	"../libtommath/bn_mp_rshd.c" \
	"../libtommath/bn_mp_set.c" \
	"../libtommath/bn_mp_set_double.c" \
	"../libtommath/bn_mp_set_i32.c" \
	"../libtommath/bn_mp_set_i64.c" \
	"../libtommath/bn_mp_set_l.c" \
	"../libtommath/bn_mp_set_u32.c" \
	"../libtommath/bn_mp_set_u64.c" \
	"../libtommath/bn_mp_set_ul.c" \
	"../libtommath/bn_mp_sqr.c" \
	"../libtommath/bn_mp_sqrt.c" \
	"../libtommath/bn_mp_sub.c" \
	"../libtommath/bn_mp_sub_d.c" \
	"../libtommath/bn_mp_xor.c" \
	"../libtommath/bn_mp_zero.c" \
	"../libtommath/bn_s_mp_add.c" \
	"../libtommath/bn_s_mp_balance_mul.c" \
	"../libtommath/bn_s_mp_exptmod.c" \
	"../libtommath/bn_s_mp_exptmod_fast.c" \
	"../libtommath/bn_s_mp_invmod_fast.c" \
	"../libtommath/bn_s_mp_invmod_slow.c" \
	"../libtommath/bn_s_mp_karatsuba_mul.c" \
	"../libtommath/bn_s_mp_karatsuba_sqr.c" \
	"../libtommath/bn_s_mp_montgomery_reduce_fast.c" \
	"../libtommath/bn_s_mp_mul_digs.c" \
	"../libtommath/bn_s_mp_mul_digs_fast.c" \
	"../libtommath/bn_s_mp_mul_high_digs.c" \
	"../libtommath/bn_s_mp_mul_high_digs_fast.c" \
	"../libtommath/bn_s_mp_rand_platform.c" \
	"../libtommath/bn_s_mp_sqr.c" \
	"../libtommath/bn_s_mp_sqr_fast.c" \
	"../libtommath/bn_s_mp_sub.c" \
	"../libtommath/bn_s_mp_toom_mul.c" \
	"../libtommath/bn_s_mp_toom_sqr.c" \
	"abs.c" \
	"alist.c" \
	"alloc.c" \
	"arg.c" \
	"array.c" \
	"binding.c" \
	"block.c" \
	"bool.c" \
	"buf.c" \
	"buf_fd.c" \
	"buf_file.c" \
	"buf_getc.c" \
	"buf_getchar.c" \
	"buf_inspect.c" \
	"buf_inspect_s16.c" \
	"buf_inspect_s16_binary.c" \
	"buf_inspect_s16_decimal.c" \
	"buf_inspect_s16_hexadecimal.c" \
	"buf_inspect_s16_octal.c" \
	"buf_inspect_s32.c" \
	"buf_inspect_s32_binary.c" \
	"buf_inspect_s32_decimal.c" \
	"buf_inspect_s32_hexadecimal.c" \
	"buf_inspect_s32_octal.c" \
	"buf_inspect_s64.c" \
	"buf_inspect_s64_binary.c" \
	"buf_inspect_s64_decimal.c" \
	"buf_inspect_s64_hexadecimal.c" \
	"buf_inspect_s64_octal.c" \
	"buf_inspect_s8.c" \
	"buf_inspect_s8_binary.c" \
	"buf_inspect_s8_decimal.c" \
	"buf_inspect_s8_hexadecimal.c" \
	"buf_inspect_s8_octal.c" \
	"buf_inspect_sw.c" \
	"buf_inspect_sw_binary.c" \
	"buf_inspect_sw_decimal.c" \
	"buf_inspect_sw_hexadecimal.c" \
	"buf_inspect_sw_octal.c" \
	"buf_inspect_u16.c" \
	"buf_inspect_u16_binary.c" \
	"buf_inspect_u16_decimal.c" \
	"buf_inspect_u16_hexadecimal.c" \
	"buf_inspect_u16_octal.c" \
	"buf_inspect_u32.c" \
	"buf_inspect_u32_binary.c" \
	"buf_inspect_u32_decimal.c" \
	"buf_inspect_u32_hexadecimal.c" \
	"buf_inspect_u32_octal.c" \
	"buf_inspect_u64.c" \
	"buf_inspect_u64_binary.c" \
	"buf_inspect_u64_decimal.c" \
	"buf_inspect_u64_hexadecimal.c" \
	"buf_inspect_u64_octal.c" \
	"buf_inspect_u8.c" \
	"buf_inspect_u8_binary.c" \
	"buf_inspect_u8_decimal.c" \
	"buf_inspect_u8_hexadecimal.c" \
	"buf_inspect_u8_octal.c" \
	"buf_inspect_uw.c" \
	"buf_inspect_uw_binary.c" \
	"buf_inspect_uw_decimal.c" \
	"buf_inspect_uw_hexadecimal.c" \
	"buf_inspect_uw_octal.c" \
	"buf_parse.c" \
	"buf_parse_s16.c" \
	"buf_parse_s32.c" \
	"buf_parse_s64.c" \
	"buf_parse_s8.c" \
	"buf_parse_sw.c" \
	"buf_parse_u16.c" \
	"buf_parse_u32.c" \
	"buf_parse_u64.c" \
	"buf_parse_u8.c" \
	"buf_parse_uw.c" \
	"buf_rw.c" \
	"buf_save.c" \
	"call.c" \
	"callable.c" \
	"cast.c" \
	"ceiling.c" \
	"cfn.c" \
	"character.c" \
	"compare.c" \
	"complex.c" \
	"cow.c" \
	"crypt.c" \
	"crypt_sha512.c" \
	"data.c" \
	"do_block.c" \
	"env.c" \
	"env_eval.c" \
	"env_eval_equal.c" \
	"env_eval_quote.c" \
	"env_fork.c" \
	"env_frame_capture.c" \
	"error.c" \
	"error_handler.c" \
	"eval.c" \
	"f128.c" \
	"f32.c" \
	"f64.c" \
	"fact.c" \
	"fact_action.c" \
	"fact_list.c" \
	"facts.c" \
	"facts_cursor.c" \
	"facts_spec.c" \
	"facts_spec_cursor.c" \
	"facts_transaction.c" \
	"facts_with.c" \
	"facts_with_cursor.c" \
	"fd.c" \
	"file.c" \
	"fn.c" \
	"fn_clause.c" \
	"frame.c" \
	"hash.c" \
	"ht.c" \
	"ident.c" \
	"inspect.c" \
	"integer.c" \
	"io.c" \
	"kc3.c" \
	"license.c" \
	"list.c" \
	"list_init.c" \
	"log.c" \
	"map.c" \
	"marshall.c" \
	"marshall_read.c" \
	"module.c" \
	"mutex.c" \
	"op.c" \
	"ops.c" \
	"pcallable.c" \
	"pcomplex.c" \
	"pcow.c" \
	"plist.c" \
	"pretty.c" \
	"pstruct.c" \
	"pstruct_type.c" \
	"psym.c" \
	"ptag.c" \
	"ptr.c" \
	"ptr_free.c" \
	"pvar.c" \
	"queue.c" \
	"quote.c" \
	"ratio.c" \
	"rwlock.c" \
	"s16.c" \
	"s32.c" \
	"s64.c" \
	"s8.c" \
	"sequence.c" \
	"set__fact.c" \
	"set__tag.c" \
	"set_cursor__fact.c" \
	"set_cursor__tag.c" \
	"set_item__fact.c" \
	"set_item__tag.c" \
	"sh.c" \
	"sha1.c" \
	"sign.c" \
	"skiplist__fact.c" \
	"skiplist_node__fact.c" \
	"special_operator.c" \
	"str.c" \
	"struct.c" \
	"struct_type.c" \
	"sw.c" \
	"sym.c" \
	"tag.c" \
	"tag_add.c" \
	"tag_addi.c" \
	"tag_band.c" \
	"tag_bnot.c" \
	"tag_bor.c" \
	"tag_bxor.c" \
	"tag_div.c" \
	"tag_init.c" \
	"tag_mod.c" \
	"tag_mul.c" \
	"tag_neg.c" \
	"tag_shift_left.c" \
	"tag_shift_right.c" \
	"tag_sqrt.c" \
	"tag_sub.c" \
	"tag_type.c" \
	"time.c" \
	"timespec.c" \
	"to_lisp.c" \
	"tuple.c" \
	"u16.c" \
	"u32.c" \
	"u64.c" \
	"u8.c" \
	"ucd.c" \
	"unquote.c" \
	"uw.c" \
	"var.c" \
	"void.c" \
	"wait.c" \

