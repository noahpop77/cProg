#include <sys/socket.h>
#include <stdio.h>


int main(){
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr address = {
        AF_INET,
        htons(9999),    // port used
        0
    };

    

    bind(socketfd, &address, sizeof(address));
    listen(socketfd, 10);
    int clientfd = accept(socketfd, 0, 0);

    poll
}

