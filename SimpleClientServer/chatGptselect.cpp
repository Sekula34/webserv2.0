#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
//#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 10

int make_socket_non_blocking(int sfd) {
    int flags, s;
    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int main() {
    int listen_fd, new_fd, max_fd, i;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    fd_set read_fds, write_fds, master_read_fds, master_write_fds;
    char buffer[1024];
    int clients[MAX_CLIENTS] = {-1}; // Initialize all client fds to -1 (no client)

    // Create and set up the listening socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (make_socket_non_blocking(listen_fd) == -1) {
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // Initialize the master fd sets
    FD_ZERO(&master_read_fds);
    FD_ZERO(&master_write_fds);
    FD_SET(listen_fd, &master_read_fds);
    max_fd = listen_fd;

    while (1) {
        // Copy the master sets to the working sets
        read_fds = master_read_fds;
        write_fds = master_write_fds;

        // Call select
        if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check for new incoming connections
        if (FD_ISSET(listen_fd, &read_fds)) {
            cli_len = sizeof(cli_addr);
            new_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
            if (new_fd < 0) {
                perror("accept");
                continue;
            }

            if (make_socket_non_blocking(new_fd) == -1) {
                close(new_fd);
                continue;
            }

            // Add the new client to the master read fd set
            FD_SET(new_fd, &master_read_fds);
            if (new_fd > max_fd) {
                max_fd = new_fd;
            }

            // Add the new client to the clients array
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == -1) {
                    clients[i] = new_fd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients\n");
                close(new_fd);
            }
        }

        // Check for data from clients
        for (i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i];
            if (fd == -1) {
                continue;
            }

            if (FD_ISSET(fd, &read_fds)) {
                int n = read(fd, buffer, sizeof(buffer));
                if (n <= 0) {
                    if (n == 0) {
                        // Connection closed
                        printf("Connection closed by client\n");
                    } else {
                        perror("read");
                    }
                    close(fd);
                    FD_CLR(fd, &master_read_fds);
                    clients[i] = -1;
                } else {
                    // Handle incoming data (for example, echo it back)
                    buffer[n] = '\0';
                    printf("Received: %s\n", buffer);
                    FD_SET(fd, &master_write_fds); // Add to write set to echo back
                }
            }

            if (FD_ISSET(fd, &write_fds)) {
                // Echo back the data to the client
                int n = write(fd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("write");
                }
                FD_CLR(fd, &master_write_fds); // Remove from write set after sending
            }
        }
    }

    close(listen_fd);
    return 0;
}
