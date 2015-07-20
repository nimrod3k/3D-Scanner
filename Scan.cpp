// compile with g++ Scan.cpp -o scan.exe -lws2_32 -lwininet
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <wininet.h>
#include <sstream>
#include <iomanip>
#include "client.h"

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


using namespace std;

/**************************************************
* Turns commands into ready to send commands
**************************************************/
int runFTP(int piCount, int turns, int camerasPer)
{
   system("del /Q \"C:\\Users\\Aaron\\Desktop\\Scanner\\Copy Files\\*\"");
   char address[12]="169.254.1.1";
   int numPics = turns*camerasPer;
   string cHead="C:\\Users\\Aaron\\Desktop\\Scanner\\Copy Files\\f";
   string sHead="/pictures/f";
   string trail=".jpg";
   HINTERNET hInternet = InternetOpen(NULL,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0); // Initialization for WinInet Functions
   if (hInternet == NULL)
   {
      cout << "Error: Could not open Internet connection\n";
      InternetCloseHandle(hInternet); // Close hInternet
      return 1;
   }
   // HINTERNET hFtpSession;
   for (int i = 0; i < piCount; i++)
   {
      address[10] = '1'+i;
//      address[10] = '1';
      cout << "Address to Get from to: " << address << endl;
      HINTERNET hFtpSession = InternetConnect(hInternet,address,INTERNET_DEFAULT_FTP_PORT,"pi","raspberry", INTERNET_SERVICE_FTP,INTERNET_FLAG_PASSIVE,0);  // Starts a session in this case an FTP session
      if (hFtpSession == NULL)
      {
         cout << "Error: Could not open FTP Session\n";
         InternetCloseHandle(hInternet); // Close hInternet
         return 1;
      }
      for (int j = 0; j < numPics/piCount; j++)
      {
         stringstream ss;
         ss << setw(2) << setfill('0') << ((i*numPics/piCount)+j);
         string cNum = ss.str();
         string cFile = cHead + cNum + trail;
         ss.str("");
         ss << setw(2) << setfill('0') << j;
         string sNum = ss.str();
         string sFile = sHead + sNum + trail;
         cout << "File to get: " << sFile << endl;
         cout << "File to Write: " << cFile << endl;
         if(!FtpGetFile(hFtpSession,sFile.c_str(),cFile.c_str(),TRUE,FILE_ATTRIBUTE_NORMAL,FTP_TRANSFER_TYPE_ASCII,0))
         {
            /*cout << "Error: Could not copy file\n";
            cout << GetLastError() << endl;
            DWORD dwInetError;
            DWORD dwExtLength = 1000;
            TCHAR *szExtErrMsg = NULL;
            TCHAR errmsg[1000];
            szExtErrMsg = errmsg;
            int returned = InternetGetLastResponseInfo(&dwInetError, szExtErrMsg, &dwExtLength);
            cout << "dwInetError: " << dwInetError << returned << endl;
            cout << "Bufer: " << szExtErrMsg << endl;
            InternetCloseHandle(hFtpSession); // Close hFtpSession
            InternetCloseHandle(hInternet); // Close hInternet
            return 1;*/
         }
      }
      InternetCloseHandle(hFtpSession); // Close hFtpSession

   }

   InternetCloseHandle(hInternet); // Close hInternet    

   return 0;   
}

/**************************************************
* Turns commands into ready to send commands
**************************************************/
char* interpretCommand(const string msg)
{
   int index = msg.find(' ',0);
   string cmd;
   string arg;
   if (index != -1)
   {
      cmd = msg.substr(0,index);
      arg = msg.substr(index,msg.length());
   }
   else
   {
      cmd = msg;
   }
   if (!cmd.compare("snap"))
   {
      cmd = "1" + arg;
      cout << cmd << endl;
   }
   else if (!cmd.compare("ftp"))
   {
      cout << "run FTP" << endl;
      cmd = "2" + arg;
   }
   else
   {
      cmd = msg;
   }
   return (char*) cmd.c_str();
}

/**************************************************
* Prompts for command to send and transforms it to
* ready to send command
**************************************************/
char* getCommand(string &msg)
{
	cout << "Command: ";
	getline(cin, msg);

   if (msg.compare("close") == 0)
	{
      return (char*)"0";
   }
   if (msg.compare("start") == 0)
   {
      return (char*)"3";
   }

   return interpretCommand(msg);
}

/***************************************************
* Runs the entire program from pictures to 3D model
***************************************************/
int ProcRequest(Client *client[], int piCount, int turns, int camerasPer)
{
   cout << "ProcRequest() run\n";
   char cmd[5] = "1 0\0";
   char collect[4];
   for (int i = 0; i < turns; ++i)
   {
      //cout << "Before cmd: " << cmd << endl;
      itoa(i*camerasPer,collect,10);
      //cout << "itoa complete - Collect: " << collect << endl;
      for (int j = 0; collect[j] != '\0'; ++j)
      {
         cmd[j+2] = collect[j];
      }
      //cout << "After cmd: " << cmd << endl;
      for (int j = 0; j < piCount; ++j)
      {
         client[j]->Send(cmd);
      }
      for (int j = 0; j < piCount; ++j)
      {
         client[j]->Recv();
      }
      system("pause");
   }
   if (runFTP(piCount,turns,camerasPer))
      return 1;
   return 0; // All Good
}

int main(int argc, char* argv[]) 
{
	string msg;
   char address[12] = "169.254.1. ";
   int piCount = 1;
   int camerasPer = 3;
   int turns = 1;
   if (argc > 1)
   {
      piCount = atoi(argv[1]);
   }
   if (argc > 2)
   {
      turns = atoi(argv[2]);
   }
   if (argc > 3)
   {
      camerasPer = atoi(argv[3]);
   }   
   Client *client[piCount];
   for (int i = 0; i < piCount; i++)
   {
      address[10] = '1' + i;
      cout << "Creating connection to: " << address << endl;
      client[i] = new Client(address);
      if (!client[i]->Start())
      {
         for (int i = 0; i < piCount; i++)
         {
            client[i]->Stop();
            delete client[i];
         }
         return 1;
      }
   }

   char *cmsg;//[DEFAULT_BUFFER_LENGTH];
	while(true)
	{
      cmsg = getCommand(msg);
      
      // Close the connection when user enter "close"
      if (cmsg[0]=='0')
      {
         cout << "closing...\n";
         break;
      }
      else if (cmsg[0]=='3')
      {
         ProcRequest(client,piCount, turns, camerasPer);
         //break;
      }
      else if (cmsg[0]=='2')
      {
         runFTP(piCount,turns,camerasPer);
         cout << "FTP complete\n";
      }
      else
      {
         for (int i = 0; i < piCount; i++)
         {
            client[i]->Send(cmsg);
         }
         for (int i = 0; i < piCount; i++)
         {
            cout << "Recieving!!\n";
            client[i]->Recv();
         }
      }
	}
   for (int i = 0; i < piCount; i++)
   {
      cout << "Stopping Connections\n";
      client[i]->Stop();
      cout << "Deleting Variables\n";
      delete client[i];
   }
	return 0;
}