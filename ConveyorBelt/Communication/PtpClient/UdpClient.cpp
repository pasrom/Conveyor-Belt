/* includes */
#include "UdpClient.h"

UdpClient::UdpClient() {
	return;
}

UdpClient::~UdpClient() {
	printf("UdpClient dekonstruktor\n");
	return;
}

void UdpClient::deleteSfd() {
	if (shutdown(sFd, 2) == OK)
		close(sFd);
}

/*********************************************************************
 *
 * receiveUDP - read from UDP socket and store message and client address
 *
 * Parameter:
 * struct request - structure including data received, client address and port
 * 
 * RETURNS: OK, or ERROR if a resource could not be allocated.
 */
STATUS UdpClient::receiveUDP(struct requestPTP * myRequestP) {
	struct sockaddr_in serverAddr; /* server's socket address */
	struct sockaddr_in clientAddr; /* client's socket address */
	int sockAddrSize; /* size of socket address structure */
	char inetAddr[INET_ADDR_LEN]; /* buffer for client's inet addr */
	char message[MESSAGE_SIZE];
	/* set up the local address */
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* create a UDP-based socket */
	if ((sFd = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
		perror("could not open socket");
		return (ERROR);
	}

	/* bind socket to local address */
	if (bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("could not bind socket");
		close(sFd);
		return (ERROR);
	}

	/* read data from a socket and satisfy requests */

	if (recvfrom(sFd, message, MESSAGE_SIZE, 0, (struct sockaddr *) &clientAddr, &sockAddrSize) == ERROR) {
		perror("error in recvfrom");
		close(sFd);
		return (ERROR);
	}

	/* convert inet address to dot notation and store it*/
	inet_ntoa_b(clientAddr.sin_addr, myRequestP->inetAddr);
	printf("%s\n", message);
	myRequestP->PTP_version = message[0];
	myRequestP->message_type = message[1];
	myRequestP->epoch_number = 0;
	myRequestP->nanoseconds = 0;
	myRequestP->seconds = (time_t) atoi(&message[2]);

	close(sFd);
	return (OK);
}

/****************************************************************************
 *
 * sendUDP - send a message to a server over a UDP socket
 *
 * Parameter:
 * struct request - structure including data to send, server address and port
 * 
 * RETURNS: OK, or ERROR if the message could not be sent to the server.
 */
STATUS UdpClient::sendUDP(struct requestPTP * myRequestP) {
	struct sockaddr_in serverAddr; /* server's socket address */
	int sockAddrSize; /* size of socket address structure */
	int sFd; /* socket file descriptor */
	int mlen; /* length of message */
	char message[MESSAGE_SIZE];
	int retAddr, retName;

	/* create client's socket */
	if ((sFd = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
		perror("error in creating socket");
		return (ERROR);
	}

	/* bind not required - port number is dynamic */

	/* build server socket address */
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);

	retAddr = inet_addr(myRequestP->inetAddr);
	if (retAddr == ERROR) {
		perror("unknown server name: inet_addr\n");
	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retAddr;
	}
	retName = hostGetByName(myRequestP->inetAddr);
	if (retName == ERROR) {
		perror("unknown server name: hostGetByName\n");
	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retName;
	}
	if (retName == ERROR && retAddr == ERROR) {
		return (ERROR);
	}

	message[0] = myRequestP->PTP_version;
	message[1] = myRequestP->message_type;
	sprintf(&message[2], "%d", myRequestP->seconds);

	/* send request to server */
	if (sendto(sFd, (caddr_t) &message, MESSAGE_SIZE, 0, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("error in sendto");
		close(sFd);
		return (ERROR);
	}

	close(sFd);
	return (OK);
}
STATUS UdpClient::sendUDP(struct requestPTP * myRequestP, char *str) {
	struct sockaddr_in serverAddr; /* server's socket address */
	int sockAddrSize; /* size of socket address structure */
	int sFd; /* socket file descriptor */
	int retAddr, retName;

	/* create client's socket */
	if ((sFd = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
		perror("error in creating socket");
		return (ERROR);
	}

	/* bind not required - port number is dynamic */

	/* build server socket address */
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);

	retAddr = inet_addr(myRequestP->inetAddr);
	if (retAddr == ERROR) {
		perror("unknown server name: inet_addr\n");
	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retAddr;
	}
	retName = hostGetByName(myRequestP->inetAddr);
	if (retName == ERROR) {
		perror("unknown server name: hostGetByName\n");
	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retName;
	}
	if (retName == ERROR && retAddr == ERROR)
		return (ERROR);

	/* send request to server */
	if (sendto(sFd, str, strlen(str), 0, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("error in sendto");
		close(sFd);
		return (ERROR);
	}

	close(sFd);
	return (OK);
}
