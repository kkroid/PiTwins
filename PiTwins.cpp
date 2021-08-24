#include "Server.h"
#include "ServerMsgDispatcher.h"

using namespace std;

#define ADDR_MSG_SERVER "0.0.0.0:5556"
#define NAME_MSG_SERVER "MsgServer"

int main(int argc, char *argv[]) {
    PiRPC::Server::getMsgInstance().init(ADDR_MSG_SERVER, NAME_MSG_SERVER);
    PiRPC::Server::getMsgInstance().setMessageDispatcher(&(ServerMsgDispatcher::getInstance()));
    PiRPC::Server::getMsgInstance().run();
}
