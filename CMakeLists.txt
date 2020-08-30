# cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

add_library(peellib STATIC  
   src/BinderParser.cpp
   src/ListenThread.cpp
   src/SocketAddress.cpp
   src/XmlParser.cpp
   src/BroadcastListenThread.cpp
   src/Log.cpp
   src/SocketException.cpp
   src/xmltag.cpp
   src/c3dFile.cpp
   src/profile.cpp
   src/XmlTagGet.cpp
   src/File.cpp
   src/Signal.cpp
   src/Util.cpp
   src/HttpSocket.cpp
   src/Socket.cpp
   src/WinUtil.cpp)

#   src/UnixUtil.cpp

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


install(TARGETS  peellib DESTINATION ${CMAKE_SOURCE_DIR}/lib)
