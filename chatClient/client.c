#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

int main() {
    // Create socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /*
    struct sockaddr_in {
        __uint8_t       sin_len;        
        sa_family_t     sin_family;
        in_port_t       sin_port;
        struct  in_addr sin_addr;
        char            sin_zero[8];
    };
    */
    
    // Initializes a zeroed out sockaddr_in struct
    struct sockaddr_in address = {0};
    
    // Pre processor constant set to 2, describes TCP/UDP connection family
    address.sin_family = AF_INET;

    // Port used for address communication
    address.sin_port = htons(9999);
    
    // Connecting to localhost. Change this if your server is elsewhere.
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Cast struct sockaddr_in * to struct sockaddr *
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("connect failed");
        return 1;
    }

    // stdin - 0
    struct pollfd fds[2] = {
        {
            .fd = 0,
            .events = POLLIN,
            .revents = 0
        },
        {
            .fd = sockfd,
            .events = POLLIN,
            .revents = 0
        }
    };

    for (;;) {
        char buffer[256] = { 0 };

        poll(fds, 2, 50000);

        if (fds[0].revents & POLLIN) {
            ssize_t bytes_read = read(0, buffer, 255);
            if (bytes_read > 0) {
                send(sockfd, buffer, bytes_read, 0);
            }
        } else if (fds[1].revents & POLLIN) {
            ssize_t bytes_received = recv(sockfd, buffer, 255, 0);
            if (bytes_received <= 0) {
                break; // Server disconnected or error
            }

            printf("%s", buffer);
        }
    }

    close(sockfd);
    return 0;
}
