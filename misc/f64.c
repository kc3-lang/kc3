#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void) {
    const uint8_t bytes[8] = {0x18, 0x2d, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40};
    double d;

    /* Method 1: memcpy */
    memcpy(&d, bytes, 8);
    printf("memcpy:   %.17g\n", d);

    /* Method 2: pointer cast */
    d = *(double *)bytes;
    printf("cast:     %.17g\n", d);

    /* Print bytes */
    printf("bytes:    ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    return 0;
}
