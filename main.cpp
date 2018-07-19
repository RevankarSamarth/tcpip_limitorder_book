
#include <iostream>
#include <vector>
#include <thread>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <unistd.h>
#include <bits/stdc++.h>

using namespace std;

#define SHM_SIZE 1024

int server(vector<pair<string, pair<float,float>>>&, int&, int&);


int main(){
   
   //for creating shared memory segment
   key_t key;
   int shmid;
   char* data;
   int mode;

   //key generation
   if ((key = ftok(".", 'R')) == -1){
     cout << "key error\n";
     exit(1);
   }

   //pointer to shared memory
   if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1){
     cout<<"shmid\n";
     exit(1);
   }
   data =(char*)shmat(shmid, NULL, 0);
   
   

   //variable to be set when updating the best bid and ask
   static int update_flag = 0;
   
   static int open_soc = 1;// to check if application has been closed
   
   //vector to hold the updates(stack could be a better appraoch)
   vector<pair<string, pair<float,float>>> updates;
   
   //thread to spawn the server
   thread s(server, ref(updates), ref(update_flag), ref(open_soc));  // this will run the server and await for connection from client	
   
   cout<<"Updates written in shared memory also displayed on server\n";

   while(open_soc){
	if (update_flag){
                    string buf = updates.back().first + "," + to_string(updates.back().second.first)  +","+ to_string(updates.back().second.second);
		    char msg[buf.length() + 1];
                
		    strcpy(msg, buf.c_str());
		    //write to the shared memory
                    strcpy(data, msg);
 		
		    //display the updates on server
                    cout << data << endl;
		    update_flag = 0;
	}
	
   }
	
   
   shmdt(data);
   shmctl(shmid, IPC_RMID, NULL);  // delete the shared memory segment
   //data = NULL;
   s.join();
   cin.get();
   return 0;
}
