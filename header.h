#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "die_with_error.c"

#define PORT 8081
#define MAX_BUFFER_SIZE 1024
#define MAX_DATA_SIZE 200

typedef struct {
    int id;
    int row_number;
    int bookshelf_number;
    int book_number;
    int code;
} Book;

typedef struct {
    size_t books_amount;
    Book* books;
} Library;