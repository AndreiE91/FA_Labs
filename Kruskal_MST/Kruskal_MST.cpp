using namespace std;
#include <iostream>
#include <windows.h> //WinApi header for colored text
#include "Profiler.h"

/* CONCLUSIONS|OBSERVATIONS|INTERPRETATIONS

    I notice on the profiler report that for 100 vertices and 400 edges, total number of operations performed is 595, corresponding very nicely to a linear 
    time complexity. Since edges are always 4 times as many as vertices, it will always be the case of T=|E| + |V + variable for the tests performed in the program.
    Variable part is explained below. NOTE: The real algorithm is actually superlinear, only reason this graph looks linear is because we only consider basic set operations
    and we ignore the sorting part.

    The real bottleneck in performance is actually the sorting part, which is why it is important to use a very efficient algorithm. For this, I used quickSort with
    random pivot selection, one of the fastest sorting algorithms that I know. Since there needs to be sorted as many elements as there are edges, it will always be a 
    factor of ElogE somewhere in the calculation of time complexity(or VlogV if there are less edges than vertices, not the case here). For this lab, the sorting operations
    are not counted as we only focused on the basic set operations make, find and union. 

    In each iteration of Kruskal's algorithm, there will be |V| number of make() calls, |E| number of find() calls and anywhere between |V| and |E| union() calls. This is
    the previously mentioned variable part.

    Since this is always the case, and the number of tests smooth our graph from bumps, we get a nice linear curve. In reality, if we were to also consider the previously
    mentioned quickSort part of the edges, we would obtain a graph corresponding of  O(ElogE) which is very familiar to the one from quickSort and is superlinear.
    
*/

//Constants
#define NR_TESTS 5
#define N 10000
#define STEP_SIZE 100
#define EDGE_FACTOR 4
#define MAX_RAND_WEIGHT 100
#define MIN_RAND_WEIGHT 0
#define DEMO_SIZE 10

//Data structures
typedef struct Node {
    int key, rank;
    Node* parent;
    bool initialized;
};

typedef struct Edge {
    int vertex1, vertex2, weight;
};

//Prototypes
void make(Node* x);
void link(Node* x, Node* y);
Node* find(Node* x);
void demo();
void printElements(int size, Node** members);
void printNode(int data, int parent, int rank);
void setUnion(Node* x, Node* y);
void allocateMembers(Node** members, int size);
void printSet(int set[], int size);
void copyArray(int in[], int copy[], int n);
void printArr(int arr[], int n);
void swapVal(Edge& a, Edge& b);
int randomPartition(Edge arr[], int left, int right);
int partition(Edge arr[], int left, int right);
void quickSort(Edge arr[], int left, int right, bool random);
int randomNumber(int low, int high);
bool existsInArray(int arr[], int size, int element);
void kruskal(int vertices[], Edge* edgeStruct, int nrVertices, int nrEdges, int& outputPathWeight, Edge* outputEdgeSet, int countEdgeStruct, bool print);
void resetIndexes();
void perf();

//Globals
Profiler p("DisjointSets");
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
Node* membersDemo[DEMO_SIZE];
int opMake, opUnion, opFind;

//Function definitions
void resetIndexes() {
    opMake = 0;
    opUnion = 0;
    opFind = 0;
}

int randomNumber(int low, int high) {
    return(rand() % (high - low + 1) + low);
}

int randomPartition(Edge arr[], int left, int right) {
    int i = randomNumber(left, right - 1);
    swapVal(arr[right], arr[i]);
    return partition(arr, left, right);
}

int partition(Edge arr[], int left, int right) {
    Edge x = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; ++j) {
        if (arr[j].weight <= x.weight) {
            ++i;
            swapVal(arr[i], arr[j]);
        }
    }
    swapVal(arr[i + 1], arr[right]);
    return i + 1;
}

void quickSort(Edge arr[], int left, int right, bool random) {
    if (left < right) {
        int pivot = random ? randomPartition(arr, left, right) : partition(arr, left, right);
        quickSort(arr, left, pivot - 1, random);
        quickSort(arr, pivot + 1, right, random);
    }
}

void swapVal(Edge& a, Edge& b) {
    Edge temp = a;
    a = b;
    b = temp;
}

