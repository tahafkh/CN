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

int count_digit(int number)
{
    if (number == 0)
        return 1;
    return int(log10(number) + 1);
}

int Network::findClosestNode(map<int, int> &distance, set<int> &sptSet)
{
    int min = INT16_MAX, closes_node = -1;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
        int v = *itr;
        if (distance[v] < min && sptSet.find(v) == sptSet.end())
        {
            min = distance[v];
            closes_node = v;
        }
    }

    return closes_node;    
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

void Network::print_lsrp_routing_table(int src_node, map<int, int> &predecessor, map<int, int> &distance) {
    cout << "Path: [s]->[d] Min-Cost Shortest Path" << endl;
    cout << "     ";
    for (int i = 0 ; i < 3 ; i++) {
        for (int j = 0 ; j < 11 ; j++)
            cout<<"-";
        cout<<"  ";
    }
    cout << endl;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
    {   
        int current = *itr;
        if (current == src_node)
            continue;
        vector<int> path;
        while (predecessor.find(current) != predecessor.end())
        {
            path.push_back(current);
            current = predecessor[current];
        }

        path.push_back(src_node);
        int v = *itr;

        cout << "      ";
        cout << "[" << src_node << "] -> [" << v << "]";

        cout << "       ";
        cout << distance[v];
        cout << "       ";

        cout << src_node;
        for (int i = path.size() - 2; i >= 0; i--)
        {
            cout << "->" << path[i];
        }
        cout << "\n";
    }
}

void Network::print_routing_table(int src_node, map<int, int> &predecessor, map<int, int> &distance)
{
    cout << "Dest  Next Hop  Dist  Shortest path  " << endl;
    for (int j = 0; j < 50; j++)
        cout << "-";
    cout << endl;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        if (*itr / 10) // 2 decimal val
            cout << *itr << "    ";
        else // 1 decimal val
            cout << *itr << "     ";

        int current = *itr;
        vector<int> path;
        while (predecessor.find(current) != predecessor.end())
        {
            path.push_back(current);
            current = predecessor[current];
        }

        path.push_back(src_node);

        int next_hop = path[path.size() - 2];
        if (*itr == src_node)
            next_hop = src_node;

        if (next_hop / 10) // 2 decimal val
            cout << next_hop << "        ";
        else // 1 decimal val
            cout << next_hop << "         ";

        cout << distance[*itr];
        for (int i = 0; i < 6 - count_digit(distance[*itr]); i++)
            cout << ' ';

        cout << "[ " << src_node;

        for (int i = path.size() - 2; i >= 0; i--)
        {
            cout << "->" << path[i];
        }
        cout << " ]\n";
    }
}

void Network::print_iteration_table(int itr_num, map<int, int> &distance) {
    cout << "                Iter " << itr_num << ":               "<< endl;
    cout << "Dest            ";
    for (auto itr = nodes.begin() ; itr != nodes.end() ; itr++) {
        cout << "| " << *itr << " |  ";
    }
    cout << endl;
    cout << "Cost            ";
    for (auto itr = nodes.begin() ; itr != nodes.end() ; itr++) {
        int v = *itr;
        if (distance[v] == INT16_MAX)
            cout << "| " << NO_ROUTE << " |  ";
        else
            cout << "| " << distance[v] << " |  ";
    }
    cout << endl;
    for (int j = 0; j < 40 ; j++)
        cout << "-";
    cout << endl;
    cout << endl;
}

void Network::set_topology(std::string line)
{
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

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
        for (auto itr2 = nodes.begin(); itr2 != nodes.end(); itr2++) {
            if (*itr != *itr2 && this->topology.find(make_pair(*itr, *itr2)) == this->topology.end()) {
                this->topology[make_pair(*itr, *itr2)] = NO_ROUTE;
            }
        }
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
            // if (this->topology.find(make_pair(*itr, *itr2)) == this->topology.end())
            if (this->topology[make_pair(*itr, *itr2)] == NO_ROUTE)
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
    set<int> sptSet;
    map<int, int> distance;
    map<int, int> predecessor;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
        distance[*itr] = INT16_MAX;

    distance[src_node] = 0;

    while (sptSet.size() != this->nodes.size())
    {
        int u = findClosestNode(distance, sptSet);
        if (u == -1)
        {
            cerr << "No close nodes found" << endl;
            return;
        }
        sptSet.insert(u);

        for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
        {
            int v = *itr;
            if (sptSet.find(v) == sptSet.end() && this->topology[make_pair(u, v)] != NO_ROUTE && distance[u] != INT16_MAX && distance[u] + topology[make_pair(u, v)] < distance[v])
            {
                distance[v] = distance[u] + topology[make_pair(u, v)];
                predecessor[v] = u;
            }
        }

        print_iteration_table(sptSet.size(), distance);
    }

    print_lsrp_routing_table(src_node, predecessor, distance);

}

void Network::dvrp_route(int src_node)
{
    map<int, int> distance;
    map<int, int> predecessor;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
        distance[*itr] = INT16_MAX;

    distance[src_node] = 0;

    for (std::set<int, std::less<int>>::size_type i = 1; i < nodes.size(); i++)
    {
        for (auto itr = topology.begin(); itr != topology.end(); itr++)
        {
            int u = itr->first.first;
            int v = itr->first.second;
            int weight = itr->second;
            if (distance[u] != INT16_MAX && distance[u] + weight < distance[v])
            {
                distance[v] = distance[u] + weight;
                predecessor[v] = u;
            }
        }
    }

    print_routing_table(src_node, predecessor, distance);
}

void Network::modify_topology(std::pair<int, int> nodes, int new_cost)
{
    this->topology[nodes] = new_cost;
}

void Network::remove_route(std::pair<int, int> nodes)
{
    this->topology[nodes] = NO_ROUTE;
}

void Network::handle_command(std::string cmd)
{
    string command;
    stringstream line(cmd);
    line >> command;
    if (command == TOPOLOGY)
        set_topology(cmd);
    else if (command == SHOW)
        show_topology();
    else if (command == LSRP)
    {
        int src_node;
        line >> src_node;
        lsrp_route(src_node);
    }
    else if (command == DVRP)
    {
        int src_node;
        line >> src_node;
        dvrp_route(src_node);
    }
    else if (command == MODIFY)
    {
        string route;
        line >> route;
        vector<int> tokens = parse_route(route);
        modify_topology(make_pair(tokens[0], tokens[1]), tokens[2]);
        modify_topology(make_pair(tokens[1], tokens[0]), tokens[2]);
    }
    else if (command == REMOVE)
    {
        string route;
        line >> route;
        vector<int> tokens = parse_route(route);
        remove_route(make_pair(tokens[0], tokens[1]));
        remove_route(make_pair(tokens[1], tokens[0]));
    }
    else
        cerr << "Unknown Command" << endl;
}
