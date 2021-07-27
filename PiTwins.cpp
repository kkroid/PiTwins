#include "Server.h"
#include "ServerMsgReceiver.h"

using namespace std;

int main(int argc, char *argv[]) {
    PiRPC::Server::getMsgInstance().init(ADDR_MSG_SERVER, NAME_MSG_SERVER);
    PiRPC::Server::getMsgInstance().setOnNewMsgReceivedCallback([](const char *data, size_t size) {
        ServerMsgReceiver::getInstance().onNewMsgReceived(std::string(data, size));
    });
    PiRPC::Server::getMsgInstance().run();
}
