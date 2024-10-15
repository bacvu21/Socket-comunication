/*simple server handler */
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

/*create short reason that socket does not need require a lot of space*/
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
int bindCreateSocket(int hSocket)
{
	int iRetVal = -1;		// value return
	int clientPort = 12345; // must be greater than system port

	struct sockaddr_in remote = {0}; // create a socket struct

	remote.sin_family = AF_INET;				/**internet address family */
	remote.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY tell the OS that use for all network interfaces
	remote.sin_port = htons(clientPort);		/**any incomming interface with local port*/

	iRetVal = bind(hSocket, (struct sockaddr *)&remote, sizeof(remote));
	return iRetVal;
}
int main(void)
{
	int socket_des = 0;
	int sock = 0;
	int clientLent = 0;
	struct sockaddr_in client = {0};
	char clientMessage[200] = {0};
	char message[100] = {0};
	const char *pMessage = "hello from Bac host ";
	/*call socketCreate to create socket*/
	socket_des = socketCreate();
	if (socket_des == -1)
	{
		printf("Could not create socket ");
		return 1;
	}
	printf("Socket Created...\n");

	/**Bind */
	if (bindCreateSocket(socket_des) < 0)
	{
		perror("bind failed");
		return 1;
	}
	printf("Bind done...\n");
	/* listen */
	listen(socket_des, 3);
	while (1)
	{
		printf("waiting for incoming connections...\n");
		clientLent = sizeof(struct sockaddr_in);
		/**Accept connections from an incoming client */
		sock = accept(socket_des, (struct sockaddr *)&client, (int *)&clientLent);
		if (sock < 0)
		{
			perror("accepted failed");
			return 1;
		}
		printf("Connection accepted \n");

		memset(clientMessage, '\0', sizeof clientMessage);
		memset(message, '\0', sizeof message);

		/*Receive message from client*/
		clientLent = recv(sock, clientMessage, sizeof clientMessage, 0);
		if (clientLent < 0)
		{
			perror("recv failed");
			break;
		}
		printf("Message from client: %s\n", clientMessage);

		/*Recv a replay from the client */
		if (strcmp(pMessage, clientMessage) == 0)
		{
			strcpy(message, "Hi there!");
		}
		else
		{
			strcpy(message, "Invalid message");
		}
		// send message to client
		if (send(sock, message, strlen(message), 0) < 0)
		{
			perror("send failed");
			return 1;
		}
		close(sock);
		sleep(1); // wait 1 second
	}

	return 0;
}
