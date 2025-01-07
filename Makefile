CFLAGS = -Wall

all: spoof_temp

spoof_temp: main.cpp
	$(CXX) main.cpp -o spoof_temp -std=c++11 $(CFLAGS)

clean:
	rm -f spoof_temp
