#include "header.h"

void printArr(Book books[], int len) {
    for (size_t i = 0; i < len; i++) {
        printf("Row: %d Bookshelf: %d Book: %d ID: %d\n",
        books[i].row_number, books[i].bookshelf_number, books[i].book_number, books[i].id);
    }
    printf("All books written\n");
}


int main(int argc, char *argv[]) {
    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    struct sockaddr_in fromAddr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    if (argc == 3 ) {
        // Номер порта
        server_addr.sin_port = htons(atoi(argv[2]));
        // Адрес сервера
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }

    Book library[1000];
            
    printf("Server adres configurated\n");
    char buffer[MAX_BUFFER_SIZE];
    int sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Sending books begin\n");
    // int msg_received = 0;
    size_t rcvd_msg_amount = 0;
    size_t iterator = 0;
    do {
        iterator = 0;
        Book message;
        message.code = 2;
        memset(buffer, 0, MAX_BUFFER_SIZE);
        memcpy(buffer, &message, sizeof(message));
                        
        sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        printf("Library request has been sent\n");
                
        do {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            unsigned int fromAddrSize = sizeof(fromAddr);
            int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&fromAddr, &fromAddrSize);
            memcpy(&library[iterator], buffer, bytes_received);
            ++iterator;
        } while (library[iterator - 1].code != 6);
        printArr(library, iterator);
        ++rcvd_msg_amount;
        sleep(5);
    } while (rcvd_msg_amount < 6);
    
    close(sockfd);
    
    return 0;
}