#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <set>
#include <cmath>

#include "defs.h"

class Network {
private:
    // Topology of the network
    // Format: {<N1,N2>: distance}
    std::map<std::pair<int, int>, int> topology;

    std::set<int, std::less<int>> nodes;
    
    void set_topology(std::string);
    void show_topology();
    void lsrp_route(int src_node);
    void dvrp_route(int src_node);
    void modify_topology(std::pair<int, int>, int);
    void remove_route(std::pair<int, int>);

    
public:
    Network(std::string topology_file);
    void handle_command(std::string cmd);

};

#endif
