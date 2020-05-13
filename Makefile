SRC= BroadcastListenThread.cpp File.cpp HttpSocket.cpp ListenThread.cpp Log.cpp Signal.cpp Socket.cpp SocketAddress.cpp SocketException.cpp UnixUtil.cpp Util.cpp c3dFile.cpp

SRCS:=$(SRC:%=src/%)
OBJS:=${SRC:.cpp=.o}
DOBJS = $(addprefix debug/, $(OBJS))
ROBJS = $(addprefix release/, $(OBJS))

all: debug release
debug: libpeeld.a

release: libpeel.a

libpeeld.a: $(DOBJS)
	ar -rcs $@ $<

libpeel.a: $(ROBJS)
	ar -rcs $@ $<


debug/%.o: src/%.cpp
	@mkdir -p debug
	g++ -Iinclude -O0 -g $(CPPFLAGS) -o $@ -c $<

release/%.o : src/%.cpp
	@mkdir -p release
	g++ -Iinclude -O3 $(CPPFLAGS) -o $@ -c $<

clean:
	rm $(DOBJS)
	rm $(ROBJS)
	rm libpeel.a
	rm libpeeld.a
