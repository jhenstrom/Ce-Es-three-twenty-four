
generate:generate.o random.o graph.o dot.o
	g++ -g -o generate generate.o random.o graph.o dot.o

generate.o: dot.h random.h generate.cpp
	g++ -c -g generate.cpp

random.o: random.cpp random.h
	g++ -c -g random.cpp

graph.o: graph.cpp graph.h random.h
	g++ -c -g graph.cpp

dot.o: dot.cpp dot.h
	g++ -c -g dot.cpp
