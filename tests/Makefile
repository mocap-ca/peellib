all : broadcast_listen broadcast_send

c3dTest : ../src/c3dFile.cpp ../include/peel/c3dFile.h c3dTest.cpp
	g++ -o $@ -I../include/ c3dTest.cpp ../src/c3dFile.cpp

broadcast_send :  broadcast_send.cpp
	g++ -g3 -O0 -I../include/ broadcast_send.cpp ../libpeeld.a -o broadcast_send

broadcast_listen : broadcast_listen.cpp
	g++ -g3 -O0 -I../include/ broadcast_listen.cpp ../libpeeld.a -o broadcast_listen

unix_socket : unix_socket.cpp
	g++ -g3 -O0 -I../include/ unix_socket.cpp ../libpeeld.a -o unix_socket

clean :
	-rm broadcast_listen
	-rm broadcast_send


