a.out: libJTelSvr.a t1.cpp
	g++ -g t1.cpp libJTelSvr.a -I../include -lpthread -o a.out

libJTelSvr.a:JTelSvr.o MyMisc.o
	ar -rc libJTelSvr.a JTelSvr.o MyMisc.o

JTelSvr.o:../source/TelSvrLib.cpp ../include/TelSvrLib.h
	g++ -c ../source/TelSvrLib.cpp -I../include -Wall -o JTelSvr.o

MyMisc.o:../source/MyMisc.cpp ../include/MyMisc.h
	g++ -c ../source/MyMisc.cpp -I../include -Wall -o MyMisc.o

