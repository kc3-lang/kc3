#include <stdio.h>
#include "libkc3/types.h"

void display_min_max() {
    // Display minimum and maximum values for integer types
    printf("Minimum and Maximum values for integer types:\n");
    printf("S8_MIN: %d\n", S8_MIN);
    printf("S8_MAX: %d\n", S8_MAX);
    printf("S16_MIN: %d\n", S16_MIN);
    printf("S16_MAX: %d\n", S16_MAX);
    printf("S32_MIN: %d\n", S32_MIN);
    printf("S32_MAX: %d\n", S32_MAX);
    printf("S64_MIN: %ld\n", S64_MIN);
    printf("S64_MAX: %ld\n", S64_MAX);
    printf("SW_MIN: %ld\n", SW_MIN);
    printf("SW_MAX: %ld\n", SW_MAX);
}

int main() {
    display_min_max();
    return 0;
}