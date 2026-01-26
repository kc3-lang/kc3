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
/**
 * @file buf_wineditline.h
 * @brief Wineditline backend for s_buf.
 */
#ifndef IC3_BUF_WINEDITLINE_H
#define IC3_BUF_WINEDITLINE_H

void    buf_wineditline_close (s_buf *buf, const char *history_path);
s_buf * buf_wineditline_open_r (s_buf *buf, const char *prompt,
				const char *history_path);

#endif /* IC3_BUF_WINEDITLINE_H */
