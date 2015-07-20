// Compile with: g++ server.cpp -o serve -I/usr/include/python2.7 -lpython2.7
#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <cstring>
//#include <Python.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

int setup(addrinfo *host_info_list, int &socketfd, char *port = "27015")
{
   int status;
   struct addrinfo host_info;       // The struct that getaddrinfo() fills up wi$

   memset(&host_info, 0, sizeof host_info);

   cout << "Setting up the structs..."  << endl;

   host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
   host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM$

   // Now fill up the linked list of host_info structs with google's address inf$
   host_info.ai_flags = AI_PASSIVE;
   status = getaddrinfo(NULL, port, &host_info, &host_info_list);

   // getaddrinfo returns 0 on succes, or some other value when an error occured.
   // (translated into human readable text by the gai_gai_strerror function).
   if (status != 0)
   {
      cout << "getaddrinfo error" << gai_strerror(status) ;
      return -1;
   }

   cout << "Creating a socket..."  << endl;
   socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
   host_info_list->ai_protocol);
   if (socketfd == -1)
   {
      cout << "socket error " ;
      return -2;
   }

   cout << "Binding socket..."  << endl;
   // we make use of the setsockopt() function to make sure the port is not in u$
   // by a previous execution of our code. (see man page for more information)
   int yes = 1;
   status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
   status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
   if (status == -1)
   {
      cout << "bind error" << endl ;
      return -3;
   }

   cout << "Listening for connections..."  << endl;
   status =  listen(socketfd, 5);
   if (status == -1)
   {
      cout << "listen error" << endl ;
      return -4;
   }

}

int programRun(int new_sd)
{
   ssize_t bytes_recieved;
   char incomming_data_buffer[512];
   char msgToSend[512];
   while (1)
   {
      cout << "Waiting to recieve data\n";
      bytes_recieved = recv(new_sd, incomming_data_buffer,512,0);
      if (bytes_recieved == 0)
      {
         cout << "host shut down." << endl;
         return -1;
      }
      if (bytes_recieved == -1)
      {
         cout << "recieve error!" << endl ;
         return -2;
      }

      incomming_data_buffer[bytes_recieved] = '\0';
      cout << incomming_data_buffer << endl;
      if (!strcmp("Who are you?", incomming_data_buffer))
      {
	strncpy(msgToSend,"I am Server A!",sizeof(msgToSend));
	send (new_sd, msgToSend, strlen(msgToSend), 0);
      }
      else if (incomming_data_buffer[0] == '1')
      {
	cout << "Run yon bash script! starting at " << incomming_data_buffer[1]
	     << endl;
	if (bytes_recieved > 2)
	{
          int num = atoi(incomming_data_buffer+2);
	  stringstream ss;
          ss.str("");
	  ss << setw(2) << setfill('0') << num;
          string file1 = ss.str();
	  ++num;
	  ss.str("");
          ss << setw(2) << setfill('0') << num;
          string file2 = ss.str();
	  ++num;
          ss.str("");
          ss << setw(2) << setfill('0') << num;
          string file3 = ss.str();

	  string head = "sudo sh pics.sh";
	  //string path = " ftp/pictures/f";
	  //string type = ".jpg";
          string path = " f";
          string type = ".jpg";
          string shComm = head + path + file1 + type + path + file2 + type +
                          path + file3 + type;

	  system(shComm.c_str());
	  strncpy(msgToSend, "Ran Yon Bash Script!", sizeof(msgToSend));
	  send(new_sd, msgToSend, strlen(msgToSend), 0);
	}
	else
	{
	  system("sudo sh pics.sh");
	}
      }
      else
      {
	strncpy(msgToSend, "Unknown Command: Did nothing!", sizeof(msgToSend));
	cout << "Unkown Command!\n";
	send (new_sd, msgToSend, strlen(msgToSend), 0);
      }

   }

}

int main()
{
  while (true)
  {
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host$
    
    int socketfd ; // The socket descripter
    if (setup(host_info_list,socketfd) < 0)
      return 1;
    
    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
      cout << "listen error" << endl ;
    }
    else
    {
      cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
    }
    programRun(new_sd);
    
    send(new_sd, "Server Closing", 18, 0);
    cout << "Server Closing\n";
    freeaddrinfo(host_info_list);
    close(new_sd);
    close(socketfd);
  }
  return 0;
}
