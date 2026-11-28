#include "RouteWise.h"
#include <cstdlib>

Customer::Customer()
{
    customerId = 0;
    next = NULL;
}

Parcel::Parcel()
{
    trackingId = 0;
    customerId = 0;
    weight = 0;
    priority = 1;
    status = "Registered";
    requestType = 0;
    arrivalOrder = 0;
    next = NULL;
}

void displayParcel(const Parcel* parcel)
{
    if (parcel == NULL)
        return;

    cout << "Tracking ID : " << parcel->trackingId << endl;
    cout << "Customer ID : " << parcel->customerId << endl;
    cout << "Route       : " << parcel->source << " -> " << parcel->destination << endl;
    cout << "Weight      : " << parcel->weight << " kg" << endl;
    cout << "Priority    : " << parcel->priority << endl;
    cout << "Status      : " << parcel->status << endl;
}

CustomerList::CustomerList()
{
    head = NULL;
    tail = NULL;
    total = 0;
}

CustomerList::~CustomerList()
{
    Customer* current = head;
    while (current != NULL)
    {
        Customer* nextCustomer = current->next;
        delete current;
        current = nextCustomer;
    }
}

bool CustomerList::addCustomer(int id, const string& name, const string& phone, const string& address)
{
    if (id <= 0 || name.empty() || searchCustomer(id) != NULL)
        return false;

    Customer* newCustomer = new Customer;
    newCustomer->customerId = id;
    newCustomer->name = name;
    newCustomer->phone = phone;
    newCustomer->address = address;

    if (head == NULL)
        head = tail = newCustomer;
    else
    {
        tail->next = newCustomer;
        tail = newCustomer;
    }

    total++;
    return true;
}

Customer* CustomerList::searchCustomer(int id) const
{
    Customer* current = head;
    while (current != NULL)
    {
        if (current->customerId == id)
            return current;
        current = current->next;
    }
    return NULL;
}

void CustomerList::displayCustomers() const
{
    if (head == NULL)
    {
        cout << "No customer records are available.\n";
        return;
    }

    cout << "\nCUSTOMER RECORDS\n";
    cout << "------------------------------------------------------------\n";
    Customer* current = head;
    while (current != NULL)
    {
        cout << "ID: " << current->customerId
             << " | Name: " << current->name
             << " | Phone: " << current->phone
             << " | Address: " << current->address << endl;
        current = current->next;
    }
}

int CustomerList::getCount() const
{
    return total;
}

bool CustomerList::loadFromFile(const string& fileName)
{
    ifstream file(fileName.c_str());
    if (!file)
        return false;

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        string idText, name, phone, address;
        stringstream row(line);
        getline(row, idText, '|');
        getline(row, name, '|');
        getline(row, phone, '|');
        getline(row, address);

        int id = atoi(idText.c_str());
        if (id > 0 && searchCustomer(id) == NULL)
            addCustomer(id, name, phone, address);
    }

    return true;
}

bool CustomerList::saveToFile(const string& fileName) const
{
    ofstream file(fileName.c_str());
    if (!file)
        return false;

    Customer* current = head;
    while (current != NULL)
    {
        file << current->customerId << '|'
             << current->name << '|'
             << current->phone << '|'
             << current->address << endl;
        current = current->next;
    }
    return true;
}

ParcelHashTable::ParcelHashTable()
{
    capacity = HASH_SIZE;
    total = 0;
    slots = new Parcel*[capacity];
    for (int i = 0; i < capacity; i++)
        slots[i] = NULL;
}

ParcelHashTable::~ParcelHashTable()
{
    delete[] slots;
}

int ParcelHashTable::hashFunction(int trackingId) const
{
    if (trackingId < 0)
        trackingId = -trackingId;
    return trackingId % capacity;
}

