# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
#add_library (httpProxy "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "serverDB/httpProxy.cpp")
#target_include_directories (httpProxy PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add source to this project's executable.
#add_executable (mainProxy "${CMAKE_CURRENT_SOURCE_DIR}/examples/serverAPI/mainHttpProxy")
#target_link_libraries (mainProxy LINK_PUBLIC httpProxy)
include_directories ("include")
#add_library(httpProxy "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpProxy.cpp" )
#target_link_libraries (httpProxy)
#add_executable (mainHttpProxy  "examples/serverAPI/mainHttpProxy.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpProxy.cpp")
#add_executable (socketServer  "examples/serverAPI/mainHttpProxy.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpProxy.cpp")
#add_executable (mainHttpWinProxy2.0  "examples/serverAPI/mainHttpProxy2.0.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpProxy2.0.cpp")
#add_executable (mainHttpLinuxProxy2.0  "examples/serverAPI/mainHttpProxy2.0.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpProxy2.0.cpp")
add_executable (mainWinHttpServer "examples/server/mainHttpServer.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/http/httpServer/httpServer.cpp")
#add_executable (mainLinuxHttpServer  "examples/server/mainHttpServer.cpp" "src/TCPsocket.cpp" "src/TCPserver.cpp" "src/TCPclient.cpp" "src/http/http.cpp" "src/serverDB/httpServer.cpp")

add_executable (httpClient "examples/client/httpClient.cpp" "src/TCPsocket.cpp" "src/TCPclient.cpp" "src/http/http.cpp")

#target_link_libraries (mainHttpLinuxProxy2.0 -lpthread)
#target_link_libraries (mainLinuxHttpServer -lpthread)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  #set_property(TARGET mainHttpProxy PROPERTY CXX_STANDARD 20)
  #set_property(TARGET socketServer PROPERTY CXX_STANDARD 20)
  #set_property(TARGET mainHttpWinProxy2.0 PROPERTY CXX_STANDARD  20)
  #set_property(TARGET mainHttpLinuxProxy2.0 PROPERTY CXX_STANDARD  20)
  set_property(TARGET mainWinHttpServer PROPERTY CXX_STANDARD  20)
  set_property(TARGET httpClient PROPERTY CXX_STANDARD  20)
  #set_property(TARGET mainLinuxHttpServer PROPERTY CXX_STANDARD  20)
endif()
# TODO: Add tests and install targets if needed.
