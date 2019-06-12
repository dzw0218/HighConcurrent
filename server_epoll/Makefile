TARGET = server

server : server.o servertcp.o
	g++ -std=c++11 server.o servertcp.o -o server

server.o : server.cpp servertcp.h
	g++ -std=c++11 -c server.cpp servertcp.h

servertcp.o : servertcp.cpp servertcp.h
	g++ -std=c++11 -c servertcp.cpp servertcp.h

.PHONY : clean
clean :
	-rm *.o server
