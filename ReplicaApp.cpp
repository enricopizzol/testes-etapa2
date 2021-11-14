
#include "ReplicaManager.hpp"

int main(int argc, char const *argv[])
{

    int port = atoi(argv[1]);
    int id = atoi(argv[2]);
    int liderPort = atoi(argv[3]);
    int liderId = atoi(argv[4]);

    ReplicaManager replica(port, id , "127.0.0.1", liderPort, liderId);

    replica.run();

    return 0;
}