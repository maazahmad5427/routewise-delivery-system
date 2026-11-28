#include "RouteWise.h"

int readInt(const string& prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            cin.ignore(10000, '\n');
            return value;
        }

        cout << "Please enter a valid whole number.\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

float readFloat(const string& prompt)
{
    float value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            cin.ignore(10000, '\n');
            return value;
        }

        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

string readText(const string& prompt)
{
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void showMenu()
{
    cout << "\n======================================================\n";
    cout << "              ROUTEWISE DELIVERY SYSTEM\n";
    cout << "======================================================\n";
    cout << " 1. Register Customer\n";
    cout << " 2. Register Parcel\n";
    cout << " 3. Search Parcel\n";
    cout << " 4. Add Normal Delivery Request\n";
    cout << " 5. Add Urgent Delivery Request\n";
    cout << " 6. Process Next Delivery\n";
    cout << " 7. Mark Parcel as Delivered\n";
    cout << " 8. Undo Last Operation\n";
    cout << " 9. Display Pending Deliveries\n";
    cout << "10. Display Parcel Records\n";
    cout << "11. Display City Network\n";
    cout << "12. BFS Network Traversal\n";
    cout << "13. DFS Network Traversal\n";
    cout << "14. Find Shortest Delivery Route\n";
    cout << "15. Generate Minimum Delivery Network\n";
    cout << "16. Compare BST and AVL Performance\n";
    cout << "17. Display System Statistics\n";
    cout << "18. Display Customers and Delivery History\n";
    cout << "19. Add City Location\n";
    cout << "20. Add Road Between Locations\n";
    cout << "21. Save and Exit\n";
    cout << "======================================================\n";
}

void rebuildIndexesAndQueues(ParcelList& parcels, ParcelHashTable& hashTable,
    ParcelBST& bst, ParcelAVL& avl,
    DeliveryQueue& normalQueue, UrgentHeap& urgentHeap,
    DeliveryHistory& history, long& nextArrivalOrder)
{
    Parcel* current = parcels.getHead();
    while (current != NULL)
    {
        hashTable.insert(current);
        bst.insert(current);
        avl.insert(current);

        if (current->arrivalOrder >= nextArrivalOrder)
            nextArrivalOrder = current->arrivalOrder + 1;

        if (current->status == "Delivered")
            history.add(current);
        else if (current->status == "Pending" && current->requestType == 2)
            urgentHeap.enqueueWithOrder(current, current->arrivalOrder);

        current = current->next;
    }

    // Restore normal requests in their original arrival order.
    long lastOrder = -1;
    while (true)
    {
        Parcel* selected = NULL;
        current = parcels.getHead();
        while (current != NULL)
        {
            if (current->status == "Pending" && current->requestType == 1 &&
                current->arrivalOrder > lastOrder &&
                (selected == NULL || current->arrivalOrder < selected->arrivalOrder))
                selected = current;
            current = current->next;
        }

        if (selected == NULL)
            break;

        normalQueue.enqueue(selected);
        lastOrder = selected->arrivalOrder;
    }
}

int main()
{
    CustomerList customers;
    ParcelList parcels;
    ParcelHashTable hashTable;
    DeliveryQueue normalQueue;
    UrgentHeap urgentHeap;
    DeliveryHistory history;
    UndoStack undoStack;
    ParcelBST bst;
    ParcelAVL avl;
    CityGraph city;

    long nextArrivalOrder = 1;

    customers.loadFromFile("data/customers.txt");
    parcels.loadFromFile("data/parcels.txt");
    city.loadFromFile("data/network.txt");

    rebuildIndexesAndQueues(parcels, hashTable, bst, avl,
        normalQueue, urgentHeap, history, nextArrivalOrder);

    cout << "\nWelcome to RouteWise. The delivery workspace is ready.\n";
    cout << "Loaded customers: " << customers.getCount()
        << " | Loaded parcels: " << parcels.getCount()
        << " | City locations: " << city.getLocationCount() << endl;

    bool running = true;
    while (running)
    {
        showMenu();
        int choice = readInt("Select an option: ");

        if (choice == 1)
        {
            int id = readInt("Enter customer ID: ");
            string name = readText("Enter customer name: ");
            string phone = readText("Enter phone number: ");
            string address = readText("Enter address: ");

            if (customers.addCustomer(id, name, phone, address))
                cout << "Customer registered successfully.\n";
            else
                cout << "Customer could not be added. Check the ID and required fields.\n";
        }
        else if (choice == 2)
        {
            int trackingId = readInt("Enter a new tracking ID: ");
            if (trackingId <= 0 || hashTable.search(trackingId) != NULL)
            {
                cout << "This tracking ID is invalid or already in use.\n";
                continue;
            }

            int customerId = readInt("Enter customer ID: ");
            if (customers.searchCustomer(customerId) == NULL)
            {
                cout << "Customer was not found. Register the customer first.\n";
                continue;
            }

            string source = readText("Enter parcel source: ");
            string destination = readText("Enter parcel destination: ");
            float weight = readFloat("Enter parcel weight in kg: ");
            int priority = readInt("Enter priority (1 = low, 5 = highest): ");

            if (source.empty() || destination.empty() || weight <= 0 || priority < 1 || priority > 5)
            {
                cout << "Parcel details are not valid. No record was created.\n";
                continue;
            }

            Parcel* parcel = new Parcel;
            parcel->trackingId = trackingId;
            parcel->customerId = customerId;
            parcel->source = source;
            parcel->destination = destination;
            parcel->weight = weight;
            parcel->priority = priority;

            if (parcels.addParcel(parcel))
            {
                hashTable.insert(parcel);
                bst.insert(parcel);
                avl.insert(parcel);
                cout << "Parcel registered successfully. Tracking ID: " << trackingId << endl;
            }
            else
            {
                delete parcel;
                cout << "Parcel could not be added.\n";
            }
        }
        else if (choice == 3)
        {
            int trackingId = readInt("Enter tracking ID to search: ");
            Parcel* parcel = hashTable.search(trackingId);
            if (parcel == NULL)
                cout << "No parcel was found with that tracking ID.\n";
            else
                displayParcel(parcel);
        }
        else if (choice == 4)
        {
            int trackingId = readInt("Enter tracking ID for normal delivery: ");
            Parcel* parcel = hashTable.search(trackingId);

            if (parcel == NULL)
                cout << "Parcel was not found.\n";
            else if (parcel->status != "Registered")
                cout << "Only a newly registered parcel can be added to the delivery queue.\n";
            else
            {
                parcel->status = "Pending";
                parcel->requestType = 1;
                parcel->arrivalOrder = nextArrivalOrder++;
                normalQueue.enqueue(parcel);
                cout << "Normal delivery request added to the queue.\n";
            }
        }
        else if (choice == 5)
        {
            int trackingId = readInt("Enter tracking ID for urgent delivery: ");
            Parcel* parcel = hashTable.search(trackingId);

            if (parcel == NULL)
                cout << "Parcel was not found.\n";
            else if (parcel->status != "Registered")
                cout << "Only a newly registered parcel can be added to urgent dispatch.\n";
            else
            {
                int priority = readInt("Set urgent priority (1 = low, 5 = highest): ");
                if (priority < 1 || priority > 5)
                {
                    cout << "Priority must be between 1 and 5.\n";
                    continue;
                }

                parcel->priority = priority;
                parcel->status = "Pending";
                parcel->requestType = 2;
                parcel->arrivalOrder = nextArrivalOrder++;
                urgentHeap.enqueueWithOrder(parcel, parcel->arrivalOrder);
                cout << "Urgent delivery added to the priority heap.\n";
            }
        }
        else if (choice == 6)
        {
            Parcel* parcel = NULL;
            int requestType = 0;
            long arrivalOrder = 0;

            // Urgent requests are selected before normal requests.
            if (!urgentHeap.isEmpty())
            {
                parcel = urgentHeap.dequeue(arrivalOrder);
                requestType = 2;
            }
            else if (!normalQueue.isEmpty())
            {
                parcel = normalQueue.dequeue();
                if (parcel != NULL)
                    arrivalOrder = parcel->arrivalOrder;
                requestType = 1;
            }

            if (parcel == NULL)
                cout << "There are no pending deliveries to process.\n";
            else
            {
                UndoRecord record;
                record.parcel = parcel;
                record.previousStatus = parcel->status;
                record.operation = 1;
                record.requestType = requestType;
                record.arrivalOrder = arrivalOrder;
                undoStack.push(record);

                parcel->status = "In Transit";
                cout << "Delivery processed. Parcel " << parcel->trackingId
                    << " is now In Transit.\n";
            }
        }
        else if (choice == 7)
        {
            int trackingId = readInt("Enter tracking ID to mark as delivered: ");
            Parcel* parcel = hashTable.search(trackingId);

            if (parcel == NULL)
                cout << "Parcel was not found.\n";
            else if (parcel->status != "In Transit")
                cout << "A parcel must be In Transit before it can be marked Delivered.\n";
            else
            {
                UndoRecord record;
                record.parcel = parcel;
                record.previousStatus = parcel->status;
                record.operation = 2;
                record.requestType = parcel->requestType;
                record.arrivalOrder = parcel->arrivalOrder;
                undoStack.push(record);

                parcel->status = "Delivered";
                history.add(parcel);
                cout << "Parcel marked as delivered.\n";
            }
        }
        else if (choice == 8)
        {
            UndoRecord record;
            if (!undoStack.pop(record))
                cout << "There is no operation available to undo.\n";
            else if (record.operation == 1)
            {
                record.parcel->status = record.previousStatus;
                if (record.requestType == 2)
                    urgentHeap.enqueueWithOrder(record.parcel, record.arrivalOrder);
                else
                    normalQueue.enqueueFront(record.parcel);

                cout << "Last dispatch operation was undone. Parcel "
                    << record.parcel->trackingId << " is Pending again.\n";
            }
            else if (record.operation == 2)
            {
                history.removeByTrackingId(record.parcel->trackingId);
                record.parcel->status = record.previousStatus;
                cout << "Delivery status was restored to In Transit.\n";
            }
        }
        else if (choice == 9)
        {
            cout << "\nPENDING DELIVERY REQUESTS\n";
            urgentHeap.display();
            normalQueue.display();
        }
        else if (choice == 10)
        {
            parcels.displayParcels();
        }
        else if (choice == 11)
        {
            city.displayNetwork();
        }
        else if (choice == 12)
        {
            string start = readText("Enter starting location: ");
            string destination = readText("Enter destination to check reachability: ");
            city.breadthFirstSearch(start);
            if (city.isReachable(start, destination))
                cout << destination << " is reachable from " << start << ".\n";
            else
                cout << destination << " is not reachable from " << start << ".\n";
        }
        else if (choice == 13)
        {
            string start = readText("Enter starting location: ");
            city.depthFirstSearch(start);
        }
        else if (choice == 14)
        {
            string source = readText("Enter source location: ");
            string destination = readText("Enter destination location: ");
            city.shortestRoute(source, destination);
        }
        else if (choice == 15)
        {
            city.minimumSpanningTree();
        }
        else if (choice == 16)
        {
            cout << "\nPARCEL SEARCH TREE COMPARISON\n";
            cout << "Number of indexed parcels: " << parcels.getCount() << endl;
            cout << "BST height : " << bst.height() << endl;
            cout << "AVL height : " << avl.height() << endl;
            bst.displayInOrder();
            avl.displayInOrder();
            cout << "The AVL tree rotates after insertion to keep its height balanced.\n";
        }
        else if (choice == 17)
        {
            cout << "\nROUTEWISE SYSTEM STATISTICS\n";
            cout << "Customers                 : " << customers.getCount() << endl;
            cout << "Registered parcels        : " << parcels.getCount() << endl;
            cout << "Hash table records        : " << hashTable.getCount() << endl;
            cout << "Urgent pending deliveries : " << urgentHeap.getCount() << endl;
            cout << "Normal pending deliveries : " << normalQueue.getCount() << endl;
            cout << "Completed deliveries      : " << history.getCount() << endl;
            cout << "Undoable operations       : " << undoStack.getCount() << endl;
            cout << "City locations            : " << city.getLocationCount() << endl;
        }
        else if (choice == 18)
        {
            customers.displayCustomers();
            history.display();
        }
        else if (choice == 19)
        {
            string name = readText("Enter the new city location name: ");
            if (city.addLocation(name))
                cout << "Location added successfully.\n";
            else
                cout << "Location could not be added. Check for a duplicate or the location limit.\n";
        }
        else if (choice == 20)
        {
            string first = readText("Enter first location name: ");
            string second = readText("Enter second location name: ");
            int distance = readInt("Enter road distance in km: ");

            if (city.addRoad(first, second, distance))
                cout << "Road added successfully.\n";
            else
                cout << "Road could not be added. Check both names and enter a positive distance.\n";
        }
        else if (choice == 21)
        {
            running = false;
        }
        else
        {
            cout << "That menu option does not exist. Please try again.\n";
        }
    }

    bool customersSaved = customers.saveToFile("data/customers.txt");
    bool parcelsSaved = parcels.saveToFile("data/parcels.txt");
    bool networkSaved = city.saveToFile("data/network.txt");

    cout << "\nRouteWise is closing. ";
    if (customersSaved && parcelsSaved && networkSaved)
        cout << "Customer, parcel, and city-network records have been saved.\n";
    else
        cout << "Some records could not be saved. Check the data folder.\n";

    return 0;
}
