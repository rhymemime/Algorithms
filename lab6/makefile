all: RG-Tester a.out

clean:
	rm -f a.out RG-Tester 
	sh -c "if [ -f RainbowGraph.cpp ]; then mv RainbowGraph.cpp RainbowGraph.cpp.backup; fi"

RG-Tester: RG-Tester.cpp RG.cpp RG.h
	g++ -O2 -o RG-Tester RG-Tester.cpp RG.cpp

a.out:	RainbowGraph.cpp main.cpp
	g++ -O2 main.cpp

RainbowGraph.cpp: RG.cpp RG.h
	sh -c "if [ -f RainbowGraph.cpp ]; then mv RainbowGraph.cpp RainbowGraph.cpp.backup; fi"
	cat RG.h RG.cpp | sed '/include .RG\.h/d' > RainbowGraph.cpp

