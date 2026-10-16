CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -pthread
LDLIBS = -pthread

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean


all: $(OBJECTS)

perProcess: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPS) peerProcess

-include $(DEPS)
