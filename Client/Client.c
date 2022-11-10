#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#define SIZE 1024
#include "../EncryptDecrypt.h"

void Execute_CD(int sockfd, int policy)
{
    char buffer[SIZE];
    bzero(buffer, SIZE);
    encryptData(buffer, policy);
    send(sockfd, buffer, strlen(buffer), 0);
}

// For download;
void Execute_DWD(int sockfd, int policy)
{
    char buffer[SIZE];
    int n;

    // Download file declaration. The download file will be saved by name "DownloadFromServerSide.txt".
    FILE *fp;
    char *filename = "DownloadFromServerSide.txt";

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("[-]Error in writing file for DWD");
        exit(1);
    }

    // while (1)
    // {
    //     n = recv(sockfd, buffer, SIZE, 0);
    //     if (n <= 0)
    //     {
    //         break;
    //         return;
    //     }
    //     decryptData(buffer, policy);
    //     fprintf(fp, "%s", buffer);
    //     bzero(buffer, SIZE);
    // }
    int flag = 0; /* File read finished or not. */
    // while (1)
    // {
        /* Clear the buffer. */
        for (int i = 0; i < SIZE; i++)
        {
            buffer[i] = '\0';
        }

        /* Receive file. */
        int recv_try = recv(sockfd, buffer, SIZE, 0);
        if (recv_try == -1)
        {
            return;
        }

        /* Fill the buffer. */
        for (int i = 0; i < SIZE; i++)
        {
            if (buffer[i] == '\0')
            {
                flag = 1;
                break;
            }
            fputc(buffer[i], fp);
        }

        // if (flag)
        // {
        //     break;
        // }
    // }
    fclose(fp);
    return;
}

// For Uploading
void Execute_UPD(FILE *fp, int sockfd, int policy)
{

    int n;
    char data[SIZE] = {0};

    // while (fgets(data, SIZE, fp) != NULL)
    // {
    //     encryptData(data, policy);
    //     if (send(sockfd, data, sizeof(data), 0) == -1)
    //     {
    //         perror("[-]Error in uploading file\n");
    //         exit(1);
    //     };
    //     bzero(data, SIZE);
    // }

    int flag = 0;
    // while (1)
    // {
        char buffer[SIZE];
        bzero(buffer, SIZE);

        for (int i = 0; i < SIZE; i++)
        {
            buffer[i] = fgetc(fp);
            if (buffer[i] == '\0')
            {
                buffer[i] = '\0 ';
                flag = 1;
                break;
            }
        }
        int send_try = send(sockfd, buffer, SIZE, 0);
        if (send_try == -1)
        {
            return;
        }

    //     if (flag)
    //     {
    //         break;
    //     }
    // }
    fclose(fp);
}

// For listing files(LS COMMAND)
void Execute_LS(int sockfd, int policy)
{
    char buffer[SIZE];
    bzero(buffer, SIZE);

    send(sockfd, buffer, strlen(buffer), 0);

    int n;
    // while (1)
    // {
    n = recv(sockfd, buffer, sizeof(buffer), 0);

    // if (n <= 0)
    // {
    //     break;
    //     return;
    // }
    // }

    printf("List of files are :\n%s", buffer);
    return;
}

// For Fetching Current Working Directory.
void Execute_CWD(int sockfd, int policy)
{
    char buffer[SIZE];
    bzero(buffer, SIZE);

    // encryptData(buffer, policy);
    send(sockfd, buffer, strlen(buffer), 0);
    // printf("Message sent from Client\n");

    int n;
    // while (1)
    // {
    n = recv(sockfd, buffer, sizeof(buffer), 0);
    // if (n <= 0)
    // {
    //     break;
    //     return;
    // }
    // }

    decryptData(buffer, policy);
    printf("Current Directory is: %s\n", buffer);
    // return;
}

// Take input
void TakeInput(int sockfd, int policy)
{
    char buffer[1024];

    char input[SIZE];
    bzero(buffer, 1024);
    int n;
    while (1)
    {
        printf("Enter command : ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n')
            ;

        strncpy(input, buffer, 64);

        printf("Command Typed : %s\n", buffer);
        send(sockfd, buffer, strlen(buffer), 0);

        bzero(buffer, 1024);
        recv(sockfd, buffer, sizeof(buffer), 0);
        // printf("Message received from Server: %s\n", buffer);

        if (strncmp(input, "CWD", 3) == 0)
        {
            Execute_CWD(sockfd, policy);
        }
        else if (strncmp(input, "LS", 2) == 0)
        {
            Execute_LS(sockfd, policy);
        }
        else if (strncmp(input, "UPD", 3) == 0)
        {
            char *part = strtok(input, " ");
            printf("%s\n", part);

            char *fileName = strtok(NULL, "\n");
            printf("%s\n", fileName);

            FILE *fp;
            fp = fopen(fileName, "r");
            if (fp == NULL)
            {
                perror("[-]Error in reading file.");
                exit(1);
            }
            Execute_UPD(fp, sockfd, policy);
            printf("[+]File data sent successfully.\n");
        }
        else if (strncmp(input, "DWD", 3) == 0)
        {

            Execute_DWD(sockfd, policy);
            // break;
        }
        else if (strncmp(input, "CD", 2) == 0)
        {

            Execute_CD(sockfd, policy);
        }
        else if (strncmp(input, "EXIT", 4) == 0)
        {
            exit(1);
        }
    }
    close(sockfd);
    printf("Disconnected from the server.\n");
}

int main(int argc, char *argv[])
{
    int policy;
    char *ip;
    int port;
    if (argc == 4)
    {
        policy = atoi(argv[1]);
        ip = argv[2];
        port = atoi(argv[3]);
        // printf("Error in giving the arguments. Expected arguments in order: Executable, Policy, Ip, Port");
    }

    // char *ip = "127.0.0.1";
    // int port = 6001;
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[SIZE];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the server.\n");

    TakeInput(sock, policy);

    return 0;
}