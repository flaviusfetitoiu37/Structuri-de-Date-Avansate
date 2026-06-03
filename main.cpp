#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#ifdef PROJECT_ROOT
const string TASKS_FILENAME = string(PROJECT_ROOT) + "/tasks.txt";
#else
const string TASKS_FILENAME = "tasks.txt";
#endif

struct Task {
    int id;
    string title;
    int priority;
};

struct HashNode {
    int key;
    Task value;
    HashNode* next;
    HashNode(int k, Task v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

class HashTable {
private:
    HashNode** table;
    int bucketCount;

    int hashFunction(int key) {
        return key % bucketCount;
    }

public:
    HashTable(int buckets) {
        bucketCount = buckets;
        table = new HashNode*[bucketCount];
        for (int i = 0; i < bucketCount; i++) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        for (int i = 0; i < bucketCount; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

    void insert(int key, Task value) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }
        HashNode* newNode = new HashNode(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }

    Task search(int key) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        while (current != nullptr) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }
        return {-1, "", -1};
    }

    void remove(int key) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        HashNode* prev = nullptr;
        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    table[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    void display() {
        for (int i = 0; i < bucketCount; i++) {
            HashNode* current = table[i];
            if (current != nullptr) {
                cout << "Bucket " << i << ": ";
                while (current != nullptr) {
                    cout << "[" << current->key << ": " << current->value.title << "] -> ";
                    current = current->next;
                }
                cout << "NULL\n";
            }
        }
    }
};

class MinHeap {
private:
    Task* arr;
    int capacity;
    int size;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (arr[index].priority < arr[parent].priority) {
                swap(arr[index], arr[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && arr[left].priority < arr[smallest].priority) {
            smallest = left;
        }
        if (right < size && arr[right].priority < arr[smallest].priority) {
            smallest = right;
        }
        if (smallest != index) {
            swap(arr[index], arr[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap(int cap) {
        capacity = cap;
        size = 0;
        arr = new Task[capacity];
    }

    ~MinHeap() {
        delete[] arr;
    }

    void insert(Task task) {
        if (size == capacity) return;
        arr[size] = task;
        size++;
        heapifyUp(size - 1);
    }

    Task extractMin() {
        if (size <= 0) return {-1, "", -1};
        if (size == 1) {
            size--;
            return arr[0];
        }
        Task root = arr[0];
        arr[0] = arr[size - 1];
        size--;
        heapifyDown(0);
        return root;
    }

    bool isEmpty() {
        return size == 0;
    }

    void display() {
        for (int i = 0; i < size; i++) {
            cout << "ID: " << arr[i].id << ", Title: " << arr[i].title
                 << ", Priority: " << arr[i].priority << "\n";
        }
    }

    bool saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Eroare: nu pot scrie in " << filename << "!\n";
            return false;
        }
        for (int i = 0; i < size; i++) {
            file << arr[i].id << "\n" << arr[i].title << "\n" << arr[i].priority << "\n";
        }
        file.close();
        return true;
    }
};

void ensureTasksFileExists(const string& filename) {
    ifstream check(filename);
    if (!check.is_open()) {
        ofstream create(filename);
        create.close();
        cout << "Fisier creat: " << filename << "\n";
    } else {
        check.close();
    }
}

bool loadTasksFromFile(const string& filename, MinHeap& heap, HashTable& hashTable, int& idCounter) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int maxId = 0;
    int id, priority;
    string title;

    while (file >> id) {
        file.ignore();
        getline(file, title);
        file >> priority;
        Task t = {id, title, priority};
        heap.insert(t);
        hashTable.insert(id, t);
        if (id > maxId) {
            maxId = id;
        }
    }

    file.close();
    if (maxId > 0) {
        idCounter = maxId + 1;
    }
    return maxId > 0;
}

int main() {
    MinHeap heap(100);
    HashTable hashTable(10);
    int choice;
    int idCounter = 1;

    ensureTasksFileExists(TASKS_FILENAME);

    if (loadTasksFromFile(TASKS_FILENAME, heap, hashTable, idCounter)) {
        cout << "Sarcini incarcate din " << TASKS_FILENAME << "\n\n";
    } else {
        cout << "Fisier gol sau fara sarcini. Salveaza in: " << TASKS_FILENAME << "\n\n";
    }

    while (true) {
        cout << "1. Add Task\n";
        cout << "2. Extract Highest Priority Task\n";
        cout << "3. Search Task by ID\n";
        cout << "4. Display All Tasks in Heap\n";
        cout << "5. Display Hash Table Structure\n";
        cout << "6. Exit\n";
        cout << "Choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n\n";
            continue;
        }

        if (choice == 1) {
            string title;
            int priority;

            cout << "Enter title: ";
            cin.ignore();
            getline(cin, title);

            cout << "Enter priority: ";
            if (!(cin >> priority)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n\n";
                continue;
            }

            Task t = {idCounter, title, priority};
            heap.insert(t);
            hashTable.insert(idCounter, t);
            heap.saveToFile(TASKS_FILENAME);
            cout << "Task added with ID: " << idCounter << "\n";
            idCounter++;
        } else if (choice == 2) {
            Task t = heap.extractMin();
            if (t.id == -1) {
                cout << "No tasks available.\n";
            } else {
                cout << "Executed Task - ID: " << t.id << ", Title: " << t.title
                     << ", Priority: " << t.priority << "\n";
                hashTable.remove(t.id);
                heap.saveToFile(TASKS_FILENAME);
            }
        } else if (choice == 3) {
            int searchId;
            cout << "Enter ID to search: ";
            if (!(cin >> searchId)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid ID.\n\n";
                continue;
            }
            Task t = hashTable.search(searchId);
            if (t.id == -1) {
                cout << "Task not found.\n";
            } else {
                cout << "Found Task - Title: " << t.title << ", Priority: " << t.priority << "\n";
            }
        } else if (choice == 4) {
            heap.display();
        } else if (choice == 5) {
            hashTable.display();
        } else if (choice == 6) {
            heap.saveToFile(TASKS_FILENAME);
            cout << "Date salvate in " << TASKS_FILENAME << ". La revedere!\n";
            break;
        }
        cout << "\n";
    }
    return 0;
}
