/**
 * server.c
 * Code Description: Defines the server application using TCP connection.
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
	//Variables declared to store the structures for the personal and peer address, to and from the client,
	//The peer address size, the bugger, the input and output files, the socket descriptors, and the client connection.
	struct sockaddr_in my_addr, peer_addr;
	struct send_msg from_client;
	struct resp_msg to_client;
	socklen_t peer_addr_size;
	char *buffer;
	FILE *fd_in, *fd_out;
	int socketDescriptor, clientConnection;

	//Creates socket and socket descriptor
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	//Tests for an error while creating the socket.
	if(socketDescriptor == -1)
	{
		perror("\nAn error occurred while creating the socket.");
		exit(errno);
	}

	//Sets the protocol family and resets the buffer.
	my_addr.sin_family = AF_INET;
	buffer = malloc(MAX_IN);

	//Prompts the user for the address to bind.
	printf("Enter address to bind: ");
	fgets(buffer, MAX_IN, stdin);

	//Converts to address format and sets the address.
	my_addr.sin_addr.s_addr = inet_addr(buffer);

	//Resets the input buffer.
	free(buffer);
	buffer = malloc(MAX_IN);

	//Prompts the user for the port to listen to.
	printf("Enter the port to listen: ");
	fgets(buffer, MAX_IN, stdin);

	//Converts the port to an integer and sets the port.
	my_addr.sin_port = htons(atoi(buffer));

	//Attempts to bind to the address.
	if(bind(socketDescriptor, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1)
	{
		perror("\nAn error occurred while binding the address.");
		exit(errno);
	}

	//Attempts to listen for incoming connections.
	if(listen(socketDescriptor, 1) == -1)
	{
		perror("\nAn error occurred while listening for incoming connections.");
		exit(errno);
	}

	//Outputs a listening to incoming connections statement to the user.
	printf("Listening for incoming connections.\n");

	//Server runs until manually closed
	while(1)
	{
		//Establishes a connection with the client.
		clientConnection = accept(socketDescriptor, (struct sockaddr*)&peer_addr, &peer_addr_size);

		//Only continues when the connection is established
		if(clientConnection != -1)
		{
			//Outputs a connected message and reads in the initial message from the client.
			printf("Connected to Client!\n");
			receive_msg(clientConnection, sizeof(from_client), &from_client);

			//Creates response to send to the client (handshake).
			to_client.msg_type = from_client.msg_type;

			//Determines the response based on the msg_type.
			switch(from_client.msg_type)
			{
				//Client-to-Server
				case 0:
					//Attempts to open a file to save the file from the client.
					if((fd_out = fopen(from_client.file_name, "wb+")) == NULL)
					{
						to_client.status = errno;
						send_msg(clientConnection, sizeof(to_client), &to_client);
						perror("\nAn error occurred while creating the file.");
						exit(errno);
					}

					//Resets the status and sends the message to begin transmission.
					to_client.status=0;
					send_msg(clientConnection,sizeof(to_client),&to_client);

					//Starts receiving the message.
					recv_data(clientConnection, from_client.file_size, fd_out);
					break;
				//Server-to-Client
				case 1:
					//Attempts to open file requested by the client
					fd_in = fopen(from_client.file_name, "rb");
					if(fd_in == NULL)
					{
						to_client.status = errno;
						send_msg(clientConnection,sizeof(to_client),&to_client);
						perror("\nAn error occurred while opening the file.");
						exit(errno);
					}

					//Saves file size by going to end, saving the position, then resetting to the start.
					fseek(fd_in,0L,SEEK_END);
					to_client.file_size=ftell(fd_in);
					fseek(fd_in,0,SEEK_SET);

					//Sends the response message.
					send_msg(clientConnection, sizeof(to_client), &to_client);

					//Starts sending the message.
					send_data(clientConnection,to_client.file_size,fd_in);
					break;
				default:
					perror("\nAn error occurred due to an invalid message type.");
					exit(errno);
					break;
			}

			//Determines if the file needs to be closed
			if(fd_out != NULL)
			{
				fflush(fd_out);
				fclose(fd_out);
			}
			printf("\nListening for incoming connections.\n");
		}
	}
}