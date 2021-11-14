#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdexcept>
#include <errno.h>
#include <thread>

#define TRUE 1
#define BUFFER_SIZE 2048
#define KEEP_ALIVE_TIME 5
#define SERVER_TIMEOUT 20

using namespace std;

class ReplicaManager
{

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    int mainSocket, replicaSocket = -1;

    int id, port, leaderPort, leader, leaderId, isBakcup = 0;
    string leaderIp;

    //list o sockets
    list<int> backupSocketList;

    //
    static std::atomic<bool> terminate;

public:
    ReplicaManager(int port, int id, string leaderIp, int leaderPort, int leaderId);
    // ~ReplicaManager();

    // config

    void setupServerConnection();
    void setupReplicaConnection(int new_replica_port, std::string new_replica_ip);
    void run();

    // subrotinas
    void runCommunication();
    // void KeepAlive();
    // void checkPrimaryServerTimeout();
};