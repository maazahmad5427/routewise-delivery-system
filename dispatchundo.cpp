#include "RouteWise.h"

DeliveryQueue::DeliveryQueue()
{
    capacity = 10;
    items = new Parcel*[capacity];
    frontIndex = 0;
    rearIndex = -1;
    total = 0;
}

DeliveryQueue::~DeliveryQueue()
{
    delete[] items;
}

void DeliveryQueue::grow()
{
    int newCapacity = capacity * 2;
    Parcel** bigger = new Parcel*[newCapacity];

    for (int i = 0; i < total; i++)
        bigger[i] = items[(frontIndex + i) % capacity];

    delete[] items;
    items = bigger;
    capacity = newCapacity;
    frontIndex = 0;
    rearIndex = total - 1;
}

bool DeliveryQueue::enqueue(Parcel* parcel)
{
    if (parcel == NULL)
        return false;

    if (total == capacity)
        grow();

    rearIndex = (rearIndex + 1) % capacity;
    items[rearIndex] = parcel;
    total++;
    return true;
}

bool DeliveryQueue::enqueueFront(Parcel* parcel)
{
    if (parcel == NULL)
        return false;

    if (total == capacity)
        grow();

    frontIndex = (frontIndex - 1 + capacity) % capacity;
    items[frontIndex] = parcel;
    if (total == 0)
        rearIndex = frontIndex;
    total++;
    return true;
}

Parcel* DeliveryQueue::dequeue()
{
    if (total == 0)
        return NULL;

    Parcel* parcel = items[frontIndex];
    frontIndex = (frontIndex + 1) % capacity;
    total--;

    if (total == 0)
    {
        frontIndex = 0;
        rearIndex = -1;
    }
    return parcel;
}

bool DeliveryQueue::isEmpty() const
{
    return total == 0;
}

int DeliveryQueue::getCount() const
{
    return total;
}

void DeliveryQueue::display() const
{
    if (total == 0)
    {
        cout << "No normal deliveries are waiting.\n";
        return;
    }

    cout << "Normal deliveries (first in, first out):\n";
    for (int i = 0; i < total; i++)
    {
        Parcel* parcel = items[(frontIndex + i) % capacity];
        cout << i + 1 << ". Tracking ID " << parcel->trackingId
             << " | " << parcel->source << " -> " << parcel->destination << endl;
    }
}

UrgentHeap::UrgentHeap()
{
    capacity = 10;
    items = new DeliveryItem[capacity];
    total = 0;
}

UrgentHeap::~UrgentHeap()
{
    delete[] items;
}

void UrgentHeap::grow()
{
    int newCapacity = capacity * 2;
    DeliveryItem* bigger = new DeliveryItem[newCapacity];
    for (int i = 0; i < total; i++)
        bigger[i] = items[i];

    delete[] items;
    items = bigger;
    capacity = newCapacity;
}

bool UrgentHeap::hasHigherPriority(const DeliveryItem& first, const DeliveryItem& second) const
{
    if (first.parcel->priority != second.parcel->priority)
        return first.parcel->priority > second.parcel->priority;

    return first.arrivalOrder < second.arrivalOrder;
}

void UrgentHeap::moveUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (!hasHigherPriority(items[index], items[parent]))
            break;

        DeliveryItem temp = items[index];
        items[index] = items[parent];
        items[parent] = temp;
        index = parent;
    }
}

void UrgentHeap::moveDown(int index)
{
    while (true)
    {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int best = index;

        if (left < total && hasHigherPriority(items[left], items[best]))
            best = left;
        if (right < total && hasHigherPriority(items[right], items[best]))
            best = right;

        if (best == index)
            break;

        DeliveryItem temp = items[index];
        items[index] = items[best];
        items[best] = temp;
        index = best;
    }
}

bool UrgentHeap::enqueue(Parcel* parcel)
{
    if (parcel == NULL)
        return false;
    return enqueueWithOrder(parcel, parcel->arrivalOrder);
}

bool UrgentHeap::enqueueWithOrder(Parcel* parcel, long arrivalOrder)
{
    if (parcel == NULL)
        return false;

    if (total == capacity)
        grow();

    items[total].parcel = parcel;
    items[total].arrivalOrder = arrivalOrder;
    moveUp(total);
    total++;
    return true;
}

Parcel* UrgentHeap::dequeue(long& arrivalOrder)
{
    if (total == 0)
        return NULL;

    Parcel* parcel = items[0].parcel;
    arrivalOrder = items[0].arrivalOrder;
    total--;

    if (total > 0)
    {
        items[0] = items[total];
        moveDown(0);
    }
    return parcel;
}

bool UrgentHeap::isEmpty() const
{
    return total == 0;
}

int UrgentHeap::getCount() const
{
    return total;
}

void UrgentHeap::display() const
{
    if (total == 0)
    {
        cout << "No urgent deliveries are waiting.\n";
        return;
    }

    cout << "Urgent deliveries (highest priority first):\n";
    for (int i = 0; i < total; i++)
    {
        cout << i + 1 << ". Tracking ID " << items[i].parcel->trackingId
             << " | Priority " << items[i].parcel->priority
             << " | " << items[i].parcel->source << " -> "
             << items[i].parcel->destination << endl;
    }
}

DeliveryHistory::DeliveryHistory()
{
    head = NULL;
    tail = NULL;
    total = 0;
}

DeliveryHistory::~DeliveryHistory()
{
    HistoryNode* current = head;
    while (current != NULL)
    {
        HistoryNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

void DeliveryHistory::add(Parcel* parcel)
{
    if (parcel == NULL)
        return;

    HistoryNode* newNode = new HistoryNode;
    newNode->parcel = parcel;
    newNode->previous = tail;
    newNode->next = NULL;

    if (head == NULL)
        head = tail = newNode;
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
    total++;
}

bool DeliveryHistory::removeByTrackingId(int trackingId)
{
    HistoryNode* current = head;
    while (current != NULL)
    {
        if (current->parcel->trackingId == trackingId)
        {
            if (current->previous != NULL)
                current->previous->next = current->next;
            else
                head = current->next;

            if (current->next != NULL)
                current->next->previous = current->previous;
            else
                tail = current->previous;

            delete current;
            total--;
            return true;
        }
        current = current->next;
    }
    return false;
}

void DeliveryHistory::display() const
{
    if (head == NULL)
    {
        cout << "Delivery history is empty.\n";
        return;
    }

    cout << "\nCOMPLETED DELIVERY HISTORY\n";
    HistoryNode* current = head;
    while (current != NULL)
    {
        cout << "Tracking ID " << current->parcel->trackingId
             << " | " << current->parcel->source << " -> "
             << current->parcel->destination << " | Delivered" << endl;
        current = current->next;
    }
}

int DeliveryHistory::getCount() const
{
    return total;
}

UndoStack::UndoStack()
{
    topNode = NULL;
    total = 0;
}

UndoStack::~UndoStack()
{
    UndoNode* current = topNode;
    while (current != NULL)
    {
        UndoNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

void UndoStack::push(const UndoRecord& record)
{
    UndoNode* newNode = new UndoNode;
    newNode->record = record;
    newNode->next = topNode;
    topNode = newNode;
    total++;
}

bool UndoStack::pop(UndoRecord& record)
{
    if (topNode == NULL)
        return false;

    UndoNode* oldTop = topNode;
    record = oldTop->record;
    topNode = topNode->next;
    delete oldTop;
    total--;
    return true;
}

bool UndoStack::isEmpty() const
{
    return topNode == NULL;
}

int UndoStack::getCount() const
{
    return total;
}
