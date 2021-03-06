all:	 TestMessage TestRedirection TestLectureEcriture terminal acquisition autorisation 

autorisation.o: autorisation.c
	gcc -Wall -g -c autorisation.c

autorisation: autorisation.o message.o lectureEcriture.o annuaire.o alea.o
	gcc -Wall autorisation.o message.o lectureEcriture.o annuaire.o alea.o -o autorisation

message.o: message.c message.h
	gcc -Wall -g -c message.c

alea.o: alea.h alea.c
	gcc -Wall -g -c alea.c

terminal.o: terminal.c
	gcc -Wall -g -c terminal.c

terminal: terminal.o message.o lectureEcriture.o alea.o annuaire.o
	gcc -Wall terminal.o message.o lectureEcriture.o alea.o annuaire.o -o terminal

acquisition.o : acquisition.c
	gcc -Wall -g -c acquisition.c

acquisition: acquisition.o lectureEcriture.o message.o
	gcc -Wall acquisition.o lectureEcriture.o message.o -o acquisition -lpthread

testAnnuaire: testAnnuaire.c annuaire.o alea.o lectureEcriture.o
	gcc testAnnuaire.c annuaire.o alea.o lectureEcriture.o -o testAnnuaire

annuaire.o: annuaire.c annuaire.h alea.h lectureEcriture.h
	gcc -c annuaire.c


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
