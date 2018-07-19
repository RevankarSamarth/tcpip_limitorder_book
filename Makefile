main_server: main.o server.o
	g++ -o main_server main.o server.o -lpthread -lrt

main.o: main.cpp
	g++ -c main.cpp

server.o: server.cpp server.h
	g++ -c server.cpp  






clean:
	rm *.o main_server 
