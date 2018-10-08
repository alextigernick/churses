cmake_minimum_required(VERSION 2.6)
PROJECT(3dont)

# Usage: 
#   cmake .. -DCMAKE_INCLUDE_PATH=/home/ott/exp/include -DCMAKE_LIBRARY_PATH=/home/ott/exp/lib
#       -DBOOST_INCLUDEDIR=/home/ott/exp/include -DBOOST_LIBRARYDIR=/home/ott/exp/lib
#

ADD_EXECUTABLE(main src/main.cpp)
TARGET_LINK_LIBRARIES(main -lncurses ${USED_LIBS})

#ADD_EXECUTABLE(test-otpc-tp test-otpc-tp.cpp test-otpc-conn.cpp)
#TARGET_LINK_LIBRARIES(test-otpc-tp ${USED_LIBS})