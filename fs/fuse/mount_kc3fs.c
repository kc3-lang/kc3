/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include <errno.h>
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <fuse_opt.h>
#include <string.h>
#include "../../libkc3/kc3.h"
#include "types.h"

static int fs_readdir (const char *path, void *data,
                       fuse_fill_dir_t fill_dir, off_t offset,
                       struct fuse_file_info *file_info);
static int fs_read (const char *path, char *buf, size_t size,
                    off_t offset, struct fuse_file_info *file_info);
static int fs_open (const char *path, struct fuse_file_info *ffi);
static int fs_getattr (const char *path, struct stat *st);

static s_facts *g_facts = NULL;

static const struct fuse_operations g_fsops = {
  .readdir = fs_readdir,
  .read = fs_read,
  .open = fs_open,
  .getattr = fs_getattr,
};

static const char g_readme_md[] =
  "# KC3 FUSE filesystem example\n"
  "\n"
  "This is an example of filesystem implementation in C11\n"
  "using FUSE from OpenBSD 7.6 and libkc3 v0.1.14\n"
  "\n"
  "There is a small memory filesystem containing this file.\n"
  "\n"
  "## License\n"
  "kc3\n"
  "Copyright 2022,2023,2024 kmx.io <contact@kmx.io>\n"
  "\n"
  "Permission is hereby granted to use this software granted the\n"
  "above copyright notice and this permission paragraph are\n"
  "included in all copies and substantial portions of this\n"
  "software.\n"
  "\n"
  "THIS SOFTWARE IS PROVIDED \"AS-IS\" WITHOUT ANY GUARANTEE OF\n"
  "PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE\n"
  "AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF\n"
  "THIS SOFTWARE.\n";

static const s_sym *g_sym_FileStat = NULL;
static const s_sym *g_sym_kc3fs_file_stat = NULL;

static int fs_readdir (const char *path, void *data, fuse_fill_dir_t fill_dir,
                off_t offset, struct fuse_file_info *file_info)
{
  (void) offset;
  (void) file_info;
  if (strcmp(path, "/"))
    return -ENOENT;
  fill_dir(data, ".", NULL, 0);
  fill_dir(data, "..", NULL, 0);
  fill_dir(data, "README.md", NULL, 0);
  return 0;
}

static int fs_read (const char *path, char *buf, size_t size,
                    off_t offset, struct fuse_file_info *file_info)
{
  int len;
  s_str path_str;
  (void) file_info;
  str_init_1(&path_str, NULL, path);
  if (true) {
    err_write_1("fs_read: ");
    err_inspect_str(&path_str);
    err_write_1("\n");
  }
  if (strncmp(path, "/README.md", path_str.size))
    return -ENOENT;
  len = strlen(g_readme_md);
  if (offset < len) {
    if (offset + size > (uw) len)
      size = len - offset;
    memcpy(buf, g_readme_md + offset, size);
  }
  else
    size = 0;
  return size;
}

static int fs_open (const char *path, struct fuse_file_info *ffi)
{
  if (strcmp(path, "/README.md"))
    return -ENOENT;
  if ((ffi->flags & 3) != O_RDONLY)
    return -EACCES;
  return 0;
}

static int fs_getattr (const char *path, struct stat *dest)
{
  s_facts_with_cursor cursor = {0};
  s_fact *fact = NULL;
  s_tag tag_path;
  s_tag tag_sym_kc3fs_file_stat;
  s_tag tag_file_stat;
  tag_init_str_1(&tag_path, NULL, path);
  tag_init_sym(&tag_sym_kc3fs_file_stat, g_sym_kc3fs_file_stat);
  tag_init_var(&tag_file_stat, g_sym_FileStat);
  if (! facts_with(g_facts, &cursor, (t_facts_spec) {
        &tag_path, &tag_sym_kc3fs_file_stat, &tag_file_stat,
        NULL, NULL})) {
    err_puts("fs_getattr: facts_with");
    abort();
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    err_puts("fs_getattr: facts_with_cursor_next");
    abort();
  }
  if (! fact)
    return -ENOENT;
  if (file_stat_to_struct_stat(tag_file_stat.data.struct_.data,
                               dest)) {
    err_puts("fs_getattr: file_stat_to_struct_stat");
    abort();
  }
  facts_with_cursor_clean(&cursor);
  return 0;
}

