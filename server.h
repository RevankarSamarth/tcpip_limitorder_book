#ifndef server_h
#define server_h

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;
//config for server client connection

#define portNum 1500

#define bufsize 1024

int server(vector<pair<string, pair<float,float>>>&, int&, int&);

string data_transmission(multimap<float, int, greater<float>>&, multimap<float, int>&, vector<pair<string, pair<float,float>>>&, int&, int&, char*, int&, int&);


#endif










