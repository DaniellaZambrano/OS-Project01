CC = g++
VERSION = -std=c++14
LIBS = -lpthread

all: station_1 station_2 station_3

station_1: station_1.cpp 
	$(CC) $(VERSION) -I. station_1.cpp -o station_1.exe $(LIBS)

station_2: station_2.cpp 
	$(CC) $(VERSION) -I. station_2.cpp -o station_2.exe

station_3: station_3.cpp 
	$(CC) $(VERSION) -I. station_3.cpp -o station_3.exe

tests: test/test_json.cpp test/test_production_card.cpp
	$(CC) $(VERSION) -I. test/test_json.cpp -o test_json.exe
	$(CC) $(VERSION) -I. test/test_production_card.cpp -o test_production_card.exe

clean:
	rm -R *.exe
