CXX := g++
CXXFLAGS := -std=c++17  

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:%.cpp=%.o)

main: $(OBJECTS)
	$(CXX) $^  -o $@
	
%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<
	
clean:
	rm -rf $(OBJECTS) main
