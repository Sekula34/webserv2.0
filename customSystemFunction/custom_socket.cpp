#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int socket_call_count = 0;
#define SOCKET_FAIL_ON_CALL 15 // Customize to fail on the nth call

// Define a function pointer type for the real socket
typedef int (*socket_t)(int, int, int);
static socket_t real_socket = NULL;

int socket(int domain, int type, int protocol) {
    if (!real_socket) {
        // Load the actual socket function
        real_socket = (socket_t)dlsym(RTLD_NEXT, "socket");
        if (!real_socket) {
            const char *error_msg = "Error locating the real socket\n";
            write(2, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
    }

    socket_call_count++;
    const char *msg = "Intercepted socket\n";
    write(1, msg, strlen(msg));  // Optional debug print

    // Fail on the specified call number
    if (socket_call_count % 5 == 1) {
        const char *fail_msg = "Simulated socket failure\n";
        write(1, fail_msg, strlen(fail_msg));
        return -1;  // Simulate failure by returning -1
    }

    // Call the real socket function
    return real_socket(domain, type, protocol);
}
