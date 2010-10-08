CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(LinuxCamera)

ADD_EXECUTABLE(LinuxCamera
Demo.cpp
#camera.cpp
)
TARGET_LINK_LIBRARIES(LinuxCamera 
raw1394 dc1394 
pthread
)


ADD_EXECUTABLE(grabcolor grabcolor.cpp)
TARGET_LINK_LIBRARIES(grabcolor raw1394 dc1394 pthread)
