#include "RouteWise.h"

CityGraph::CityGraph()
{
    totalLocations = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
    {
        adjacency[i] = NULL;
        for (int j = 0; j < MAX_VERTICES; j++)
            matrix[i][j] = 0;
    }
}

CityGraph::~CityGraph()
{
    for (int i = 0; i < totalLocations; i++)
    {
        RoadEdge* current = adjacency[i];
        while (current != NULL)
        {
            RoadEdge* nextRoad = current->next;
            delete current;
            current = nextRoad;
        }
    }
}

int CityGraph::findLocation(const string& name) const
{
    for (int i = 0; i < totalLocations; i++)
    {
        if (locations[i] == name)
            return i;
    }
    return -1;
}

bool CityGraph::addLocation(const string& name)
{
    if (name.empty() || totalLocations == MAX_VERTICES || findLocation(name) != -1)
        return false;

    locations[totalLocations] = name;
    totalLocations++;
    return true;
}

void CityGraph::addRoadByIndex(int first, int second, int distance)
{
    RoadEdge* current = adjacency[first];
    while (current != NULL)
    {
        if (current->destination == second)
        {
            current->distance = distance;
            break;
        }
        current = current->next;
    }

    if (current == NULL)
    {
        RoadEdge* newRoad = new RoadEdge;
        newRoad->destination = second;
        newRoad->distance = distance;
        newRoad->next = adjacency[first];
        adjacency[first] = newRoad;
    }

    matrix[first][second] = distance;
}

bool CityGraph::addRoad(const string& first, const string& second, int distance)
{
    int from = findLocation(first);
    int to = findLocation(second);

    if (from == -1 || to == -1 || from == to || distance <= 0)
        return false;

    addRoadByIndex(from, to, distance);
    addRoadByIndex(to, from, distance);
    return true;
}

bool CityGraph::loadFromFile(const string& fileName)
{
    ifstream file(fileName.c_str());
    if (!file)
        return false;

    int locationCount;
    if (!(file >> locationCount) || locationCount < 0 || locationCount > MAX_VERTICES)
        return false;

    file.ignore(10000, '\n');
    for (int i = 0; i < locationCount; i++)
    {
        string name;
        if (!getline(file, name))
            return false;
        if (!addLocation(name))
            return false;
    }

    int roadCount;
    if (!(file >> roadCount) || roadCount < 0)
        return false;

    for (int i = 0; i < roadCount; i++)
    {
        int first, second, distance;
        if (!(file >> first >> second >> distance))
            return false;
        if (first < 0 || first >= totalLocations || second < 0 || second >= totalLocations)
            return false;
        if (first != second && distance > 0)
            addRoadByIndex(first, second, distance);
        if (first != second && distance > 0)
            addRoadByIndex(second, first, distance);
    }

    return true;
}

bool CityGraph::saveToFile(const string& fileName) const
{
    ofstream file(fileName.c_str());
    if (!file)
        return false;

    file << totalLocations << endl;
    for (int i = 0; i < totalLocations; i++)
        file << locations[i] << endl;

    int roadCount = 0;
    for (int i = 0; i < totalLocations; i++)
    {
        for (int j = i + 1; j < totalLocations; j++)
        {
            if (matrix[i][j] > 0)
                roadCount++;
        }
    }

    file << roadCount << endl;
    for (int i = 0; i < totalLocations; i++)
    {
        for (int j = i + 1; j < totalLocations; j++)
        {
            if (matrix[i][j] > 0)
                file << i << " " << j << " " << matrix[i][j] << endl;
        }
    }

    return true;
}

void CityGraph::displayNetwork() const
{
    if (totalLocations == 0)
    {
        cout << "The city network has no locations yet.\n";
        return;
    }

    cout << "\nCITY NETWORK - ADJACENCY LIST\n";
    for (int i = 0; i < totalLocations; i++)
    {
        cout << locations[i] << " -> ";
        RoadEdge* current = adjacency[i];
        if (current == NULL)
            cout << "No connected roads";
        while (current != NULL)
        {
            cout << locations[current->destination] << " (" << current->distance << " km)";
            if (current->next != NULL)
                cout << " -> ";
            current = current->next;
        }
        cout << endl;
    }

    cout << "\nCITY NETWORK - ADJACENCY MATRIX\n";
    cout << "      ";
    for (int i = 0; i < totalLocations; i++)
        cout << i << "    ";
    cout << endl;

    for (int i = 0; i < totalLocations; i++)
    {
        cout << i << "     ";
        for (int j = 0; j < totalLocations; j++)
        {
            if (matrix[i][j] == 0)
                cout << "-    ";
            else
                cout << matrix[i][j] << "    ";
        }
        cout << " | " << locations[i] << endl;
    }
}

void CityGraph::breadthFirstSearch(const string& start) const
{
    int startIndex = findLocation(start);
    if (startIndex == -1)
    {
        cout << "Starting location was not found.\n";
        return;
    }

    bool visited[MAX_VERTICES] = { false };
    int queue[MAX_VERTICES];
    int front = 0;
    int rear = 0;

    queue[rear++] = startIndex;
    visited[startIndex] = true;

    cout << "BFS order: ";
    bool firstLocation = true;
    while (front < rear)
    {
        int currentIndex = queue[front++];
        if (!firstLocation)
            cout << " -> ";
        cout << locations[currentIndex];
        firstLocation = false;

        RoadEdge* road = adjacency[currentIndex];
        while (road != NULL)
        {
            if (!visited[road->destination])
            {
                visited[road->destination] = true;
                queue[rear++] = road->destination;
            }
            road = road->next;
        }
    }
    cout << endl;
}

