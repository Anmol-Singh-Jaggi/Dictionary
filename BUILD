# Change these according to the Boost installation path in your system
BOOST_HEADER_PATH=~/.boost_home/include
BOOST_OBJECT_PATH=~/.boost_home/lib

g++ -std=c++11 -fexceptions -isystem $BOOST_HEADER_PATH -c dictionary.cpp -o dictionary.o
g++ -o dictionary dictionary.o $BOOST_OBJECT_PATH/libboost_filesystem.a $BOOST_OBJECT_PATH/libboost_system.a
