CC = gcc
VERSION = -std=c++11

test_json.o: json.hpp test_json.cpp
	$(CC) $(VERSION) -I. test_json.cpp -o test_json.o

clean:
	rm *.o
