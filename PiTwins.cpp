#include "Server.h"
#include "ServerMsgDispatcher.h"

using namespace std;

int main(int argc, char *argv[]) {
    PiRPC::Server::getMsgInstance().init(ADDR_MSG_SERVER, NAME_MSG_SERVER);
    PiRPC::Server::getMsgInstance().setMessageDispatcher(&(ServerMsgDispatcher::getInstance()));
    PiRPC::Server::getMsgInstance().run();
}
