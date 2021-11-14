#include "ReplicaManager.hpp"
#include <errno.h>
#include <stdexcept>
sockaddr_in serv_addr, cli_addr;
socklen_t cliLen;

std::string appendErrorMessage(const std::string message)
{
    return message + " (" + std::string(strerror(errno)) + ")";
}

void ReplicaManager::runCommunication()
{
    int buffer = 99;
    int heartBeat = 5;
    bool terminate = false;

    std::cout << "Entrou no runCommunication" << std::endl;
    while (!terminate)
    {

        if (this->isBakcup == 1)
        {
            while (terminate == false || (this->port != this->leaderPort))
            {
                sleep(KEEP_ALIVE_TIME + 1);

                recv(replicaSocket, &buffer, sizeof(int), 0);
                cout << "\nBuffer: " << buffer << endl;
                if (buffer != 5)
                {
                    printf("Primary server timed out.\n");
                    // startElection(server);
                };
            }
        }
        else if (this->isBakcup == 0)
        {
            while (TRUE)
            {
                sleep(KEEP_ALIVE_TIME);
                std::cout << "Sending keep alive message to replica" << std::endl;

                for (auto &socket : backupSocketList)
                {

                    {
                        if (socket)
                        {
                            std::cout << "Real sending" << std::endl;
                            send(socket, &heartBeat, sizeof(heartBeat), 0);
                        }
                    }
                };
            }
        }
    }
}

ReplicaManager::ReplicaManager(int port, int id, string leaderIp, int leaderPort, int leaderId)
{

    this->leader = leader;
    this->id = id;
    this->port = port;
    this->leaderIp = leaderIp;
    this->leaderPort = leaderPort;

    //ReplicaManager::election_started = false;

    // If this is not the leader replica
    if (this->leader == this->id)
    {
        // inicializa servidor primário
        ReplicaManager::setupServerConnection();
         cout << "inicializou socket lider " << endl;
    }
    else
    {

        cout << "entrou no if do backup " << endl;
        this->isBakcup = 1;
        // cria cliente
        // incializa conexão do backup
        ReplicaManager::setupReplicaConnection(this->leaderPort, this->leaderIp);

        // inicializa servidor primário
        ReplicaManager::setupServerConnection();
    }

   
}

void ReplicaManager::setupReplicaConnection(int replicaPort, std::string replicaIp)
{

    cout << "entrou no setupReplicaConnection" << endl;
    // Create socket
    if ((this->replicaSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error(appendErrorMessage("Error connecting to server"));

    // Fill server socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(replicaPort);
    server_address.sin_addr.s_addr = inet_addr(replicaIp.c_str());

    // Try to connect to replica
    if (connect(this->replicaSocket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        throw std::runtime_error(appendErrorMessage("Error connecting to server"));

    cout << "saiu do setupReplicaConnection" << endl;
}

void ReplicaManager::setupServerConnection()
{

    cout << "entrou no setupServerConnection" << endl;
    // Create socket
    if ((this->mainSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error(appendErrorMessage("Error during socket creation"));

    // Prepare server socket address (Where the server listens at)
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(ReplicaManager::port);

    // Set socket options
    int yes = 1;
    if (setsockopt(this->mainSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw std::runtime_error(appendErrorMessage("Error setting socket options"));

    // Bind socket
    if (bind(mainSocket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        throw std::runtime_error(appendErrorMessage("Error during socket bind"));
    
    listen(mainSocket,20);
}

void ReplicaManager::run()
{
    int newsockfd;
    char bufferUserId[20];
    std::string name;
    int threadType;
    int command;
    int user_validation = 255;
    struct sockaddr_in backup_address;

    std::cout << "Entrou no Replica run" << std::endl;
    // readConfig();
    if (this->isBakcup == 0)
    {
        int backupSocket, backupsConnected = 0;

        while (backupsConnected < 3)
        {
            std::cout << "Tentando conectar ao backup" << std::endl;

            // tenta conectar ao socket
            backupSocket = accept(this->mainSocket, (struct sockaddr *)&client_address, &cliLen);
            if (backupSocket != -1)
            {
                // adicionar na lista de sockets
                backupSocketList.push_back(backupSocket);
                std::cout << "Connected to the backup" << std::endl;
                backupsConnected++;
            }
            sleep(2);
        }
    }
    
    thread RmCommunicationThread(&ReplicaManager::runCommunication, this);
    RmCommunicationThread.detach();

    while (TRUE)
    {

         std::cout << "ENTROU NO WHILE TRUE" << std::endl;
        if ((newsockfd = accept(this->mainSocket, (struct sockaddr *)&cli_addr, &cliLen)) == -1)
        {
            std::cout << "Error, Server was not able to accept client connection" << std::endl;
        }

        // receive client machine id
        int machineId;
        read(newsockfd, &machineId, sizeof(machineId));

        // receive client thread type
        read(newsockfd, &threadType, sizeof(threadType));

        // receive user id
        read(newsockfd, bufferUserId, sizeof(bufferUserId));
        name = bufferUserId;

        // thread incomingThread, outgoingThread;
        if (threadType == 1)
        {
            // print user name for server debug
            std::cout << "userId connected:" << name << " -"
                      << "machine id connected: " << machineId << endl;

            // // verify if user is able to connect or not
            // if (verifyUserId("@" + name, this->profile_list, machineId)) //quer dizer que o usuario ainda não existe
            // {
            //     // send to client user aproval to log in
            //     user_validation = 1;
            //     write(newsockfd, &user_validation, sizeof(user_validation));
            //     thread incomingThread(&Server::runIncomingThread, this, "@" + name, newsockfd, machineId);
            //     incomingThread.detach();
            // }
            // else
            // {
            //     // send to client user disapproval to log in
            //     user_validation = 0;
            //     write(newsockfd, &user_validation, sizeof(user_validation));
            // }
        }
        else if (threadType == 2)
        {

            // Outgoing thread selected
            name = "@" + name;
            // thread outgoingThread(&Server::runOutgoingThread, this, name, newsockfd, machineId);
            // outgoingThread.detach();
        }
        else
        {
            std::cout << "Thread type is not supported" << std::endl;
        }
    }
}

// void ReplicaManager::KeepAlive()
// {
// }

// void ReplicaManager::checkPrimaryServerTimeout()
// {
// }