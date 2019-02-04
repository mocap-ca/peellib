SRC=BinderParser.cpp BroadcastListenThread.cpp File.cpp HttpSocket.cpp ListenThread.cpp Log.cpp Signal.cpp Socket.cpp SocketAddress.cpp SocketException.cpp UnixUtil.cpp Util.cpp c3dFile.cpp

SRCS:=$(SRC:%=src/%)
OBJS:=${SRCS:.cpp=.o}
CPPFLAGS = -Iinclude -O0 -g


.cpp.o:
	$(CC) $(CPPFLAGS) -o $@ -c $<

all: build $(OBJS)
	echo $(SRC)
	echo $(SRCS)
	echo $(OBJS)
	ar -rcs peellib.a $(OBJS)

build:
	mkdir build

clean:
	rm src/*.o
