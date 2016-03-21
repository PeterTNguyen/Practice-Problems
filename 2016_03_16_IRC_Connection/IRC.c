#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdarg.h>

#define MSGLEN 512
#define CHANLEN 200
#define NAMELEN 17
#define PARAMLEN 100
#define BUFFERLEN 10
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int recvMessages(int * sockfd, char * buffer, char recvBuffer[BUFFERLEN*MSGLEN], int * line, int waitTime);
void printrecvBuffer(char * recvBuffer);
int createSendMessage(int * sockfd, char * message, int n, ...);

int main()
{
	char msgBuffer[MSGLEN], recvBuffer[BUFFERLEN * MSGLEN];
	//char serverAddr[PARAMLEN];
	//char userName[PARAMLEN], nickName[PARAMLEN], realName[PARAMLEN];
	//char channels[MSGLEN];
	char msgRecipient[CHANLEN];
	char returnVal[MSGLEN];
	const char serverAddr[] = "chat.freenode.net";
	const char userName[] = "phoshzzle";
	const char nickName[] = "phoshzzle_bot";
	const char realName[] = "Peter Nguyen";
	//const char channels[] = "#reddit-dailyprogrammer,#rdp,#botters-test";
	const char channels[] = "#botters-test";
	const char helloWorld[] = "Hello World!";
	char pongMsg[MSGLEN + 4];
	char * curMsg;
	char * strSearch, * strChannel, * strCommand, * operand;
	int sockfd, port = 6667, curLine, prevLine, numMsg, i, n;
	int calculation;
	float floatCalc;

	struct sockaddr_in serv_addr;
    struct hostent *server;

    //Initialize 
    bzero(recvBuffer, BUFFERLEN * MSGLEN);
    curLine = 0;
    prevLine = 0;

	//Input and parse parameters
	/*
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
	*/
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
    createSendMessage(&sockfd, msgBuffer, 2, "NICK ", nickName);

    //Wait and parse return message
    recvMessages(&sockfd, msgBuffer, recvBuffer, &curLine, 1);
    prevLine++;

	//Write USER Username 0 * :Real Name
	createSendMessage(&sockfd, msgBuffer, 4, "USER ", userName, " 0 * ", realName);

    //Wait and parse return message
    while(1)
    { 
    	//Receive Messages
    	numMsg = recvMessages(&sockfd, msgBuffer, recvBuffer, &curLine, 1);

    	if(numMsg >0)
    	{
    		for(i = 0; i < numMsg; i++)
    		{
    			//Grab and print unprocessed message from buffer
	    		curMsg = &recvBuffer[((prevLine) * MSGLEN) % (MSGLEN * BUFFERLEN)];
	    		strtok(curMsg,"\r");
	    		printf("<%s\n", curMsg);

	    		//PING PONG
	    		if((curMsg[0] == 'P') && (strstr(curMsg, "PING") != NULL))
	    		{
	    			//Create PONG Response
	    			createSendMessage(&sockfd, pongMsg, 2, "PONG", curMsg + 4);
	    		}

	    		//RPL_ENDOFMOTD
	    		if((strstr(curMsg, " 376 ") != NULL))
	    		{
	    			//Create JOIN Response
	    			createSendMessage(&sockfd, msgBuffer, 2, "JOIN ", channels);
	    		}

				//JOIN message
	    		strSearch = strstr(curMsg, "JOIN ");
	    		if(strSearch != NULL)
	    		{//Channel replies to username
		    		if((strstr(curMsg, nickName) != NULL))
		    		{
		    			//Create HELLO WORLD Response
		    			createSendMessage(&sockfd, msgBuffer, 4, "PRIVMSG ", strSearch + 5, " :",helloWorld);
		    		}
	    		}

	    		//PRIVMSG
	    		//<:phoshzzle_human!6c38b23b@gateway/web/freenode/ip.108.56.178.59 PRIVMSG phoshzzle_bot :random 20
	    		strSearch = strstr(curMsg, "PRIVMSG ");
	    		if(strSearch != NULL)
	    		{//Channel replies to username
	    			strSearch = strchr(strSearch, ' ');
	    			
	    			//Private msg to USER
		    		if((strstr(strSearch, nickName) != NULL))
		    		{
		    			//Message from channel
		    			strChannel = strchr(strSearch, '#');
		    			bzero(returnVal, MSGLEN);

		    			if(strChannel != NULL)
		    			{
		    				bzero(msgRecipient, CHANLEN);
		    				msgRecipient[0] = '#';
		    				memcpy(msgRecipient+1, strChannel + 1, strchr(strSearch+1, ' ') - strChannel);
		    				strSearch = strchr(strSearch+1, ' ');
		    			}
		    			else	//it's priv message from a USER
		    			{
		    				//Get recipient 
		    				bzero(msgRecipient, NAMELEN);
		    				memcpy(msgRecipient, curMsg + 1, strchr(curMsg, '!') - curMsg - 1);
		    			}

		    			strSearch = strchr(strSearch+1, ' ');

		    			//Random
		    			if((strstr(strSearch, "random") != NULL))
		    			{
		    				strSearch = strstr(strSearch, "random");
		    				//Genrate Random #
		    				strSearch = strchr(strSearch+1, ' ');
		    				calculation = atoi(strSearch+1);
		    				calculation = rand() % calculation;
		    				sprintf(returnVal, "%d", calculation);

		    				//Create HELLO WORLD Response
		    				createSendMessage(&sockfd, msgBuffer, 4, "PRIVMSG ", msgRecipient, " :", returnVal);
		    			}

		    			//Sum
		    			if((strstr(strSearch, "sum") != NULL))
		    			{
		    				//Get inputs and multiply
		    				calculation = 0;
		    				operand = strstr(strSearch, "sum");
		    				operand = strchr(strSearch, ' ');
		    				while(operand != NULL)
		    				{
		    					operand = strchr(operand+1, ' ');
		    					if(operand != NULL)
		    						calculation += atoi(operand+1);
		    				}
		    				sprintf(returnVal, "%d", calculation);

		    				//Create HELLO WORLD Response
		    				createSendMessage(&sockfd, msgBuffer, 4, "PRIVMSG ", msgRecipient, " :", returnVal);
		    			}

		    			//Mult
		    			if((strstr(strSearch, "mult") != NULL))
		    			{
		    				strSearch = strstr(strSearch, "mult");
		    				//Get inputs and multiply
		    				calculation = 1;
		    				operand = strSearch;
		    				while(operand != NULL)
		    				{
		    					operand = strchr(operand+1, ' ');
		    					if(operand != NULL)
		    						calculation *= atoi(operand+1);
		    				}
		    				sprintf(returnVal, "%d", calculation);

		    				//Create HELLO WORLD Response
		    				createSendMessage(&sockfd, msgBuffer, 4, "PRIVMSG ", msgRecipient, " :", returnVal);
		    			}
		    			
		    			//Division
		    			if((strstr(strSearch, "div") != NULL))
		    			{
		    				strSearch = strstr(strSearch, "div");
		    				//Genrate Random #
		    				strSearch = strchr(strSearch+1, ' ');
		    				calculation = atoi(strSearch+1);
		    				strSearch = strchr(strSearch+1, ' ');
		    				floatCalc = (float)calculation / (float)atoi(strSearch+1);
		    				sprintf(returnVal, "%f", floatCalc);

		    				//Create HELLO WORLD Response
		    				createSendMessage(&sockfd, msgBuffer, 4, "PRIVMSG ", msgRecipient, " :", returnVal);
		    			}
		    		}
	    		}

	    		//increment processed line counter
	    		prevLine = (prevLine + 1) % BUFFERLEN;
    		}
    	}
    	
    }
    
    return 0;
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

int createSendMessage(int * sockfd, char * message, int n, ...)
{
	int ret;
	char * subStr;
	bzero(message, MSGLEN);
	va_list args;
	va_start(args, n);

	//Zero message buffer, copy first str in
	bzero(message, MSGLEN);
	subStr = va_arg(args, char *);
	strcpy(message, subStr);

	//Append rest of strs
	for(int i = 1; i < n; i++)
	{
		subStr = va_arg(args, char *);
		strcat(message, subStr);
	}
	va_end(args);

	//Send Message
	printf(">%s\n", message);
	strcat(message, "\r\n");
	ret = write(*sockfd,message,strlen(message));
    if (ret < 0) 
         error("ERROR writing to socket");
    return ret;
}

