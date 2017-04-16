#include "networking/networker.h"
#include "global_constants.h"

Networker::Networker(bool isserver, WriteBuffer &sendbuffer_) : sendbuffer(sendbuffer_)
{
    //ctor
}

Networker::~Networker()
{
    enet_host_destroy(host);
}
