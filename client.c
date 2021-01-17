/**
 * client.c
 * Code Description: Defines the client application using TCP connection.
 * @author Dimitri Herr, Josh Bibighaus, Dominic Fiorino
 * @version 4/12/20 4/27/20
 * @contact ddh5256@psu.edu, jtb5770@psu.edu, daf5508@psu.edu
 */ 

#include "proj.h"

//Prototypes declared for receiving the message, sending the message, receiving the data, and sending the data.
void receive_msg(int, int, void*);
void send_msg(int, int, void*);
void send_data(int, int, FILE*);
void recv_data(int, int, FILE*);

int main(int argc,char *argv[])
{
	//Variables declared to store the server address, the initial message, the server response,
	//the buffer, the file descriptors sent (out) and recieved (in), the socket descriptor,
	//and the transfer type.
	struct sockaddr_in server_addr;
	struct send_msg initial_msg;
	struct resp_msg server_resp;
	char *buffer;
	FILE *fd_in, *fd_out;
	int socketDescriptor, transferType;

	//Creates socket and socket descriptor
	socketDescriptor = socket(AF_INET,SOCK_STREAM,0);

	//Tests for an error while creating the socket.
	if(socketDescriptor == -1)
	{
		perror("\nAn error has occurred while creating a socket.");
		exit(errno);
	}
	//Allocates memory for the buffer.
	buffer = malloc(MAX_IN);

	//Prompts the user for the address to connect.
	printf("Enter remote address to connect: ");
	fgets(buffer, MAX_IN, stdin);

	//Converts to address format and sets family.
	server_addr.sin_addr.s_addr = inet_addr(buffer);
	server_addr.sin_family = AF_INET;

	//Resets the input buffer.
	free(buffer);
	buffer = malloc(MAX_IN);

	//Prompts the user for the port.
	printf("Enter the remote port: ");
	fgets(buffer,MAX_IN,stdin);

	//Converts to int and sets port.
	server_addr.sin_port = htons(atoi(buffer));

	//Prompts for communication type and runs until valid.
	do
	{
		//Resets the input buffer.
		free(buffer);
		buffer = malloc(MAX_IN);

		//Prompts the user for the type of transfer.
		printf("--Choose the type of transfer--\n\t1. Client-to-Server\n\t2. Server-to-Client\n\tEnter: ");
		fgets(buffer, MAX_IN, stdin);

		//Sets the transfer type.
		transferType = atoi(buffer);
		transferType--;

		//Determines when the transfer type is correct.
		switch(transferType)
		{
			//Client-to-Server
			case 0:
				initial_msg.msg_type = CMD_SEND;
				break;
			//Server-to-Client
			case 1:
				initial_msg.msg_type = CMD_RECV;
				break;
			default:
				printf("\nInvalid transfer type, please try again.\n");
				break;
		}
	} while(transferType != 0 && transferType != 1);

	//Resets the input buffer.
	free(buffer);
	buffer=malloc(MAX_IN);

	//Prompts the user for the name of the file.
	printf("Enter the file name: ");
	fgets(buffer,MAX_IN,stdin);

	//Removes the newline from the buffer.
	buffer[strlen(buffer) - 1] = '\0';

	//Opens the file based on the transfer type.
	switch(transferType)
	{
		//Client-to-Server
		case 0:
			//Opens the file.
			fd_out=fopen(buffer, "rb");

			//Determines the file validity.
			if(fd_out==NULL)
			{
				perror("\nThere is an error with the file.");
				exit(errno);
			}
			break;
		//Server-to-Client
		case 1:
			//Opens the file.
			fd_in = fopen(buffer, "wb+");

			//Determines the file validity
			if(fd_in==NULL)
			{
				perror("\nThere is an error with the file.");
				exit(errno);
			}
			break;
	}

	//Determines the length of the file to be sent.
	if(fd_out != NULL)
	{
		//Saves file size by going to end, saving the position, then resetting to start
		fseek(fd_out, 0L, SEEK_END);
		initial_msg.file_size = ftell(fd_out);
		fseek(fd_out, 0, SEEK_SET);
	}

	//Sets the file name (irrelevant of transfer type).
	strcpy(initial_msg.file_name, buffer);

	//Attempts to connect to the address.
	if(connect(socketDescriptor, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("\nAn error occurred while establishing a connection to the address.");
		exit(errno);
	}

	//Sends message to determine communication type.
	send_msg(socketDescriptor,sizeof(initial_msg),&initial_msg);

	//Gets response from server.
	receive_msg(socketDescriptor,sizeof(server_resp),&server_resp);

	//Determines if the response was successful.
	if(server_resp.status != 0)
	{
		fprintf(stderr,"\nServer reported an error: %s\n", strerror(server_resp.status));
		exit(server_resp.status);
	}

	//Begins data sending or recieving
	switch(server_resp.msg_type)
	{
		//Client-to-Server
		case 0:
			send_data(socketDescriptor,initial_msg.file_size,fd_out);
			break;
		//Server-to-Client
		case 1:
			recv_data(socketDescriptor,server_resp.file_size,fd_in);
			break;
	}

	//Resets the input buffer.
	free(buffer);
	buffer=malloc(MAX_IN);

	//Determines which files need to be closed.
	if(fd_in != NULL)
	{
		fflush(fd_in);
		fclose(fd_in);
	}
	if(fd_out != NULL)
	{
		fflush(fd_out);
		fclose(fd_out);
	}

	//Closes the socket descriptor.
	close(socketDescriptor);

	exit(0);
}