void CityGraph::depthFirst(int current, bool visited[]) const
{
    visited[current] = true;
    cout << locations[current];

    RoadEdge* road = adjacency[current];
    while (road != NULL)
    {
        if (!visited[road->destination])
        {
            cout << " -> ";
            depthFirst(road->destination, visited);
        }
        road = road->next;
    }
}

void CityGraph::depthFirstSearch(const string& start) const
{
    int startIndex = findLocation(start);
    if (startIndex == -1)
    {
        cout << "Starting location was not found.\n";
        return;
    }

    bool visited[MAX_VERTICES] = { false };
    cout << "DFS order: ";
    depthFirst(startIndex, visited);
    cout << endl;
}

bool CityGraph::isReachable(const string& source, const string& destination) const
{
    int sourceIndex = findLocation(source);
    int destinationIndex = findLocation(destination);
    if (sourceIndex == -1 || destinationIndex == -1)
        return false;

    bool visited[MAX_VERTICES] = { false };
    int queue[MAX_VERTICES];
    int front = 0;
    int rear = 0;

    queue[rear++] = sourceIndex;
    visited[sourceIndex] = true;

    while (front < rear)
    {
        int current = queue[front++];
        if (current == destinationIndex)
            return true;

        RoadEdge* road = adjacency[current];
        while (road != NULL)
        {
            if (!visited[road->destination])
            {
                visited[road->destination] = true;
                queue[rear++] = road->destination;
            }
            road = road->next;
        }
    }
    return false;
}

void CityGraph::shortestRoute(const string& source, const string& destination) const
{
    int sourceIndex = findLocation(source);
    int destinationIndex = findLocation(destination);
    if (sourceIndex == -1 || destinationIndex == -1)
    {
        cout << "Source or destination was not found in the network.\n";
        return;
    }

    const int INF = 1000000000;
    int distance[MAX_VERTICES];
    int parent[MAX_VERTICES];
    bool visited[MAX_VERTICES] = { false };

    for (int i = 0; i < totalLocations; i++)
    {
        distance[i] = INF;
        parent[i] = -1;
    }
    distance[sourceIndex] = 0;

    // Dijkstra's algorithm using arrays and the adjacency matrix.
    for (int step = 0; step < totalLocations; step++)
    {
        int current = -1;
        int bestDistance = INF;

        for (int i = 0; i < totalLocations; i++)
        {
            if (!visited[i] && distance[i] < bestDistance)
            {
                bestDistance = distance[i];
                current = i;
            }
        }

        if (current == -1)
            break;

        visited[current] = true;
        for (int next = 0; next < totalLocations; next++)
        {
            if (matrix[current][next] > 0 && !visited[next])
            {
                int newDistance = distance[current] + matrix[current][next];
                if (newDistance < distance[next])
                {
                    distance[next] = newDistance;
                    parent[next] = current;
                }
            }
        }
    }

    if (distance[destinationIndex] == INF)
    {
        cout << "No route connects these two locations.\n";
        return;
    }

    int path[MAX_VERTICES];
    int pathLength = 0;
    int current = destinationIndex;
    while (current != -1 && pathLength < MAX_VERTICES)
    {
        path[pathLength++] = current;
        current = parent[current];
    }

    cout << "Recommended route: ";
    for (int i = pathLength - 1; i >= 0; i--)
    {
        cout << locations[path[i]];
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nTotal distance: " << distance[destinationIndex] << " km\n";
}

void CityGraph::minimumSpanningTree() const
{
    if (totalLocations == 0)
    {
        cout << "There are no locations in the network.\n";
        return;
    }

    const int INF = 1000000000;
    int key[MAX_VERTICES];
    int parent[MAX_VERTICES];
    bool inTree[MAX_VERTICES] = { false };

    for (int i = 0; i < totalLocations; i++)
    {
        key[i] = INF;
        parent[i] = -1;
    }
    key[0] = 0;

    for (int step = 0; step < totalLocations; step++)
    {
        int current = -1;
        int smallest = INF;

        for (int i = 0; i < totalLocations; i++)
        {
            if (!inTree[i] && key[i] < smallest)
            {
                smallest = key[i];
                current = i;
            }
        }

        if (current == -1)
        {
            cout << "The network is disconnected, so one spanning tree cannot include every location.\n";
            return;
        }

        inTree[current] = true;
        for (int next = 0; next < totalLocations; next++)
        {
            if (matrix[current][next] > 0 && !inTree[next] && matrix[current][next] < key[next])
            {
                key[next] = matrix[current][next];
                parent[next] = current;
            }
        }
    }

    int totalCost = 0;
    cout << "\nMINIMUM SPANNING TREE (Prim's Algorithm)\n";
    for (int i = 1; i < totalLocations; i++)
    {
        cout << locations[parent[i]] << " -- " << locations[i]
             << " : " << matrix[parent[i]][i] << " km\n";
        totalCost += matrix[parent[i]][i];
    }
    cout << "Total network cost: " << totalCost << " km\n";
}

int CityGraph::getLocationCount() const
{
    return totalLocations;
}
