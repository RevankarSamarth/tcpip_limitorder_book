

#include "server.h"
#include <errno.h>

#define max_clients 2
using namespace std;


int server(vector<pair<string, pair<float,float>>> &updates, int &update_flag, int &open)
{
    //data structures to store messages, Bid and Sell
    multimap<float, int, greater<float>> B;
    multimap<float, int> S;


    //config for server client connection
    int client, server, max_sd, activity, new_socket, val_read, sd;
    char buffer[bufsize];

    //set of socket descriptors
    fd_set readfds;

    //max clients
    int client_sockets[2]; //, max_clients = 2; 

    //initialize client sockets to zero
    for(int i = 0; i < max_clients; i++)
    {
      client_sockets[i] = 0;
    }

    struct sockaddr_in server_addr;
    socklen_t size;

    //create socket
    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0) 
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }

    cout << "\n=> Socket server has been created..." << endl;

    //define struct for server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    //bind the socket

    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0) 
    {
        cout << "=> Error binding connection, the socket has already been established..." << endl;
        return -1;
    }

    size = sizeof(server_addr);
    cout << "=> Looking for clients..." << endl;

    //listen
    listen(client, max_clients);
    
    while(true)
    {
    	//clear the socket
	FD_ZERO(&readfds);
	
	//add master socket to the set
	FD_SET(client, &readfds);
	max_sd = client;

	//add child sockets to thread
	for(int i = 0; i < max_clients; i++)
	{
		//socket descriptor
		sd = client_sockets[i];
		//check if valid
		if (sd>0)
		{
			FD_SET(sd, &readfds);
		}
		//highest filed descriptor
		if(sd > max_sd)
		{
			max_sd = sd;
		}
	}
	
	//wait for an activity on one of the sockets
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

	if ((activity < 0) && (errno!=EINTR)){
		cout<< "select error\n";
	}
	
	//if activity, then it is an incoming connection
	if (FD_ISSET(client, &readfds))
	{
		server = accept(client,(struct sockaddr *)&server_addr,&size);

    		// first check if it is valid or not
    		if (server < 0) 
        		cout << "=> Error on accepting..." << endl;
		 
       		strcpy(buffer, "=> Server connected...\n");
   		send(server, buffer, bufsize, 0);
	
		for(int i = 0; i < max_clients; i++)
		{
			if(client_sockets[i] == 0)
			{
				client_sockets[i] = server;
				cout<<"Adding "<<server<<" to the list of clients\n";
				break;
			}
		}
	}

	//check IO message
	for (int i = 0; i < max_clients; i++)
	{
		sd = client_sockets[i];
		
		if (FD_ISSET(sd, &readfds))
		{
			//check if it was for closing
			read(sd, buffer, 1024);
			if (buffer[0] = '\0')
			{
				//somebody disconnected
				close(sd);	
				client_sockets[i] = 0;
				break;
			}
			else
			{
				string xx = data_transmission(B, S, updates, client, server, buffer, update_flag, open);
				char yy[1024];
				strcpy(yy, xx.c_str());
				//send message to client
				send(client_sockets[0], yy, bufsize, 0);
			}
		}
	}
    }
    
    //int clientCount = 1;



    close(client);
    open = 0;
    return 0;
}




string data_transmission(multimap<float, int, greater<float>> &B, multimap<float, int> &S, vector<pair<string, pair<float,float>>> &updates, int &client, int &server, char *buffer, int &update_flag, int &open){
    bool isExit = false;    
    while (!isExit) 
    {
        
    


        
        static float temp_s = 0.0;
        static float temp_b = 0.0;
	bool b_update,s_update;
	static string symbol;
        int bytesReceived = recv(server, buffer, bufsize, 0);
        string msg(buffer);
	//perform operation on message
        if (bytesReceived != 0) 
        {
          
          if (msg.at(0) == 'C') {
			regex extract_cmsg("^C,([[:digit:]]+)");

			sregex_iterator cpos(msg.cbegin(), msg.cend(), extract_cmsg);
			sregex_iterator end;
			int c_id;
			for (; cpos != end; ++cpos) {
				c_id = stoi(cpos->str(1));
			}
			
		        //delete from B
			
		        
			for (multimap<float, int>::iterator it = B.begin(); it != B.end(); ++it) {
				if (it->second == c_id) {
					B.erase(it);
                                        //check for update after inserting in B
                                        if (B.begin()->first != temp_b)
                                        {
                                          temp_b = B.begin()->first;
                                          b_update = true;
                                        }                                                     

					break;
				}

			}

		        //delete from S
			for (multimap<float, int>::iterator it = S.begin(); it != S.end(); ++it) {
				if (it->second == c_id) {
					S.erase(it);
                                        //check for update after inserting in S
                                        if (S.begin()->first != temp_s)
                                        {
                                          temp_s = S.begin()->first;
                                          s_update = true;
                                        }
					break;
				}

			}
                        //update logic
                        if (b_update || s_update)
                         {
                           updates.push_back(make_pair(symbol,make_pair(temp_b, temp_s)));
			   
			   msg = updates.back().first + "," + to_string(updates.back().second.first)  +","+ to_string(updates.back().second.second);
			   
			   
			   update_flag = 1;
			   
                           b_update = false;
                           s_update = false;
			   return msg;
			   
                         }
		}
		else {
			regex extract_nmsg("^N,([[:digit:]]+?),([[:alpha:]]+?),([BS]),(.+?),"); 
                        //regex to match message
                        
                        float price = 0.0;
                        int id = 0;
                        
			sregex_iterator pos(msg.cbegin(), msg.cend(), extract_nmsg);
			sregex_iterator end;
			for (; pos != end; pos++) {
				/*"ID: " pos->str(1) << "Symbol: "pos->str(2)
				"Side " << pos->str(3) << "Price: " pos->str(4) << endl;*/

				id = stoi(pos->str(1));
				price = stof(pos->str(4));
                                symbol = pos->str(2);
				// stores BID messages in B map
				if (pos->str(3) == "B") {

					B.insert(make_pair(price, id));
                                        //check for update after inserting in B
                                        if (B.begin()->first != temp_b)
                                        {
                                          temp_b = price;
                                          b_update = true;
                                        }                                                     

				}
                                // stores SELL messages in S map
				else if (pos->str(3) == "S") {
					S.insert(make_pair(price, id));
                                        //check for update after inserting in S
                                        if (S.begin()->first != temp_s)
                                        {
                                          temp_s = price;
                                          s_update = true;
                                        }
				}
			}
                       
                         // update logic
                         if (b_update || s_update)
                         {
		           
                           //store the updates in vector
                           updates.push_back(make_pair(symbol,make_pair(temp_b, temp_s)));

			   msg = updates.back().first + "," + to_string(updates.back().second.first)  +","+ to_string(updates.back().second.second);
			   

			   update_flag = 1;
                           b_update = false;
                           s_update = false;
			   return msg;
			   
                         }
                                         

                                        
		}

        }
        else
        {
          open = 0;
          close(server);
	  
          cout << "\nGoodbye..." << endl;
          isExit = true;
          
        }
    }
}


