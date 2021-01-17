/**
 * proj.h
 * Code Description: Hosts definitions shared by both the server and the client applications.
 * @author Dimitri Herr, Josh Bibighaus, Dominic Fiorino
 * @version 4/12/20 4/27/20
 * @contact ddh5256@psu.edu, jtb5770@psu.edu, daf5508@psu.edu
 */ 

/*
* CMPSC 311 Project Header File
* Authors: Joshua Bibighaus, Dominic Fiorino, Dimitri Herr, Ustym Koshak
* Decription: Definitions shared by both server and client applications.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/socket.h>

//Defines the max input, data size, and the command send and recieve statuses.
#define MAX_IN 128
#define MAX_DATA_SIZE 1024
#define CMD_SEND 0
#define CMD_RECV 1

//Structure for the message sent by client to server.
struct send_msg
{
  int msg_type;   //CMD_SEND or CMD_RECV
  int file_size;    //0 if inapplicable
  char file_name[128];
};

//Structure for the message sent by server to client (handshake message).
struct resp_msg
{
  int msg_type; //CMD_RESP
  int status;   //0 if ok, otherwise errno
  int file_size;  //used only with server-to-client and status=0
};

//Defines the command data status.
#define CMD_DATA CMD_SEND | CMD_RECV

//Message type for data transfer in either direction.
struct data_msg
{
  int msg_type; //CMD_DATA
  int data_leng;    //Length of data in buffer.
  char buffer[MAX_DATA_SIZE];
};