.PHONY: all build run clean


all: build

build:
	g++ main.cpp -std=c++11 -o main

run:
	./main

clean:
	rm -f main