void copyArray(int in[], int copy[], int n) {
    for (int i = 0; i < n; ++i) {
        copy[i] = in[i];
    }
}

void printArr(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void printNode(int data, int parent, int rank) {
    SetConsoleTextAttribute(hConsole, 15);
    cout << data;
    SetConsoleTextAttribute(hConsole, 7);
    cout << "|";
    SetConsoleTextAttribute(hConsole, 13);
    cout << "p=" << parent;
    SetConsoleTextAttribute(hConsole, 7);
    cout << "|";
    SetConsoleTextAttribute(hConsole, 11);
    cout << "r=" << rank;
    SetConsoleTextAttribute(hConsole, 7);
}

void printElements(int size, Node** members) {
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Sets print:\n";
    SetConsoleTextAttribute(hConsole, 7);
    cout << "{";
    for (int i = 0; i < size; ++i) {
        if (members[i]->initialized) {
            printNode(members[i]->key, members[i]->parent->key, members[i]->rank);
        }
        if (i + 1 != size) {
            cout << " ,\n";
        }
    }
    cout << "}\n";
}

void printSet(int set[], int size) {
    cout << "{";
    for (int i = 0; i < size; ++i) {
        cout << set[i];
        if (i + 1 != size) {
            cout << " ,";
        }
    }
    cout << "}\n";
}

void setUnion(Node* x, Node* y) {
    link(find(x), find(y));
}

void make(Node* x) {
    x->parent = x;
    x->rank = 0;
}

void link(Node *x, Node *y) {
    if (x->rank > y->rank) {
        y->parent = x;
    }
    else {
        x->parent = y;
        if (x->rank == y->rank) {
            ++y->rank;
        }
    }
}

void allocateMembers(Node** members, int size) {
    for (int i = 0; i < size; ++i) {
        members[i] = new Node();
        members[i]->initialized = false;
    }
}

Node* find(Node* x) {
    if (x->parent != x) {
        return find(x->parent);
    }
    return x->parent;
}

//O(ElgE) or O(VlgV) - take whichever is bigger
void kruskal(int vertices[], Edge* edgeStruct, int nrVertices, int nrEdges, int &outputPathWeight, Edge *outputEdgeSet, int countEdgeStruct, bool print) { //nrEdges is half of that here as it they are not yet pairs
    Node** elements = new Node*[nrVertices];
    allocateMembers(elements, nrVertices);
    for (int i = 0; i < nrVertices; ++i) {
        elements[i]->key = vertices[i];
        ++opMake;
        make(elements[i]);
        elements[i]->initialized = true;
    }
    //printElements(nrVertices, elements);
    quickSort(edgeStruct, 0, countEdgeStruct - 1, true);
    if (print) {
        SetConsoleTextAttribute(hConsole, 3);
        cout << "Edges after sorting:\n";
        SetConsoleTextAttribute(hConsole, 7);
    }
    int countOutputEdgeSet = 0;
    for (int i = 0; i < countEdgeStruct; ++i) {
        if (print) {
            cout << "V1=" << edgeStruct[i].vertex1 << " V2=" << edgeStruct[i].vertex2 << " W=" << edgeStruct[i].weight << "\n";
        }
    }
    for (int i = 0; i < countEdgeStruct; ++i) {
        Node* temp1 = NULL; //will have to find exactly which vertices each int from edge pair belongs to
        Node* temp2 = NULL;
        for (int j = 0; j < nrVertices; ++j) {
            if (elements[j]->key == edgeStruct[i].vertex1) {
                temp1 = elements[j];
            }
            if (elements[j]->key == edgeStruct[i].vertex2) {
                temp2 = elements[j];
            }
        }
        if (temp1 == NULL || temp2 == NULL) {
            SetConsoleTextAttribute(hConsole, 4);
            cout << "Something went wrong, will terminate here to avoid error.\n";
            SetConsoleTextAttribute(hConsole, 7);
            exit(-1);
        }
        opFind += 2;
        if (find(temp1)->key != find(temp2)->key) {
            outputEdgeSet[countOutputEdgeSet] = edgeStruct[i];
            outputPathWeight += edgeStruct[i].weight;
            ++countOutputEdgeSet;
            ++opUnion;
            setUnion(temp1, temp2);
        }
    }
}

bool existsInArray(int arr[], int size, int element) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == element) {
            return true;
        }
    }
    SetConsoleTextAttribute(hConsole, 12);
    cout << "Error: Element does not exist in the array\n";
    SetConsoleTextAttribute(hConsole, 7);
    return false;
}

