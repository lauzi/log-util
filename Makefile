CXX := g++-9
# CXX=g++
CXXFLAGS= -std=c++11 -O2

main: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f main
