#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // --- ADD THIS BLOCK ---
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return 1;
    }
    // ----------------------

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(9999);
    address.sin_addr.s_addr = INADDR_ANY;

    // Cast struct sockaddr_in * to struct sockaddr *
    bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    listen(sockfd, 10);

    int clientfd = accept(sockfd, NULL, NULL);

    // stdin - 0
    struct pollfd fds[2] = {
        {
            .fd = 0,
            .events = POLLIN,
            .revents = 0
        },
        {
            .fd = clientfd,
            .events = POLLIN,
            .revents = 0
        }
    };

    for (;;) {
        char buffer[256] = { 0 };

        poll(fds, 2, 50000);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, 255);
            send(clientfd, buffer, 255, 0);
        } else if (fds[1].revents & POLLIN) {
            if (recv(clientfd, buffer, 255, 0) <= 0) {
                break;
            }

            printf("%s", buffer);
        }
    }

    close(clientfd);
    close(sockfd);
    return 0;
}
