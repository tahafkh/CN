#include "network.hpp"

int main() {
    Network *network = new Network("test.topo");
    network->handle_command("show");
}