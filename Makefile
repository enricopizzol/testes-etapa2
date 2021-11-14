replica: ReplicaManager.o ReplicaApp.o
	g++ ReplicaManager.o ReplicaApp.o -o replica -lpthread

ReplicaApp.o: ReplicaApp.cpp
	g++ -c ReplicaApp.cpp -lpthread

ReplicaManager.o: ReplicaManager.cpp 
	g++ -c ReplicaManager.cpp -lpthread

clean:
	rm -f *.o client server