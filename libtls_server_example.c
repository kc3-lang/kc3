#include <sys/socket.h>
#include <netinet/in.h>
#include <tls.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    struct tls_config *config;
    struct tls *ctx;
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[1024];
    ssize_t len;
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }
    
    // Bind to port 8443
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8443);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }
    
    listen(server_fd, 5);
    
    // Initialize TLS
    if (tls_init() != 0) {
        fprintf(stderr, "tls_init failed\n");
        exit(1);
    }
    
    // Create TLS configuration
    config = tls_config_new();
    if (!config) {
        fprintf(stderr, "tls_config_new failed\n");
        exit(1);
    }
    
    // Set server certificate and key
    if (tls_config_set_cert_file(config, "/etc/ssl/server.crt") != 0) {
        fprintf(stderr, "Failed to set cert: %s\n", tls_config_error(config));
        exit(1);
    }
    
    if (tls_config_set_key_file(config, "/etc/ssl/server.key") != 0) {
        fprintf(stderr, "Failed to set key: %s\n", tls_config_error(config));
        exit(1);
    }
    
    // Create TLS server context
    ctx = tls_server();
    if (!ctx) {
        fprintf(stderr, "tls_server failed\n");
        exit(1);
    }
    
    if (tls_configure(ctx, config) != 0) {
        fprintf(stderr, "tls_configure: %s\n", tls_error(ctx));
        exit(1);
    }
    
    printf("TLS server listening on port 8443\n");
    
    // Accept connections
    while ((client_fd = accept(server_fd, NULL, NULL)) >= 0) {
        struct tls *client_ctx;
        
        // Accept TLS handshake on client socket
        if (tls_accept_socket(ctx, &client_ctx, client_fd) != 0) {
            fprintf(stderr, "tls_accept_socket: %s\n", tls_error(ctx));
            close(client_fd);
            continue;
        }
        
        printf("TLS connection established\n");
        
        // Read client data
        len = tls_read(client_ctx, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';
            printf("Received: %s", buffer);
            
            // Send response
            const char *response = "HTTP/1.1 200 OK\r\n"
                                 "Content-Length: 13\r\n"
                                 "Content-Type: text/plain\r\n\r\n"
                                 "Hello, TLS!\r\n";
            tls_write(client_ctx, response, strlen(response));
        }
        
        // Close TLS connection
        tls_close(client_ctx);
        tls_free(client_ctx);
        close(client_fd);
    }
    
    tls_free(ctx);
    tls_config_free(config);
    close(server_fd);
    return 0;
}

/*
 * Compile with:
 * cc -o tlsserver libtls_server_example.c -ltls -lssl -lcrypto
 *
 * Generate test certificates:
 * openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes
 */
