all:	 TestMessage TestRedirection TestLectureEcriture terminal acquisition

message.o: message.c message.h
	gcc -Wall -c message.c

alea.o: alea.h alea.c
	gcc -Wall -c alea.c

terminal.o: terminal.c
	gcc -Wall -c terminal.c

terminal: terminal.o message.o lectureEcriture.o
	gcc -Wall terminal.o message.o lectureEcriture.o -o terminal

acquisition.o : acquisition.c
	gcc -Wall -c acquisition.c

acquisition: acquisition.o lectureEcriture.o
	gcc -Wall acquisition.o lectureEcriture.o -o acquisition

TestMessage: message.o alea.o TestMessage.c
	gcc -Wall TestMessage.c message.o alea.o -o  TestMessage

TestRedirection: TestRedirection.c
	gcc -Wall TestRedirection.c -o  TestRedirection

lectureEcriture.o: lectureEcriture.c lectureEcriture.h
	gcc -c -Wall lectureEcriture.c

TestLectureEcriture: lectureEcriture.o TestLectureEcriture.c
	gcc lectureEcriture.o TestLectureEcriture.c -o TestLectureEcriture

clean:	
	rm -f *.o *~ 

cleanall: clean
	rm TestRedirection TestMessage TestLectureEcriture
