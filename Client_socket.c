/*simple client handler */
#include <stdio.h>
#include <string.h>
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>

#pragma comment(lib, "Ws2_32.lib") // add to information for compiler this link with Ws2 lib
short socketCreate(void)
{

    /*This call winsock*/
    WSADATA wsa;

    // /*initialize winsock with version 2.2 */
    printf("initialize winsock....\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed.Error Code:%d\n", WSAGetLastError());
        return 1;
    }
    printf("winsock initialize successfully  !\n");

    /*create a socket*/
    short hSocket = 0;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET tell the OS that use for IPV4 protocol family mapping to the networking subsystem of the OS, SOCK_STREAM is type of socket want to create used for TCP comunication
    return hSocket;
}
int socketConnect(int hSocket)
{

    int iRetVal = -1;
    int serverPort = 12345; // must be greater than system port
    struct sockaddr_in remote = {0};

    remote.sin_family = AF_INET;                     // AF_INET tell the OS that use for IPV4 protocol family mapping to the networking subsystem of the OS
    remote.sin_addr.s_addr = inet_addr("127.0.0.1"); // local server
    remote.sin_port = htons(serverPort);             // any incomming interface with local port

    iRetVal = connect(hSocket, (struct sockaddr *)&remote, sizeof(remote));

    return iRetVal;
}
int socketSend(int hSocket, char *pMessage, short lenRqst)

{
    int shortReval = -1;
    struct timeval tv;
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    if (setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    {
        perror("time out");
        return -1;
    }
    shortReval = send(hSocket, pMessage, lenRqst, 0);
    return shortReval;
}

int revcDatafrom_server(int hSocket, char *Rsp, short RevcSize)
{
    int shortReval = -1;
    struct timeval tv;
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    if (setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    {
        perror("time out");
        return -1;
    }
    shortReval = recv(hSocket, Rsp, RevcSize, 0);
    printf("Response from server :%s\n", Rsp);
    return shortReval;
}
int main(int argc, char *argv[])
{
    int hSocket = 0;
    int read_size = 0;
    struct sockaddr_in server;
    char sendtoServer[200] = {0};
    char serverReply[200] = {0};

    hSocket = socketCreate();
    if (hSocket == -1)
    {
        printf("Failed to create socket\n");
        return -1;
    }
    printf("Socket Created...\n");
    if (socketConnect(hSocket) < 0)
    {
        printf("Failed to connect\n");
        return -1;
    }

    printf("Connected...\n");
    printf("Enter your message: ");
    fgets(sendtoServer, 100, stdin);
    /*send to servers*/
    socketSend(hSocket, sendtoServer, strlen(sendtoServer));
    /*receive from servers */
    read_size = revcDatafrom_server(hSocket, serverReply, 200);
    if (read_size < 0)
    {
        printf("Failed to receive\n");
        return -1;
    }
    printf("Response from server :%s\n", serverReply);
    close(hSocket);

    return 0;
}
