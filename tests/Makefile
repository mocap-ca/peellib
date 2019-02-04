all : broadcast_listen broadcast_send listener

CFLAGS = -O0 -g -I../include/

c3dTest : ../src/c3dFile.cpp ../include/peel/c3dFile.h c3dTest.cpp
	g++ -o $@ $(CFLAGS) c3dTest.cpp ../src/c3dFile.cpp

broadcast_send :  broadcast_send.cpp
	g++ -g3 -O0 $(CFLAGS) broadcast_send.cpp ../peellib.a -o broadcast_send

listener : listener.cpp
	g++ -g3 -O0 $(CFLAGS) listener.cpp ../peellib.a -o listener

broadcast_listen : broadcast_listen.cpp
	g++ -g3 -O0 $(CFLAGS) broadcast_listen.cpp ../peellib.a -o broadcast_listen

unix_socket : unix_socket.cpp
	g++ -g3 -O0 $(CFLAGS) unix_socket.cpp ../peellib.a -o unix_socket

clean :
	-rm broadcast_listen
	-rm broadcast_send
	-rm listener


