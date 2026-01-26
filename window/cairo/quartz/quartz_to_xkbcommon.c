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
#include <xkbcommon/xkbcommon.h>
#include "../../../libkc3/types.h"

u32 quartz_to_xkbcommon (u16 quartz_key)
{
  switch (quartz_key) {
  case    27: return XKB_KEY_Escape;
  case 63236: return XKB_KEY_F1;
  case 63237: return XKB_KEY_F2;
  case 63238: return XKB_KEY_F3;
  case 63239: return XKB_KEY_F4;
  case 63240: return XKB_KEY_F5;
  case 63241: return XKB_KEY_F6;
  case 63242: return XKB_KEY_F7;
  case 63243: return XKB_KEY_F8;
  case 63244: return XKB_KEY_F9;
  case 63245: return XKB_KEY_F10;
  case 63246: return XKB_KEY_F11;
  case 63247: return XKB_KEY_F12;
  case    96: return XKB_KEY_grave;
  case    49: return XKB_KEY_1;
  case    50: return XKB_KEY_2;
  case    51: return XKB_KEY_3;
  case    52: return XKB_KEY_4;
  case    53: return XKB_KEY_5;
  case    54: return XKB_KEY_6;
  case    55: return XKB_KEY_7;
  case    56: return XKB_KEY_8;
  case    57: return XKB_KEY_9;
  case    48: return XKB_KEY_0;
  case    45: return XKB_KEY_minus;
  case    61: return XKB_KEY_equal;
  case   127: return XKB_KEY_BackSpace;
  case     9: return XKB_KEY_Tab;
  case    97: return XKB_KEY_a;
  case    98: return XKB_KEY_b;
  case    99: return XKB_KEY_c;
  case   100: return XKB_KEY_d;
  case   101: return XKB_KEY_e;
  case   102: return XKB_KEY_f;
  case   103: return XKB_KEY_g;
  case   104: return XKB_KEY_h;
  case   105: return XKB_KEY_i;
  case   106: return XKB_KEY_j;
  case   107: return XKB_KEY_k;
  case   108: return XKB_KEY_l;
  case   109: return XKB_KEY_m;
  case   110: return XKB_KEY_n;
  case   111: return XKB_KEY_o;
  case   112: return XKB_KEY_p;
  case   113: return XKB_KEY_q;
  case   114: return XKB_KEY_r;
  case   115: return XKB_KEY_s;
  case   116: return XKB_KEY_t;
  case   117: return XKB_KEY_u;
  case   118: return XKB_KEY_v;
  case   119: return XKB_KEY_w;
  case   120: return XKB_KEY_x;
  case   121: return XKB_KEY_y;
  case   122: return XKB_KEY_z;
  case    91: return XKB_KEY_bracketleft;
  case    93: return XKB_KEY_bracketright;
  case    92: return XKB_KEY_backslash;
  case    59: return XKB_KEY_semicolon;
  case    39: return XKB_KEY_apostrophe;
  case    13: return XKB_KEY_Return;
  case    44: return XKB_KEY_comma;
  case    46: return XKB_KEY_period;
  case    47: return XKB_KEY_slash;
  case 63232: return XKB_KEY_Up;
  case 63233: return XKB_KEY_Down;
  case 63234: return XKB_KEY_Left;
  case 63235: return XKB_KEY_Right;
  default: break;
  }
  return XKB_KEY_NoSymbol;
}
