CXXFLAGS=-std=c++11 -Wall -pedantic -Wextra

SRCS=args.cpp traceroute.cpp
OBJS=$(SRCS:.cpp=.o)

%.o: %.cpp %.h
	g++ $(CXXFLAGS) $< -o $@ -c

all: trace

trace: main.cpp $(OBJS)
	g++ $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o trace
