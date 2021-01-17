# BasicServerClient
Final project for CMPSC311: Programming in UNIX course.

The client and server program was created as a final project for CMPSC311: Programming in UNIX. These are intended to both be compiled in a UNIX environment.
Once ran, they can be used to transfer a single file or message over a network connection (given the right permissions) or between different local directories through TCP.

To run the program, it must be compiled in a UNIX environment using the command line-interface.
The following commands will make runnable versions of the server and client:

$ cc –c sendrecv.c

Then compile your server and client with this file:

$ cc server.c sendrecv.o –o server

$ cc client.c sendrecv.o –o client

$ ./server

$ ./client