void perf() {
    for (int n = STEP_SIZE; n <= N; n += STEP_SIZE) {
        for (int i = 0; i < NR_TESTS; ++i) {
            resetIndexes();
            int* vertices = new int[n];
            int* edges = new int[n * 8];
            int* weights = new int[n * 4];
            FillRandomArray(vertices, n, 10, 50000, true, 0);
            for (int j = 0; j < n * 8; ++j) {
                int randomVertex = randomNumber(0, n - 1); //pick a random vertex from the set and add one each time when building edges
                                                           //to make them random but between existing vertices
                edges[j] = vertices[randomVertex];
            }
            FillRandomArray(weights, n * 4, 10, 200, false, 0);
            Edge* edgeStruct = new Edge[n * 4];
            int countEdgeStruct = 0;
            for (int i = 0; i < n * 4; i += 2) {
                edgeStruct[countEdgeStruct].vertex1 = edges[i];
                edgeStruct[countEdgeStruct].vertex2 = edges[i + 1];
                edgeStruct[countEdgeStruct].weight = weights[i / 2];
                ++countEdgeStruct;
            }
            Edge* outputEdgeSet = new Edge[countEdgeStruct]; //don't know for sure how many edges will be taken into the final result, so array length will be the maximum possible which is equal to all edges present in graph
            for (int i = 0; i < countEdgeStruct; ++i) { //set all values intially to -8000 so I know when printing the result to ignore those who are still -8000(not initialized)
                outputEdgeSet[i].vertex1 = -8000;
                outputEdgeSet[i].vertex2 = -8000;
                outputEdgeSet[i].weight = -8000;
            }
            int outputTotalWeight = 0;
            kruskal(vertices, edgeStruct, n, n * 8, outputTotalWeight, outputEdgeSet, countEdgeStruct, false);
            Operation makeKruskal = p.createOperation("kruskal-make", n);
            Operation findKruskal = p.createOperation("kruskal-find", n);
            Operation unionKruskal = p.createOperation("kruskal-union", n);
            makeKruskal.count(opMake);
            findKruskal.count(opFind);
            unionKruskal.count(opUnion);
        }
    }
    p.divideValues("kruskal-make", NR_TESTS);
    p.divideValues("kruskal-find", NR_TESTS);
    p.divideValues("kruskal-union", NR_TESTS);
    p.addSeries("kruskal-make-find", "kruskal-make", "kruskal-find");
    p.addSeries("kruskal-total", "kruskal-make-find", "kruskal-union");
    p.showReport();
}