static int fs_init (void)
{
  uw *blocks = NULL;
  uw *size = NULL;
  s_tag tag_file_stat;
  s_tag tag_file_stat2;
  s_tag tag_path;
  s_tag tag_sym_kc3fs_file_stat;
  g_facts = g_kc3_env->facts;
  g_sym_FileStat = sym_1("FileStat");
  g_sym_kc3fs_file_stat = sym_1("kc3fs_file_stat");
  tag_init_sym(&tag_sym_kc3fs_file_stat, g_sym_kc3fs_file_stat);
  tag_init_str_1(&tag_path, NULL, "/");
  tag_init_1(&tag_file_stat,
             "%FileStat{st_dev: (Uw) 1,"
             " st_ino: (Uw) 1,"
             " st_mode: [:r, :w, :x],"
             " st_nlink: (Uw) 2,"
             " st_uid: (Uw) 0,"
             " st_gid: (Uw) 0,"
             " st_rdev: (Uw) 0,"
             " st_size: (Uw) 512,"
             " st_blksize: (Uw) 512,"
             " st_blocks: (Uw) 1,"
             " st_atim: Time.now(),"
             " st_ctim: %Time{tv_sec: 1735058914, tv_nsec: 4},"
             " st_mtim: %Time{tv_sec: 1735058914, tv_nsec: 4}}");
  eval_tag(&tag_file_stat, &tag_file_stat2);
  tag_clean(&tag_file_stat);
  if (! facts_add_tags(g_facts, &tag_path, &tag_sym_kc3fs_file_stat,
                       &tag_file_stat2)) {
    err_puts("fs_getattr: facts_add_tags \"/\" :kc3fs_file_stat");
    return 1;
  }
  tag_clean(&tag_file_stat2);
  tag_init_str_1(&tag_path, NULL, "/README.md");
  tag_init_1(&tag_file_stat,
             "%FileStat{st_dev: (Uw) 1,"
             " st_ino: (Uw) 2,"
             " st_mode: [:r, :w],"
             " st_nlink: (Uw) 1,"
             " st_uid: (Uw) 0,"
             " st_gid: (Uw) 0,"
             " st_rdev: (Uw) 0,"
             " st_size: (Uw) 0,"
             " st_blksize: (Uw) 512,"
             " st_blocks: (Uw) 1,"
             " st_atim: Time.now(),"
             " st_ctim: %Time{tv_sec: 1735058914, tv_nsec: 4},"
             " st_mtim: %Time{tv_sec: 1735058914, tv_nsec: 4}}");
  eval_tag(&tag_file_stat, &tag_file_stat2);
  tag_clean(&tag_file_stat);
  if (! (size = struct_get_w(&tag_file_stat2.data.struct_,
                             sym_1("st_size")))) {
    err_puts("fs_getattr: struct_get_w(:st_size)");
    return 1;
  }
  *size = strlen(g_readme_md);
  if (! (blocks = struct_get_w(&tag_file_stat2.data.struct_,
                               sym_1("st_blocks")))) {
    err_puts("fs_getattr: struct_get_w(:st_blocks)");
    return 1;
  }
  *blocks = *size / 512;
  if (! facts_add_tags(g_facts, &tag_path, &tag_sym_kc3fs_file_stat,
                       &tag_file_stat2)) {
    err_puts("fs_getattr: facts_add_tags \"/README.md\""
             " :kc3fs_file_stat");
    return 1;
  }
  tag_clean(&tag_file_stat2);
  return 0;
}

int main (int argc, char **argv)
{
  int    argc1;
  char **argv1;
  struct fuse_args args;
  int r = 1;
  argc1 = argc;
  argv1 = argv;
  kc3_init(NULL, &argc1, &argv1);
  if (argc1 > 0 && argv1[0] && ! strcmp(argv1[0], "--trace")) {
    g_kc3_env->trace = true;
    argc1--;
    argv1++;
  }
  if ((r = fs_init()))
    goto clean;
  args = (struct fuse_args) FUSE_ARGS_INIT(argc, argv);
  if (fuse_opt_add_arg(&args, "-o") == -1) {
    err_puts("mount_kc3fs: fuse_opt_add_arg(-o)");
    r = 1;
    goto clean;
  }
  if (fuse_opt_add_arg(&args, "fuse_version=2.6") == -1) {
    err_puts("mount_kc3fs: fuse_opt_add_arg(fuse_version)");
    r = 1;
    goto clean;
  }
#ifdef __OpenBSD__
  r = fuse_main(args.argc, args.argv, &g_fsops, NULL);
#else
  r = fuse_main(args.argc, args.argv, &g_fsops);
#endif
  fuse_opt_free_args(&args);
 clean:
  kc3_clean(NULL);
  return r;
}
