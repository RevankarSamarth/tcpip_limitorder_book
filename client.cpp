#include "server.h"


#include <string>
#include <unistd.h>




using namespace std;



int main()
{
    
    int client;
    //int portNum = 1500; //port number is same for both client and server
    bool isExit = false;
    //int bufsize = 1024;
    char buffer[bufsize];
    const char* ip = "127.0.0.1"; //ip

    
    struct sockaddr_in server_addr;
    
    //create socket
    client = socket(AF_INET, SOCK_STREAM, 0);


    if (client < 0) 
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }


    cout << "\n=> Socket client has been created..." << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);


    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        cout << "=> Connection to the server port number: " << portNum << endl;

    cout << "=> Awaiting confirmation from the server..." << endl; 
    recv(client, buffer, bufsize, 0);
    cout << "=> Connection confirmed, you are good to go...\n";

    
    strcpy(buffer,"q\0");
    // Once it reaches here, the client can send a message first.

    do {
       
        
        if (recv(client, buffer, bufsize, 0) > 0){
		cout<<buffer<<endl;	
	}
        send(client, buffer, bufsize, 0);
        
    } while (!isExit);


    cout << "\n=> Connection terminated.\nGoodbye...\n";

    close(client);
    return 0;
}
