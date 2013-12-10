flag2 = g++ -c -lpthread
analyser: analyser.o Dijkstra.o FloydWar.o Network.o parser.o Questions.o 
	g++ -o analyser analyser.o Dijkstra.o FloydWar.o Network.o parser.o \
			Questions.o
	chmod +x run.sh
analyser.o:
	$(flag2) analyser.cpp
Dijkstra.o:
	$(flag2) Dijkstra.cpp 
FloydWar.o:
	$(flag2) FloydWar.cpp 
Network.o:
	$(flag2) Network.cpp 
parser.o:
	$(flag2) parser.cpp 
Questions.o:
	$(flag2) Questions.cpp 
clean:
	rm *.o analyser *.gch