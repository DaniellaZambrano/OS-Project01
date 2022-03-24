CC = gcc
VERSION = c++11

test_json.o: json.hpp test_json.cpp
	$(CC) -std=c++11 test_json.cpp -o test_json.o

clean:
	rm *.o
