target = server
source = servertcp.o server.o

server : $(source)
	g++ -Wall -O2 $(source) -o server

server.o : server.cpp servertcp.h
	g++ -Wall -O2 -std=c++11 -c server.cpp

servertcp.o : servertcp.cpp servertcp.h
	g++ -Wall -O2 -std=c++11 -c servertcp.cpp

.PHONY:clean
clean:
	rm *.o server
