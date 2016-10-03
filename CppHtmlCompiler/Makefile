.PHONY:all
all:JSON LIB CppHtmlVM.o main.o
	g++ *.o json/*.o lib/*.o -std=c++0x -o chvm
CppHtmlVM.o:
	g++ -std=c++0x -c CppHtmlVM.cpp
main.o:
	g++ -c main.cpp
.PHONY:JSON
JSON:
	make -C json/
.PHONY:LIB
LIB:
	make -C lib/
.PHONY:clean
clean:
	-rm chvm
	-rm *.o
	make -C json/ clean
	make -C lib/ clean