void ParcelHashTable::grow()
{
    int oldCapacity = capacity;
    Parcel** oldSlots = slots;

    capacity = capacity * 2 + 1;
    slots = new Parcel*[capacity];
    for (int i = 0; i < capacity; i++)
        slots[i] = NULL;

    total = 0;
    for (int i = 0; i < oldCapacity; i++)
    {
        if (oldSlots[i] != NULL)
        {
            int start = hashFunction(oldSlots[i]->trackingId);
            for (int step = 0; step < capacity; step++)
            {
                int index = (start + step) % capacity;
                if (slots[index] == NULL)
                {
                    slots[index] = oldSlots[i];
                    total++;
                    break;
                }
            }
        }
    }

    delete[] oldSlots;
}

bool ParcelHashTable::insert(Parcel* parcel)
{
    if (parcel == NULL || search(parcel->trackingId) != NULL)
        return false;

    if ((total + 1) * 10 >= capacity * 7)
        grow();

    int start = hashFunction(parcel->trackingId);
    for (int step = 0; step < capacity; step++)
    {
        int index = (start + step) % capacity;
        if (slots[index] == NULL)
        {
            slots[index] = parcel;
            total++;
            return true;
        }
    }
    return false;
}

Parcel* ParcelHashTable::search(int trackingId) const
{
    int start = hashFunction(trackingId);
    for (int step = 0; step < capacity; step++)
    {
        int index = (start + step) % capacity;
        if (slots[index] == NULL)
            return NULL;
        if (slots[index]->trackingId == trackingId)
            return slots[index];
    }
    return NULL;
}

int ParcelHashTable::getCount() const
{
    return total;
}

ParcelList::ParcelList()
{
    head = NULL;
    tail = NULL;
    total = 0;
}

ParcelList::~ParcelList()
{
    Parcel* current = head;
    while (current != NULL)
    {
        Parcel* nextParcel = current->next;
        delete current;
        current = nextParcel;
    }
}

bool ParcelList::addParcel(Parcel* parcel)
{
    if (parcel == NULL || parcel->trackingId <= 0)
        return false;

    parcel->next = NULL;
    if (head == NULL)
        head = tail = parcel;
    else
    {
        tail->next = parcel;
        tail = parcel;
    }

    total++;
    return true;
}

Parcel* ParcelList::getHead() const
{
    return head;
}

int ParcelList::getCount() const
{
    return total;
}

void ParcelList::displayParcels() const
{
    if (head == NULL)
    {
        cout << "No parcel records are available.\n";
        return;
    }

    cout << "\nPARCEL RECORDS\n";
    cout << "------------------------------------------------------------\n";
    Parcel* current = head;
    while (current != NULL)
    {
        displayParcel(current);
        cout << "------------------------------------------------------------\n";
        current = current->next;
    }
}

bool ParcelList::loadFromFile(const string& fileName)
{
    ifstream file(fileName.c_str());
    if (!file)
        return false;

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        string fields[9];
        stringstream row(line);
        for (int i = 0; i < 8; i++)
            getline(row, fields[i], '|');
        getline(row, fields[8]);

        Parcel* parcel = new Parcel;
        parcel->trackingId = atoi(fields[0].c_str());
        parcel->customerId = atoi(fields[1].c_str());
        parcel->source = fields[2];
        parcel->destination = fields[3];
        parcel->weight = static_cast<float>(atof(fields[4].c_str()));
        parcel->priority = atoi(fields[5].c_str());
        parcel->status = fields[6].empty() ? "Registered" : fields[6];
        parcel->requestType = atoi(fields[7].c_str());
        parcel->arrivalOrder = atol(fields[8].c_str());

        if (parcel->trackingId > 0)
            addParcel(parcel);
        else
            delete parcel;
    }

    return true;
}

bool ParcelList::saveToFile(const string& fileName) const
{
    ofstream file(fileName.c_str());
    if (!file)
        return false;

    Parcel* current = head;
    while (current != NULL)
    {
        file << current->trackingId << '|'
             << current->customerId << '|'
             << current->source << '|'
             << current->destination << '|'
             << current->weight << '|'
             << current->priority << '|'
             << current->status << '|'
             << current->requestType << '|'
             << current->arrivalOrder << endl;
        current = current->next;
    }
    return true;
}
