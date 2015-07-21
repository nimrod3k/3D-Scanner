All programs include compiler instructions for MinGW compiler at the top of the file

Server.cpp
 The server program can be run without command line arguments and then it waits for a client to connect.  The only thing that  matters is what port it is listening on.  Once connected it just waits for commands from the client.  The server was going to  support several commands.  To make it easy to parse the server actually just looks at the first character and if it is a 1 then it is the command to take pictures.

 To take these pictures the client currently runs the script pics.sh using system().  pics.sh should be in the same directory as  the program.

Scan.cpp
 This requires Client.h to compile.  This process several commands including snap which sends a take picture command to the  server.  FTP which makes an FTP request to the server.  In order for this to work you currently need to hardcode the IP address.

 The form to run it in is ./client [number of pi's] [number of turns for the structure] [number of cameras per pi]

 When this program is run it tries to make a TCP/IP connection automatically.  Type close to exit.  ctrl+c will cause errors on   the server causing the server program to crash and no longer accept connections.

 ftp will cause the program to request all of the files it should be recieving.  This code was in the process of being changed from a fixed number of cameras and a one time picture taking to being able to support multiple times taking pictures and various numbers of cameras per pi.  **THERE MAY BE MANY BUGS**

 The start command will run through the whole process of taking pictures and making the FTP get request.