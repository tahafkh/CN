#include "network.hpp"

using namespace std;

vector<int> parse_route(string route)
{
    vector<int> tokens;
    stringstream rt(route);
    string token;
    while (getline(rt, token, '-'))
        tokens.push_back(stoi(token));

    return tokens;
}

int count_digit(int number) {
    if (number == 0)
        return 1;    
    return int(log10(number) + 1);
}

Network::Network(std::string topology_file)
{
    ifstream topo(topology_file);
    if (!topo)
    {
        cerr << "Topology file not found!" << endl;
        return;
    }

    string line;
    // while (getline(topo, line);)
    //     set_topology(line);
    getline(topo, line);
    set_topology(line);
}

void Network::set_topology(std::string line)
{
    cout << line << endl;
    stringstream topo(line);
    string route;

    while (topo >> route)
    {
        if (route == TOPOLOGY)
            continue;
        vector<int> tokens = parse_route(route);
        this->nodes.insert(tokens[0]);
        this->nodes.insert(tokens[1]);

        this->topology[make_pair(tokens[0], tokens[1])] = tokens[2];
        this->topology[make_pair(tokens[1], tokens[0])] = tokens[2];
    }
}

void Network::show_topology()
{
    //-------First Row-------//
    int i = 7;
    cout << " u\\v | ";
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        if (*itr / 10) // 2 decimal val
            cout << *itr << " ";
        else // 1 decimal val
            cout << " " << *itr << " ";
        i += 3;
    }
    cout << endl;

    for (int j = 0; j < i; j++)
        cout << "-";
    cout << endl;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        if (*itr / 10) // 2 decimal val
            cout << "  " << *itr << " | ";
        else // 1 decimal val
            cout << "   " << *itr << " | ";
        for (auto itr2 = nodes.begin(); itr2 != nodes.end(); itr2++)
        {
            if (*itr == *itr2)
            {
                cout << " 0 ";
                continue;
            }
            if (this->topology.find(make_pair(*itr, *itr2)) == this->topology.end())
            {
                cout << "-1 ";
                continue;
            }
            if (topology[make_pair(*itr, *itr2)] / 10) // 2 decimal val
                cout << topology[make_pair(*itr, *itr2)] << " ";
            else // 1 decimal val
                cout << " " << topology[make_pair(*itr, *itr2)] << " ";
        }
        cout << endl;
    }
}

void Network::lsrp_route(int src_node)
{
}

void Network::dvrp_route(int src_node)
{
    map<int, int> distance;
    map<int, int> predecessor;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
        distance[*itr] = INT16_MAX;
    
    distance[src_node] = 0;

    for (std::set<int, std::less<int> >::size_type i = 1; i < nodes.size(); i++) {
        for (auto itr = topology.begin(); itr != topology.end(); itr++)
        {
            int u = itr->first.first;
            int v = itr->first.second;
            int weight = itr->second;
            if (distance[u] != INT16_MAX && distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                predecessor[v] = u;
            }
        }
        
    }     

    cout << "Dest  Next Hop  Dist  Shortest path  " << endl;
    for (int j = 0; j < 50; j++)
        cout << "-";
    cout << endl;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
        if (*itr / 10) // 2 decimal val
            cout << *itr << "    ";
        else // 1 decimal val
            cout << *itr << "     ";

        int current = *itr;
        vector<int> path;
        while (predecessor.find(current) != predecessor.end()) {
            path.push_back(current);
            current = predecessor[current];
        }

        path.push_back(src_node);

        int next_hop = path[path.size()-2];
        if (*itr == src_node)
            next_hop = src_node;

        if (next_hop / 10) // 2 decimal val
            cout << next_hop << "        ";
        else // 1 decimal val
            cout << next_hop << "         ";
        
        cout << distance[*itr];
        for (int i = 0; i < 6-count_digit(distance[*itr]); i++)
            cout << ' ';

        cout << "[ " << src_node;

        for (int i = path.size()-2; i >= 0; i--)
        {
            cout << "->" << path[i];
        }
        cout << " ]\n";
    }
}

void Network::modify_topology(std::pair<int, int> nodes, int new_cost)
{
}

void Network::remove_route(std::pair<int, int> nodes)
{
}

void Network::handle_command(std::string cmd)
{
    this->show_topology();
    this->dvrp_route(1);
}
