#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "../libkc3/types.h"
#include "../libkc3/fact.h"
#include "../libkc3/tag.h"
#include "../libkc3/buf_inspect.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_file.h"
#include "../libkc3/str.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/env.h"
#include "../libkc3/facts.h"

static void render(s_buf *out, uw id, u8 action, s_fact *fact) {

    buf_inspect_uw(out, id);
    buf_write_1(out, action == FACT_ACTION_ADD ? " ADD " :
                     action == FACT_ACTION_REMOVE ? " REMOVE " : " REPLACE ");
    buf_inspect_fact(out, fact);
    buf_write_1(out, "\n");
}

int main (int argc, char **argv)
{
    char b[BUF_SIZE];
    uw id;
    u8 action;
    s_fact fact;
    s_str path;
    s_buf out;
    s_env env;
    int e_argc = 1;
    char *e_argv_data[] = { argv[0], NULL };
    char **e_argv = e_argv_data;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }
    {
        const char *fp = argv[1];
        size_t flen = strlen(fp);
        if (flen >= 5 && ! strcmp(fp + flen - 5, ".dump")) {
            char  *d_argv_data[] = { argv[0], "--restore", argv[1], NULL };
            int    d_argc = 3;
            char **d_argv = d_argv_data;
            if (! env_init(&env, &d_argc, &d_argv)) {
                fprintf(stderr, "%s: cannot restore env dump: %s\n", argv[0], argv[1]);
                return 1;
            }
            buf_init(&out, false, sizeof(b), b);
            buf_file_open_w(&out, stdout);
            facts_dump(env.facts, &out);
            buf_flush(&out);
            buf_file_close(&out);
            buf_clean(&out);
            env_clean(&env);
            return 0;
        }
        if (flen >= 5 && ! strcmp(fp + flen - 5, ".kc3c")) {
            s_marshall_read kmr = {0};
            uw n;
            uw i;
            if (! env_init(&env, &e_argc, &e_argv)) {
                fprintf(stderr, "%s: env_init failed\n", argv[0]);
                return 1;
            }
            str_init_1(&path, NULL, argv[1]);
            if (! marshall_read_init_file(&kmr, &path)) {
                fprintf(stderr, "%s: cannot open %s\n", argv[0], argv[1]);
                str_clean(&path);
                env_clean(&env);
                return 1;
            }
            marshall_read_size(&kmr);
            buf_init(&out, false, sizeof(b), b);
            buf_file_open_w(&out, stdout);
            if (! marshall_read_uw(&kmr, false, &n)) goto kc3c_done;
            i = 0;
            while (i < n) {
                s_str s = {0};
                if (! marshall_read_str(&kmr, false, &s)) goto kc3c_done;
                buf_write_1(&out, "dlopen ");
                buf_inspect_str(&out, &s);
                buf_write_1(&out, "\n");
                str_clean(&s);
                i++;
            }
            if (! marshall_read_uw(&kmr, false, &n)) goto kc3c_done;
            i = 0;
            while (i < n) {
                s_tag t = {0};
                if (! marshall_read_tag(&kmr, false, &t)) goto kc3c_done;
                buf_inspect_tag(&out, &t);
                buf_write_1(&out, "\n");
                tag_clean(&t);
                i++;
            }
        kc3c_done:
            buf_flush(&out);
            buf_file_close(&out);
            buf_clean(&out);
            marshall_read_clean(&kmr);
            str_clean(&path);
            env_clean(&env);
            return 0;
        }
    }
    {
        FILE *peek = fopen(argv[1], "rb");
        if (peek) {
            int first = fgetc(peek);
            fclose(peek);
            if (first == '%') {
                FILE *fp = fopen(argv[1], "rb");
                if (! fp) {
                    fprintf(stderr, "%s: cannot open %s\n", argv[0], argv[1]);
                    return 1;
                }
                char tbuf[BUF_SIZE];
                size_t n;
                while ((n = fread(tbuf, 1, sizeof(tbuf), fp)) > 0)
                    fwrite(tbuf, 1, n, stdout);
                fclose(fp);
                return 0;
            }
        }
    }
    if (! env_init(&env, &e_argc, &e_argv)) {
        fprintf(stderr, "%s: env_init failed\n", argv[0]);
        return 1;
    }
    str_init_1(&path, NULL, argv[1]);
    s_marshall_read mr = {0};
    sw r;
    if ( ! marshall_read_init_file(&mr, &path)) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        str_clean(&path);
        return 1;
    };
    buf_init(&out, false, sizeof(b), b);
    buf_file_open_w(&out, stdout);
    while (1) {
        while ((r = buf_peek_1(mr.buf, "KC3MARSH")) == 0) {
            if (buf_peek_1(mr.buf, "_KC3UW_") <= 0) goto done;
            if (! marshall_read_uw(&mr, false, &id) ||
            ! marshall_read_u8(&mr, false, &action) ||
            ! marshall_read_fact(&mr, false, &fact))
                goto done;
            render(&out, id, action, &fact);
            fact_clean_all(&fact);
        }
        if (r > 0) { marshall_read_chunk_file(&mr); continue;}
        break;
    }
    done:
    buf_flush(&out);
    buf_file_close(&out);
    marshall_read_clean(&mr);
    str_clean(&path);
    env_clean(&env);
    return 0;
}
