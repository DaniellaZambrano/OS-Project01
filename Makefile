CC = g++
VERSION = -std=c++14
LIBS = -lpthread

station_1: station_1.cpp json.hpp
	$(CC) $(VERSION) -I. station_1.cpp -o station_1.exe $(LIBS)

station_2: station_2.cpp json.hpp
	$(CC) $(VERSION) -I. station_2.cpp -o station_2.exe $(LIBS)

tests: test/test_json.cpp test/test_production_card.cpp
	$(CC) $(VERSION) -I. test/test_json.cpp -o test_json.exe
	$(CC) $(VERSION) -I. test/test_production_card.cpp -o test_production_card.exe

clean:
	rm -R *.exe
