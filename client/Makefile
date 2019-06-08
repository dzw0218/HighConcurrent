target = client
source = client.o clienttcp.o

client : client.o clienttcp.o
	g++ -std=c++11 client.o clienttcp.o -o client

client.o : clienttcp.h client.cpp
	g++ -std=c++11 -c client.cpp

clienttcp.o : clienttcp.h clienttcp.cpp
	g++ -std=c++11 -c clienttcp.cpp

.PHONY : clean
clean : 
	-rm $(source) client