void demo() {
    int v[] = {1, 4, 5, 6, 3, 2, 100, 26, -1, -5};
    int n = sizeof(v) / sizeof(v[0]);
    allocateMembers(membersDemo, DEMO_SIZE);
    for (int i = 0; i < n; ++i) {
        membersDemo[i]->key = v[i];
        make(membersDemo[i]);
        membersDemo[i]->initialized = true;
    }
    printElements(DEMO_SIZE, membersDemo);
    cout << "Manual union?(1/0)\n";
    int decision;
    cin >> decision;
    if (decision == 0) {
        for (int i = 0; i < n - 1; ++i) {
            setUnion(membersDemo[i], membersDemo[i + 1]);
        }
    }
    else {
        cout << "How many unions?\n";
        int unions;
        cin >> unions;
        for (int i = 0; i < unions; ++i) {
            cout << "setUnion(x, y) - !x and y are indexes of elements and not elements!\n";
            cout << "x=";
            int x, y;
            cin >> x;
            cout << "y=";
            cin >> y;
            setUnion(membersDemo[x], membersDemo[y]);
        }
    }
    printElements(DEMO_SIZE, membersDemo);
    int repeat = 1;
    do {
        int x;
        cout << "findSet(x)\nx=";
        cin >> x;
        Node* temp = NULL;
        for (int i = 0; i < n; ++i) {
            if (membersDemo[i]->key == x) {
                temp = membersDemo[i];
            }
        }
        if (temp != NULL) {
            cout << x << " belongs to set with representative = " << find(temp)->key << "\n";
        }
        else {
            cout << "Couldn't find element " << x << " belonging to any set\n";
        }
        cout << "Repeat?(1/0)\n";
        cin >> repeat;
    } while (repeat != 0);

    int vertices[] = {1, 2, 3, 4, 5, 6, 7};
    int n1 = sizeof(vertices) / sizeof(vertices[0]);
    int edges[] = {1, 2, 1, 3, 2, 4, 3, 4, 2, 3, 6, 7, 3, 6, 3, 5, 5, 7};
    int n2 = sizeof(edges) / sizeof(edges[0]);
    int weights[] = {10, 2, 7, 3, 5, 8, 4, 9, 2};
    SetConsoleTextAttribute(hConsole, 13);
    cout << "MST-Kruskal:\n";
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Vertices:\n";
    SetConsoleTextAttribute(hConsole, 7);
    printSet(vertices, n1);
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Edges:\n";
    SetConsoleTextAttribute(hConsole, 7);
    for (int i = 0; i < n2; i += 2) {
        cout << "(" << edges[i] << ", " << edges[i + 1] << ") | w=" << weights[i / 2] << "\n";
    }
    int outputPathWeight = 0; //another check to know whether algorithm works properly or not
    //create and fill an edge data structure to accomodate for later needs 
    Edge* edgeStruct = new Edge[n2];
    int countEdgeStruct = 0;
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Edge set before sorting:\n";
    SetConsoleTextAttribute(hConsole, 7);
    for (int i = 0; i < n2; i += 2) {
        edgeStruct[countEdgeStruct].vertex1 = edges[i];
        edgeStruct[countEdgeStruct].vertex2 = edges[i + 1];
        edgeStruct[countEdgeStruct].weight = weights[i / 2];
        cout << "V1=" << edgeStruct[countEdgeStruct].vertex1 << " V2=" << edgeStruct[countEdgeStruct].vertex2 << " W=" << edgeStruct[countEdgeStruct].weight << "\n";
        ++countEdgeStruct;
    }
    Edge* outputEdgeSet = new Edge[countEdgeStruct]; //don't know for sure how many edges will be taken into the final result, so array length will be the maximum possible which is equal to all edges present in graph
    for (int i = 0; i < countEdgeStruct; ++i) { //set all values intially to -8000 so I know when printing the result to ignore those who are still -8000(not initialized)
        outputEdgeSet[i].vertex1 = -8000;
        outputEdgeSet[i].vertex2 = -8000;
        outputEdgeSet[i].weight = -8000;
    }
    kruskal(vertices, edgeStruct, n1, n2, outputPathWeight, outputEdgeSet, countEdgeStruct, true);
    SetConsoleTextAttribute(hConsole, 1);
    cout << "Edge set output by Kruskal:\n";     //print final result
    SetConsoleTextAttribute(hConsole, 7);
    for (int i = 0; i < countEdgeStruct; ++i) {
        if (outputEdgeSet[i].vertex1 != -8000) {
            cout << "V1=" << outputEdgeSet[i].vertex1 << " V2=" << outputEdgeSet[i].vertex2 << " W=" << outputEdgeSet[i].weight << "\n";
        }
    }
    SetConsoleTextAttribute(hConsole, 14);
    cout << "Output path weight=" << outputPathWeight << "\n";
    SetConsoleTextAttribute(hConsole, 7);
}

int main(){
    srand(time(NULL));
    int repeat = 0;
    do {
        int decision = -1;
        do
        {
            SetConsoleTextAttribute(hConsole, 14);
            cout << "Hello! Choose one option:\n";
            SetConsoleTextAttribute(hConsole, 10);
            cout << " ***************************************\n";
            cout << " * 0 - Sets + Kruskal demo()           *\n";
            cout << " * 1 - Kruskal perf()                  *\n";
            cout << " ***************************************\n";
            SetConsoleTextAttribute(hConsole, 7);
            cin >> decision;
        } while (decision < 0 || decision > 1);
        switch (decision) {
        case 0: {
            demo();
            break;
        }
        case 1: {
            perf();
            break;
        }
        default: {
            cout << "Invalid decision ID\n";
            break;
        }
        }
        cout << "Repeat?(1/0)\n";
        cin >> repeat;
    } while (repeat == 1);
}

