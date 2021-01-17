/**
 * sendrecv.c
 * Code Description: Defines functions shared by both the server and the client applications.
 * @author Dimitri Herr, Josh Bibighaus, Dominic Fiorino
 * @version 4/12/20 4/27/20
 * @contact ddh5256@psu.edu, jtb5770@psu.edu, daf5508@psu.edu
 */ 

#include "proj.h"

/*
 * Param: Socket descriptor, message length, and message pointer.
 * Return: N/A
 * Description: Receives a message and prints any appropriate errors. 
 */
void receive_msg(int socketDescriptor, int messageSize, void *message)
{
	//Tests for error when receiving messages
	if(recv(socketDescriptor, message, messageSize, 0) == -1)
	{
		perror("\nAn error has occurred while receiving the message.");
		exit(errno);
	}
}

/*
 * Param: Socket descriptor, message length, and message pointer.
 * Return: N/A
 * Description: Sends a message and prints any appropriate errors.
 */
void send_msg(int socketDescriptor, int messageSize, void *message)
{
	//Tests for error when sending the message
	if(send(socketDescriptor, message, messageSize, 0) == -1)
	{
		perror("\nAn error has occurred while sending the message.");
		exit(errno);
	}
}

/*
 * Param: Socket descriptor, message size, and the file to read from.
 * Return: N/A
 * Description: Sends data to the specificied socket descriptor in chunks.
 */
void send_data(int socketDescriptor, int messageSize, FILE *in_data)
{
	//Variables declared to store the size of the remaining file and the data structure to deliver.
	int fileRemaining = messageSize;
	struct data_msg out_data;

	//Outputs a spacing line
	printf("\n");

	//Do-while loop runs while there is still some file left.
	do
	{
		//If statement determines how much data to send at once.
		if(fileRemaining < MAX_DATA_SIZE)
		{
			//Reads in the entire file.
			fread(&out_data.buffer, fileRemaining, 1, in_data);
			out_data.data_leng = fileRemaining;

			//Prints the amount of data sent.
			printf("Sent %d bytes from file\n", out_data.data_leng);
			
			//If statement tests for errors in reading the data.
			if(ferror(in_data))
			{
				perror("\nAn error occurred while reading in data from the file.");
				exit(errno);
			}

			//Sends the created data structure and test for errors.
			if(send(socketDescriptor, &out_data, sizeof(struct data_msg), 0) == -1)
			{
				perror("\nAn error occurred while sending the message.");
				exit(errno);
			}

			//There is now nothing left to read in from the file.
			fileRemaining = 0;
		}
		else
		{
			//Only reads in the maximum amount of data possible.
			fread(&out_data.buffer,MAX_DATA_SIZE,1,in_data);
			out_data.data_leng = MAX_DATA_SIZE;

			//Prints the amount of data sent.
			printf("Sent %d bytes from file\n",out_data.data_leng);

			//If statement tests for errors in reading the data.
			if(ferror(in_data))
			{
				perror("\nAn error occurred while reading in data from the file.");
				exit(errno);
			}

			//Deincrements the remaining size of the file.
			fileRemaining -= MAX_DATA_SIZE;
		}

		//Sends the created data structure and test for errors.
		if(send(socketDescriptor, &out_data, sizeof(struct data_msg), 0) == -1)
		{
				perror("\nAn error occurred while sending the message.");
				exit(errno);
		}
	} while(fileRemaining != 0);

	//Outputs the total amount of data sent remotely.
	printf("Wrote %d bytes remotely\n", messageSize);
}

/*
 * Param: Socket descriptor, message size, and the file to write to.
 * Return: N/A
 * Description: Reads data from the specificied socket descriptor.
 */
void recv_data(int socketDescriptor, int messageSize, FILE *out_data)
{
	//Variables declared to store the data yet to be recieved and the recieved data structure.
	int fileRemaining = messageSize;
	struct data_msg in_data;

	//Outputs a spacing line.
	printf("\n");

	//Do while loop runs while there is data to be recieved.
	do
	{
		//Receives the created data structure and tests for errors.
		if(recv(socketDescriptor, &in_data, sizeof(struct data_msg), 0) == -1)
		{
			perror("\nAn error occurred while reading in data from the structure.");
			exit(errno);
		}

		//Writes the data to the file.
		fwrite(&in_data.buffer, in_data.data_leng, 1, out_data);

		//Outputs the amount of data written and deincrements the remaining data to receive.
		printf("Read %d bytes\n",in_data.data_leng);
		fileRemaining -= in_data.data_leng;
	} while(fileRemaining != 0);

	//Outputs the total amount of data sent to the output file.
	printf("Wrote %d bytes to output file.\n",messageSize);
}