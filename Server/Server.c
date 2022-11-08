#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../EncryptDecrypt.h"
#define SIZE 1024

// For CD Command
void Execute_CD_Server(char *DestinationDir, int server_sock, int policy)
{
    // Receive buffer contents from Client
    char buffer[SIZE];
    bzero(buffer, SIZE);
    recv(server_sock, buffer, sizeof(buffer), 0);
    decryptData(buffer, policy);

    if (chdir(DestinationDir) == 0)
    {
        printf("Change Directory Successful.\n");
    }
    else
    {
        perror("Change Directory failed.\n");
    }

    return;
}

// For DWD Command
void Execute_DWD_Server(FILE *fp, int sockfd, int client_sock, int policy)
{
    int n;
    char buffer[SIZE];
    bzero(buffer, SIZE);

    while (fgets(buffer, SIZE, fp) != NULL)
    {
        encryptData(buffer, policy);
        if (send(client_sock, buffer, sizeof(buffer), 0) == -1)
        {
            perror("[-]Error in sending file from Server. Demanaded DWD\n");
            exit(1);
        }
        bzero(buffer, SIZE);
    }
}

// For UPD Command
void Execute_UPD_Server(int client_sock, int policy)
{
    char buffer[SIZE];

    // UPD command will result in a file name "FileRecvByUpload.txt".
    FILE *fp;
    char *filename = "FileRecvByUpload.txt";

    // Open newly created file in write mode so as to write content of the buffer into the file.
    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("[-]Error in writing into file. Demanded UPD.\n");
        exit(1);
    }
    // Receive file data inside a buffer from Client side. Store buffer content into newly created file.
    int n;
    while (1)
    {
        n = recv(client_sock, buffer, SIZE, 0);
        if (n <= 0)
        {
            break;
            return;
        }
        decryptData(buffer, policy);
        fprintf(fp, "%s ", buffer);
        bzero(buffer, SIZE);
    }

    return;
}

// CWD
void Execute_CWD_Server(int server_sock, int client_sock, int policy)
{
    char cwd[SIZE];
    bzero(cwd, SIZE);
    recv(server_sock, cwd, sizeof(cwd), 0);
    // decryptData(cwd, policy);
    int n;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        while (1)
        {
            n = recv(server_sock, cwd, SIZE, 0);
            if (n <= 0)
            {
                break;
                return;
            }
        }
    }

    encryptData(cwd, policy);
    send(client_sock, cwd, strlen(cwd), 0);
}

// LS
void Execute_LS_Server(int server_sock, int client_sock, int policy)
{
    char buffer[SIZE];
    bzero(buffer, SIZE);
    recv(server_sock, buffer, sizeof(buffer), 0);

    FILE *fp;

    fp = popen("ls > ListFiles.txt", "r");
    char *filename = "ListFiles.txt";
    pclose(fp);

    FILE *fp1;
    fp1 = fopen(filename, "r");
    if (fp1 == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }
    int n;
    char data[SIZE] = {0};

    while (fgets(data, SIZE, fp1) != NULL)
    {
        send(client_sock, data, strlen(data), 0);
    }

    return;
}

int main(int argc, char *argv[])
{
    int policy;
    int port;
    if (argc == 3)
    {
        policy = atoi(argv[1]);
    port = atoi(argv[2]);
        printf("Expected Arguments: Executable, policy, port");
    }
    
    // char *ip = "127.0.0.1";
    // int port = 6001;
    

    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t addr_size;
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
    char inputgiven[1024];

    while (1)
    {
        listen(server_sock, 5);
        printf("Listening...\n");
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        printf("[+]Client connected.\n");

        bzero(buffer, 1024);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);
        strncpy(inputgiven, buffer, 64);

        bzero(buffer, 1024);
        strcpy(buffer, " ");
        printf("Server: %s\n", buffer);
        send(client_sock, buffer, strlen(buffer), 0);

        if (strncmp(inputgiven, "CWD", 3) == 0)
        {
            Execute_CWD_Server(server_sock, client_sock, policy);
        }
        else if (strncmp(inputgiven, "LS", 2) == 0)
        {
            Execute_LS_Server(server_sock, client_sock, policy);
        }
        else if (strncmp(inputgiven, "UPD", 3) == 0)
        {

            Execute_UPD_Server(client_sock, policy);
            break;
            printf("[+]Data written in the file successfully.\n");
        }
        else if (strncmp(inputgiven, "DWD", 3) == 0)
        {
            char *piece = strtok(inputgiven, " ");
            printf("%s\n", piece);

            char *fileName = strtok(NULL, "\n");
            printf("%s\n", fileName);

            FILE *fp;
            fp = fopen(fileName, "r");
            if (fp == NULL)
            {
                perror("[-]Error in reading file.");
                exit(1);
            }
            Execute_DWD_Server(fp, server_sock, client_sock, policy);
            break;
        }
        else if (strncmp(inputgiven, "CD", 2) == 0)
        {
            char *piece = strtok(inputgiven, " ");
            printf("%s\n", piece);

            char *DestinationDir = strtok(NULL, "\n");
            printf("%s\n", DestinationDir);

            Execute_CD_Server(DestinationDir, server_sock, policy);
        }
        close(client_sock);
        printf("[+]Client disconnected.\n\n");
    }

    return 0;
}