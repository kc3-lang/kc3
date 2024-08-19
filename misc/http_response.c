#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../http/types.h"

// Function to calculate and print offsets
void print_offsets() {
    printf("Offset of protocol: %zu\n", offsetof(struct http_response, protocol));
    printf("Offset of code: %zu\n", offsetof(struct http_response, code));
    printf("Offset of message: %zu\n", offsetof(struct http_response, message));
    printf("Offset of headers: %zu\n", offsetof(struct http_response, headers));
    printf("Offset of body: %zu\n", offsetof(struct http_response, body));
}

// Main function
int main() {
    // Print offsets
    print_offsets();

    return 0;
}