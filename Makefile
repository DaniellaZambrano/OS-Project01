CC = g++
VERSION = -std=c++11

tests: test/test_json.cpp test/test_production_card.cpp
	$(CC) $(VERSION) -I. test/test_json.cpp -o test_json.sh
	$(CC) $(VERSION) -I. test/test_production_card.cpp -o test_production_card.sh

clean:
	rm -R *.sh
