#include "../libkc3/kc3.h"

void display_min_max() {
  s8 s8_min = S8_MIN;
  s8 s8_max = S8_MAX;
  s16 s16_min = S16_MIN;
  s16 s16_max = S16_MAX;
  s32 s32_min = S32_MIN;
  s32 s32_max = S32_MAX;
  s64 s64_min = S64_MIN;
  s64 s64_max = S64_MAX;
  sw sw_min = SW_MIN;
  sw sw_max = SW_MAX;
    // Display minimum and maximum values for integer types
  io_puts("Minimum and Maximum values for integer types:");
  io_write_1("S8_MIN:  ");
  io_inspect_s8_hexadecimal(&s8_min);
  io_write_1("\nS8_MAX:   ");
  io_inspect_s8_hexadecimal(&s8_max);
  io_write_1("\nS16_MIN: ");
  io_inspect_s16_hexadecimal(&s16_min);
  io_write_1("\nS16_MAX:  ");
  io_inspect_s16_hexadecimal(&s16_max);
  io_write_1("\nS32_MIN: ");
  io_inspect_s32_hexadecimal(&s32_min);
  io_write_1("\nS32_MAX:  ");
  io_inspect_s32_hexadecimal(&s32_max);
  io_write_1("\nS64_MIN: ");
  io_inspect_s64_hexadecimal(&s64_min);
  io_write_1("\nS64_MAX:  ");
  io_inspect_s64_hexadecimal(&s64_max);
  io_write_1("\nSW_MIN:  ");
  io_inspect_sw_hexadecimal(&sw_min);
  io_write_1("\nSW_MAX:   ");
  io_inspect_sw_hexadecimal(&sw_max);
  io_write_1("\n");
}

int main (int argc, char **argv) {
  kc3_init(NULL, &argc, &argv);
  display_min_max();
  kc3_clean(NULL);
  return 0;
}
