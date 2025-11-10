/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../../../libkc3/types.h"
#include <windows.h>
#include <xkbcommon/xkbcommon.h>

u32 vk_to_xkbcommon (u32 vk_key)
{
  switch (vk_key) {
  case VK_ESCAPE: return XKB_KEY_Escape;
  case VK_F1:     return XKB_KEY_F1;
  case VK_F2:     return XKB_KEY_F2;
  case VK_F3:     return XKB_KEY_F3;
  case VK_F4:     return XKB_KEY_F4;
  case VK_F5:     return XKB_KEY_F5;
  case VK_F6:     return XKB_KEY_F6;
  case VK_F7:     return XKB_KEY_F7;
  case VK_F8:     return XKB_KEY_F8;
  case VK_F9:     return XKB_KEY_F9;
  case VK_F10:    return XKB_KEY_F10;
  case VK_F11:    return XKB_KEY_F11;
  case VK_F12:    return XKB_KEY_F12;
    /*
  case : return XKB_KEY_grave;
    */
  case 0x31:      return XKB_KEY_1;
  case 0x32:      return XKB_KEY_2;
  case 0x33:      return XKB_KEY_3;
  case 0x34:      return XKB_KEY_4;
  case 0x35:      return XKB_KEY_5;
  case 0x36:      return XKB_KEY_6;
  case 0x37:      return XKB_KEY_7;
  case 0x38:      return XKB_KEY_8;
  case 0x39:      return XKB_KEY_9;
  case 0x30:      return XKB_KEY_0;
    /*
  case    45: return XKB_KEY_minus;
  case    61: return XKB_KEY_equal;
    */
  case VK_BACK:   return XKB_KEY_BackSpace;
  case VK_TAB:    return XKB_KEY_Tab;
  case 0x41:      return XKB_KEY_a;
  case 0x42:      return XKB_KEY_b;
  case 0x43:      return XKB_KEY_c;
  case 0x44:      return XKB_KEY_d;
  case 0x45:      return XKB_KEY_e;
  case 0x46:      return XKB_KEY_f;
  case 0x47:      return XKB_KEY_g;
  case 0x48:      return XKB_KEY_h;
  case 0x49:      return XKB_KEY_i;
  case 0x4A:      return XKB_KEY_j;
  case 0x4B:      return XKB_KEY_k;
  case 0x4C:      return XKB_KEY_l;
  case 0x4D:      return XKB_KEY_m;
  case 0x4E:      return XKB_KEY_n;
  case 0x4F:      return XKB_KEY_o;
  case 0x50:      return XKB_KEY_p;
  case 0x51:      return XKB_KEY_q;
  case 0x52:      return XKB_KEY_r;
  case 0x53:      return XKB_KEY_s;
  case 0x54:      return XKB_KEY_t;
  case 0x55:      return XKB_KEY_u;
  case 0x56:      return XKB_KEY_v;
  case 0x57:      return XKB_KEY_w;
  case 0x58:      return XKB_KEY_x;
  case 0x59:      return XKB_KEY_y;
  case 0x5A:      return XKB_KEY_z;
    /*
  case    91: return XKB_KEY_bracketleft;
  case    93: return XKB_KEY_bracketright;
  case    92: return XKB_KEY_backslash;
  case    59: return XKB_KEY_semicolon;
  case    39: return XKB_KEY_apostrophe;
    */
  case VK_RETURN: return XKB_KEY_Return;
    /*
  case    44: return XKB_KEY_comma;
  case    46: return XKB_KEY_period;
  case    47: return XKB_KEY_slash;
    */
  case VK_SPACE:  return XKB_KEY_space;
  case VK_UP:     return XKB_KEY_Up;
  case VK_DOWN:   return XKB_KEY_Down;
  case VK_LEFT:   return XKB_KEY_Left;
  case VK_RIGHT:  return XKB_KEY_Right;
  default: break;
  }
  return XKB_KEY_NoSymbol;
}
