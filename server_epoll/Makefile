TARGET = server

server : server.o servertcp.o
	g++ server.o servertcp.o -o server

server.o : server.cpp servertcp.h
	g++ -c server.cpp servertcp.h -o server.o

servertcp.o : servertcp.cpp servertcp.h
	g++ -c servertcp.cpp servertcp.h -o servertcp.o

.PHONY : clean
.clean :
	-rm *.o server
