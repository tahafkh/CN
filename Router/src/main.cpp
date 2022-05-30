#include "network.hpp"

using namespace std;

int main() {
    Network *network = new Network("test.topo");
    string cmd;
    while (getline(cin, cmd))
        network->handle_command(cmd);
    
}