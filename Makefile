# CXX := g++-9
CXX=g++
CXXFLAGS= -std=c++11 -O2

main3: main3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

main2: main2.cpp
	$(CXX) $(CXXFLAGS) $< -o $@


main: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f main main2 main3
