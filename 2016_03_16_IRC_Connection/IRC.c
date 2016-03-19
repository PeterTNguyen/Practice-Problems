#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MSGLEN 512
#define CHANLEN
#define PARAMLEN 100
#define BUFFERLEN 10
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int sendMessage(int * sockfd, char * buffer);
int recvMessages(int * sockfd, char * buffer, char recvBuffer[BUFFERLEN*MSGLEN], int * line, int waitTime);
void printrecvBuffer(char * recvBuffer);

int main()
{
	char buffer[MSGLEN], recvBuffer[BUFFERLEN * MSGLEN];
	char serverAddr[PARAMLEN];
	char userName[PARAMLEN];
	char nickName[PARAMLEN];
	char realName[PARAMLEN];
	char pongMsg[MSGLEN + 4];
	char * curMsg;
	int sockfd, port, curLine, prevLine, numMsg, i, n;

	struct sockaddr_in serv_addr;
    struct hostent *server;

    //Initialize 
    bzero(recvBuffer, BUFFERLEN * MSGLEN);
    curLine = 0;
    prevLine = 0;
    numMsg = 0;

	//Input and parse parameters
    printf("Please enter SERVER:PORT: ");
	fgets(buffer,PARAMLEN,stdin);
	strcpy(serverAddr, strtok(buffer,":"));
	port = atoi(strtok(NULL,":"));

	printf("Please enter NICK NAME: ");
	fgets(nickName,PARAMLEN,stdin);
	strtok(nickName, "\n");

	printf("Please enter USER NAME: ");
	fgets(userName,PARAMLEN,stdin);
	strtok(userName, "\n");

	printf("Please enter REAL NAME: ");
	fgets(realName,PARAMLEN,stdin);
	strtok(realName, "\n");

	//Setup Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname(serverAddr);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //Write NICK <nickname> msg
	bzero(buffer,MSGLEN);
	strcpy(buffer, "NICK ");
	strcat(buffer, nickName);
    sendMessage(&sockfd, buffer);

    //Wait and parse return message
    recvMessages(&sockfd, buffer, recvBuffer, &curLine, 1);
    prevLine++;

	//Write USER Username 0 * :Real Name
	bzero(buffer,MSGLEN);
	strcpy(buffer, "USER ");
	strcat(buffer, userName);
	strcat(buffer, " 0 * ");
	strcat(buffer, realName);
	sendMessage(&sockfd, buffer);

    //Wait and parse return message
    while(1)
    { 
    	//Receive Messages
    	numMsg = recvMessages(&sockfd, buffer, recvBuffer, &curLine, 1);

    	if(numMsg >0)
    	{
    		for(i = 0; i < numMsg; i++)
    		{
    			//Grab and print unprocessed messages from buffer
	    		curMsg = &recvBuffer[((prevLine) * MSGLEN) % (MSGLEN * BUFFERLEN)];
	    		printf("<%s\n", curMsg);

	    		//PING PONG
	    		if((curMsg[0] == 'P') && (strstr(curMsg, "PING") != NULL))
	    		{
	    			//Create PONG Response
	    			bzero(pongMsg, MSGLEN);
	    			strcpy(pongMsg, "PONG");
	       			strcat(pongMsg, curMsg + 4);	//Add response str
	    			sendMessage(&sockfd, pongMsg);
	    		}
	    		prevLine = (prevLine + 1) % BUFFERLEN;
    		}
    	}
    	
    }
    
    return 0;
}

int sendMessage(int * sockfd, char * buffer)
{
	int n;
	printf(">%s\n", buffer);
	strcat(buffer, "\r\n");
	n = write(*sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    return n;
}

int recvMessages(int * sockfd, char * buffer, char recvBuffer[BUFFERLEN*MSGLEN], int * line, int waitTime)
{
	int n, count = 0;
	char * pch;
	//sleep(waitTime);
	bzero(buffer,MSGLEN);
	n = read(*sockfd,buffer,MSGLEN - 1);
    if (n < 0)
         error("ERROR reading from socket");

    //Split received socket message by delimiter \n. Move into recvBuffer
	while(buffer != NULL)
	{
		if(strrchr(buffer, '\n') != NULL)
			count++;

		pch = strsep (&buffer, "\n");
		if(pch != NULL)
		{
			if((*line) % (MSGLEN) ==0)
				bzero(recvBuffer + *line, MSGLEN);

			//Copy strsep token to line buffer
			strcpy(&recvBuffer[*line], pch);

			//move Line marker
			*line = (buffer != NULL) ? 
				(((*line/MSGLEN)*MSGLEN + MSGLEN)) % (MSGLEN * BUFFERLEN) : //move line marker to next buffer slot
				(*line + strlen(pch)) % (MSGLEN * BUFFERLEN) ; //move line marker to mid line
		}
	}
    
	return count;
}
