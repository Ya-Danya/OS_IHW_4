#include "header.h"

void printArr(Book *books, int len) {
    for (size_t i = 0; i < len; i++) {
        printf("Row: %d Bookshelf: %d Book: %d ID: %d iter: %ld\n",
        books[i].row_number, books[i].bookshelf_number, books[i].book_number, books[i].id, i);
    }
    printf("All books written\n");
}

int main(int argc, char *argv[]) {
    unsigned int sizeOfBook = sizeof(Book);      /* Length of string to echo */
    char echoBuffer[sizeOfBook];     /* Buffer for echo string */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() */
    // Указатель на файл.
    FILE *fpc;
    // Название файла ввода.
    char* filename;
    // Если название не указано в аргументах командной строки, осуществляется ввод из консоли
    
    if (argc == 2) {
        filename = argv[1];
    } else {
        printf("Incorrect amount of comand line args.\n");
    }

    FILE *fp;
    // Открываем файл
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    // Создание и заполнение каталога книгами.
    Book books[1000];
    //books = (Book*) malloc(books_amount);
    int book_number = 0;
    while (fscanf(fp, "%d %d %d %d", &books[book_number].row_number, &books[book_number].bookshelf_number, &books[book_number].book_number, &books[book_number].id) != EOF) {
        books[book_number].code = 0;
        ++book_number;
    }

    fclose(fp);
    printf("Input done, amount of books is %d.\n", book_number);

    printArr(books, book_number);
    
    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    struct sockaddr_in fromAddr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    if (argc == 4) {
        // Номер порта
        server_addr.sin_port = htons(atoi(argv[2]));
        // Адрес сервера
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }
    
    int iterator = 0;
            
    printf("Server configurated\n");
            
    int sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Sending books begin\n");
    if (book_number > 1) {
        do {
            int msg_received = 0;
            do {
                char buffer[MAX_BUFFER_SIZE];
                memset(buffer, 0, MAX_BUFFER_SIZE);
                memcpy(buffer, &books[iterator], sizeof(books[iterator]));
                        
                sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                printf("Книга %d отправлена\n", books[iterator].id);
                
                memset(buffer, 0, MAX_BUFFER_SIZE);
                unsigned int fromAddrSize = sizeof(fromAddr);
                
                Book received_data;
                int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&fromAddr, &fromAddrSize);
                printf("iterator %d book number %d \n", iterator, book_number);
                memcpy(&received_data, buffer, sizeof(received_data));
                
                printf("Клиент получил сообщение о том, что статус отправленных данных - %d \n",received_data.code);
                msg_received = received_data.code;

            } while (msg_received != 1);

            ++iterator;
            printf("iterator %d book number %d \n", iterator, book_number);
        } while (iterator < book_number);
        close(sockfd);
        exit(0);
    }
    
    return 0;
}