cflag = g++ -pthread -std=c++0x
simulator: main timekeeper generator
	chmod +x bash_file.sh run.sh

main: main.cpp 
	$(cflag) -o main main.cpp 

timekeeper:timekeeper.cpp timekeeper.h
	$(cflag) -o timekeeper timekeeper.cpp

generator:generateStudent.o generateFaculty.o \
		generateCourses.o generateFriends.o Network.o generator.o \
		Names.o setEnvironment.o graphml.o
	$(cflag) -o generator generateStudent.o generateFaculty.o \
		generateCourses.o generateFriends.o Network.o generator.o \
		Names.o setEnvironment.o graphml.o

generateStudent.o:generateStudent.cpp generateStudent.h
	$(cflag) -c generateStudent.cpp 
generateFaculty.o:generateFaculty.cpp generateFaculty.h
	$(cflag) -c generateFaculty.cpp
generateCourses.o:generateCourses.cpp generateCourses.h
	$(cflag) -c generateCourses.cpp
generateFriends.o:generateFriends.cpp generateFriends.h
	$(cflag) -c generateFriends.cpp
Network.o:Network.cpp Network.h
	$(cflag) -c Network.cpp
generator.o:generator.cpp generator.h
	$(cflag) -c generator.cpp
Names.o:Names.cpp
	$(cflag) -c Names.cpp
setEnvironment.o:setEnvironment.cpp setEnvironment.h
	$(cflag) -c setEnvironment.cpp
graphml.o:graphml.cpp graphml.h
	$(cflag) -c graphml.cpp
clean :
	rm *.o main generator timekeeper logFile.txt universities.txt graph.graphml