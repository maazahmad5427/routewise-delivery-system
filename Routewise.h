#ifndef ROUTEWISE_H
#define ROUTEWISE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const int HASH_SIZE = 211;
const int MAX_VERTICES = 30;

struct Customer
{
    int customerId;
    string name;
    string phone;
    string address;
    Customer* next;

    Customer();
};

struct Parcel
{
    int trackingId;
    int customerId;
    string source;
    string destination;
    float weight;
    int priority;
    string status;
    int requestType;       // 0 = no request, 1 = normal, 2 = urgent
    long arrivalOrder;
    Parcel* next;

    Parcel();
};

void displayParcel(const Parcel* parcel);

// Member 1: customer records, parcel records, and tracking-ID hashing.
class CustomerList
{
private:
    Customer* head;
    Customer* tail;
    int total;

public:
    CustomerList();
    ~CustomerList();

    bool addCustomer(int id, const string& name, const string& phone, const string& address);
    Customer* searchCustomer(int id) const;
    void displayCustomers() const;
    int getCount() const;
    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
};

class ParcelHashTable
{
private:
    Parcel** slots;
    int capacity;
    int total;
    int hashFunction(int trackingId) const;
    void grow();

public:
    ParcelHashTable();
    ~ParcelHashTable();
    bool insert(Parcel* parcel);
    Parcel* search(int trackingId) const;
    int getCount() const;
};

class ParcelList
{
private:
    Parcel* head;
    Parcel* tail;
    int total;

public:
    ParcelList();
    ~ParcelList();

    bool addParcel(Parcel* parcel);
    Parcel* getHead() const;
    int getCount() const;
    void displayParcels() const;
    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
};

// Member 2: normal queue, urgent heap, delivery history, and undo stack.
struct DeliveryItem
{
    Parcel* parcel;
    long arrivalOrder;
};

class DeliveryQueue
{
private:
    Parcel** items;
    int capacity;
    int frontIndex;
    int rearIndex;
    int total;
    void grow();

public:
    DeliveryQueue();
    ~DeliveryQueue();

    bool enqueue(Parcel* parcel);
    bool enqueueFront(Parcel* parcel);
    Parcel* dequeue();
    bool isEmpty() const;
    int getCount() const;
    void display() const;
};

class UrgentHeap
{
private:
    DeliveryItem* items;
    int capacity;
    int total;
    void grow();
    bool hasHigherPriority(const DeliveryItem& first, const DeliveryItem& second) const;
    void moveUp(int index);
    void moveDown(int index);

public:
    UrgentHeap();
    ~UrgentHeap();

    bool enqueue(Parcel* parcel);
    bool enqueueWithOrder(Parcel* parcel, long arrivalOrder);
    Parcel* dequeue(long& arrivalOrder);
    bool isEmpty() const;
    int getCount() const;
    void display() const;
};

struct HistoryNode
{
    Parcel* parcel;
    HistoryNode* previous;
    HistoryNode* next;
};

class DeliveryHistory
{
private:
    HistoryNode* head;
    HistoryNode* tail;
    int total;

public:
    DeliveryHistory();
    ~DeliveryHistory();

    void add(Parcel* parcel);
    bool removeByTrackingId(int trackingId);
    void display() const;
    int getCount() const;
};

struct UndoRecord
{
    Parcel* parcel;
    string previousStatus;
    int operation;          // 1 = delivery was processed, 2 = parcel was delivered
    int requestType;
    long arrivalOrder;
};

struct UndoNode
{
    UndoRecord record;
    UndoNode* next;
};

class UndoStack
{
private:
    UndoNode* topNode;
    int total;

public:
    UndoStack();
    ~UndoStack();

    void push(const UndoRecord& record);
    bool pop(UndoRecord& record);
    bool isEmpty() const;
    int getCount() const;
};

// Member 3: ordered parcel searching with a BST and an AVL tree.
struct TreeNode
{
    Parcel* parcel;
    TreeNode* left;
    TreeNode* right;
    int height;

    TreeNode(Parcel* item);
};

class ParcelBST
{
private:
    TreeNode* root;
    TreeNode* insertNode(TreeNode* node, Parcel* parcel);
    Parcel* searchNode(TreeNode* node, int trackingId) const;
    void inOrder(TreeNode* node) const;
    void destroy(TreeNode* node);
    int getHeight(TreeNode* node) const;

public:
    ParcelBST();
    ~ParcelBST();

    void insert(Parcel* parcel);
    Parcel* search(int trackingId) const;
    void displayInOrder() const;
    int height() const;
};

class ParcelAVL
{
private:
    TreeNode* root;
    int nodeHeight(TreeNode* node) const;
    int balanceFactor(TreeNode* node) const;
    TreeNode* rotateRight(TreeNode* top);
    TreeNode* rotateLeft(TreeNode* top);
    TreeNode* insertNode(TreeNode* node, Parcel* parcel);
    Parcel* searchNode(TreeNode* node, int trackingId) const;
    void inOrder(TreeNode* node) const;
    void destroy(TreeNode* node);

public:
    ParcelAVL();
    ~ParcelAVL();

    void insert(Parcel* parcel);
    Parcel* search(int trackingId) const;
    void displayInOrder() const;
    int height() const;
};

// Member 4: city graph, traversals, shortest route, and minimum spanning tree.
struct RoadEdge
{
    int destination;
    int distance;
    RoadEdge* next;
};

class CityGraph
{
private:
    string locations[MAX_VERTICES];
    RoadEdge* adjacency[MAX_VERTICES];
    int matrix[MAX_VERTICES][MAX_VERTICES];
    int totalLocations;

    int findLocation(const string& name) const;
    void addRoadByIndex(int first, int second, int distance);
    void depthFirst(int current, bool visited[]) const;

public:
    CityGraph();
    ~CityGraph();

    bool addLocation(const string& name);
    bool addRoad(const string& first, const string& second, int distance);
    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    void displayNetwork() const;
    void breadthFirstSearch(const string& start) const;
    void depthFirstSearch(const string& start) const;
    bool isReachable(const string& source, const string& destination) const;
    void shortestRoute(const string& source, const string& destination) const;
    void minimumSpanningTree() const;
    int getLocationCount() const;
};

#endif

