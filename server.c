#include "header.h"

void sortBooksById(Book books[], int len) {
    int i, j;
    Book temp;

    for (i = 0; i < len; i++) {
        for (j = i + 1; j < len; j++) {
            if (books[i].id > books[j].id) {
                temp = books[i];
                books[i] = books[j];
                books[j] = temp;
            }
        }
    }
}

void printArr(Book books[], int len) {
    for (size_t i = 0; i < len; i++) {
        printf("Row: %d Bookshelf: %d Book: %d ID: %d\n",
        books[i].row_number, books[i].bookshelf_number, books[i].book_number, books[i].id);
    }
}

int main(int argc, char *argv[]) {
    // Создание сокета
    int sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // Если кол-во аргументов командной строки равно 2 (не считая нулевой),
    // то адрес сервера и порт задаются исходя из введенных аргументов.
    if (argc == 3 ) {
        // Номер порта
        server_addr.sin_port = htons(atoi(argv[2]));
        // Адрес сервера
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }

    // Привязка сокета к адресу сервера
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Получение данных от клиента и запись в массив
    Book received_data;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    int data_count = 0;
    Book data_array[MAX_DATA_SIZE];

    while (1) {
        if (data_count == MAX_DATA_SIZE) {
            printf("Каталог заполнен");
            break;
        }

        client_len = sizeof(client_addr);

        memset(buffer, 0, MAX_BUFFER_SIZE);
        int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        memcpy(&received_data, buffer, sizeof(received_data));
        sleep(2);
        if (received_data.code == 2) {
            printf("Observer requested library data.\n");
            sortBooksById(data_array, data_count);
            for (size_t i = 0; i < data_count - 1; i++) {
                Book sendbook = data_array[i];
                sendbook.code = 5;
                // printf("Book has been sent to observer: ");
                // printf("%d %d %d %d\n", sendbook.id, sendbook.row_number, sendbook.bookshelf_number, sendbook.book_number);
                memset(buffer, 0, MAX_BUFFER_SIZE);
                memcpy(buffer, &sendbook, sizeof(sendbook));
                sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            }


            Book last_book = data_array[data_count - 1];
            last_book.code = 6;
            memset(buffer, 0, MAX_BUFFER_SIZE);
            memcpy(buffer, &last_book, sizeof(last_book));
            sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            
            printf("Library data were send to observer.\n");
        } else if (received_data.code == 0) {
            Book message;
            message.code = 1;

            memset(buffer, 0, MAX_BUFFER_SIZE);
            memcpy(buffer, &message, sizeof(message));
        
            sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        
            data_array[data_count] = received_data;
            data_count++;
            printf("Received data from client: ");
            printf("%d %d %d %d\n", received_data.id, received_data.row_number, received_data.bookshelf_number, received_data.book_number);
        }

        // sortBooksById(data_array, data_count);
        // printArr(data_array, data_count);
    }

    close(sockfd);
    return 0;
}