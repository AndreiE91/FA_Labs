using namespace std;
#include <iostream>
#include <windows.h> //WinApi header for colored text
#include "Profiler.h"

/* CONCLUSIONS|OBSERVATIONS|INTERPRETATIONS
* 
* 
* 
*/

//Constants
#define MAXADJ 10

//Data structures

typedef struct Vertex {
    bool onStack;
    int index, lowlink;
    int id;
    int adjSize;
    int time_start, time_final;
    Vertex** adj;

    int color;
    Vertex* parent;
};

typedef struct NodeS {
    Vertex* data;
    NodeS* prev;
};

typedef struct Stack {
    NodeS* top;
};

typedef struct Graph {
	int nrVertices;
	Vertex** vertices;
};

typedef struct R1 {
    Vertex key;
    Vertex parent;
};

//Prototypes


//Globals
Profiler p("DisjointSets");
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
enum {
    COLOR_WHITE = 0,
    COLOR_GRAY,
    COLOR_BLACK
};
int timeVV;

//Function definitions
Graph* newGraph(int maxVert) {
    Graph* gr = new Graph();
    gr->vertices = (Vertex**)malloc(maxVert * sizeof(Vertex*));
    gr->nrVertices = 0;
    return gr;
}

Stack* newStack() {
    Stack* newS = new Stack();
    newS->top = NULL;
    return newS;
}

void push(Vertex* vert, Stack* stack) {
    NodeS* newN = new NodeS;
    newN->data = vert;
    if (stack->top == NULL) {
        newN->prev = NULL;
        stack->top = newN;
    }
    else {
        newN->prev = stack->top;
        stack->top = newN;
    }
}

Vertex* pop(Stack* stack) {
    NodeS* toReturn = stack->top;
    stack->top = stack->top->prev;
    return toReturn->data;
}

void newVert(int id, Graph* graph) {
    Vertex* newV = new Vertex();
    newV->adjSize = 0;
    newV->adj = (Vertex**)malloc(MAXADJ * sizeof(Vertex*));
    newV->id = id;
    graph->vertices[graph->nrVertices] = newV;
    ++graph->nrVertices;
}

void fillVerticesId(Graph* graph, int start, int finish) {
    for (int i = start; i < finish; ++i) {
        newVert(i, graph);
    }
}

void initializeVertices(Graph* graph) {
    for (int i = 0; i < graph->nrVertices; ++i) {
        graph->vertices[i]->color = COLOR_WHITE;
        graph->vertices[i]->time_start = 0;
        graph->vertices[i]->time_final = 0;
        graph->vertices[i]->parent = NULL;
        graph->vertices[i]->index = -1;
        graph->vertices[i]->lowlink = -1;
        graph->vertices[i]->onStack = false;
    }
}

void addEdge(Graph* graph, int vertexIndex1, int vertexIndex2) {
    graph->vertices[vertexIndex1]->adj[graph->vertices[vertexIndex1]->adjSize] = graph->vertices[vertexIndex2];
    ++graph->vertices[vertexIndex1]->adjSize;
}

void printGraph(Graph* graph) {
    for (int i = 0; i < graph->nrVertices; ++i) {
        cout << "Vertex " << i << ": id=" << graph->vertices[i]->id << ", color=" << graph->vertices[i]->color << ", t_start=" << graph->vertices[i]->time_start << ", t_finish=" << graph->vertices[i]->time_final;
        cout << "\nAdj of vertex " << i << ": ";
        for (int j = 0; j < graph->vertices[i]->adjSize; ++j) {
            cout << "adj" << j << ".id=" << graph->vertices[i]->adj[j]->id << " ";
        }
        cout << "\n\n";
    }
}

void dfs_visit(Graph* graph, Vertex* v) {
    ++timeVV;
    v->time_start = timeVV;
    v->color = COLOR_GRAY;
    for (int i = 0; i < v->adjSize; ++i) {
        if (v->adj[i]->color == COLOR_WHITE) {
            v->adj[i]->parent = v;
            dfs_visit(graph, v->adj[i]);
        }
    }
    v->color = COLOR_BLACK;
    ++timeVV;
    v->time_final = timeVV;
}

void bubbleSort(Vertex** arr, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (arr[i]->time_final > arr[j]->time_final) {
                Vertex* temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void extractTopologicalSort(Graph* graph) {
    Vertex** copy = (Vertex**)malloc(graph->nrVertices * sizeof(Vertex*));
    for (int i = 0; i < graph->nrVertices; ++i) {
        copy[i] = graph->vertices[i];
    }
    bubbleSort(copy, graph->nrVertices);
    cout << "Topological sort:\n";
    for (int i = 0; i < graph->nrVertices; ++i) {
        cout << copy[i]->id << " ";
    }
}

void dfs(Graph* graph, Vertex* s) {
    for (int i = 0; i < graph->nrVertices; ++i) {
        graph->vertices[i]->color = COLOR_WHITE;
        graph->vertices[i]->parent = NULL;
    }
    timeVV = 0;
    for (int i = 0; i < graph->nrVertices; ++i) {
        if (graph->vertices[i]->color == COLOR_WHITE) {
            dfs_visit(graph, graph->vertices[i]);
        }
    }
}

void printNode(int id, int indentation, bool useNewl) {
    for (int i = 0; i < indentation; ++i) {
        cout << "    ";
    }
    cout << id << (useNewl ? "\n" : " ");
}

void printRR1(int root, R1 structKPV[], int size, int indent, bool firstTimeRealRoot) {
    if (firstTimeRealRoot) {
        firstTimeRealRoot = false;
        printNode(root, indent, true);
        ++indent;
    }
    for (int i = 0; i < size; ++i) {
        if (root == structKPV[i].parent.id) {
            printNode(structKPV[i].key.id, indent, true);
            printRR1(structKPV[i].key.id, structKPV, size, indent + 1, firstTimeRealRoot);
        }
    }
}

int index = 0;
Stack* stk = newStack();
int globcnt = 0;
Vertex* strconnected1[50];
Vertex* strconnected2[50];
Vertex* strconnected3[50];
int count1 = 0, count2 = 0, count3 = 0;

bool alreadyFound(Vertex* vert) {
    for (int i = 0; i < 50; ++i) {
        if (strconnected1[i] == vert) {
            return true;
        }
    }
    return false;
}

void strongconnect(Vertex* v) {
    v->index = index;
    v->lowlink = index;
    ++index;
    push(v, stk);
    v->onStack = true;
    for (int i = 0; i < v->adjSize; ++i) {
        if (v->adj[i]->index == -1) {
            strongconnect(v->adj[i]);
            if (v->lowlink > v->adj[i]->lowlink) {
                v->lowlink = v->adj[i]->lowlink;
            }
        }
        else if (v->adj[i]->onStack) {
            if (v->adj[i]->index < v->lowlink) {
                v->lowlink = v->adj[i]->index;
           }
        }
    }
    if (v->lowlink == v->index) {
        //Vertex** strconnected;
        Vertex* w = pop(stk);
        w->onStack = false;
        if (!alreadyFound(w)) {
            strconnected1[count1] = w;
            ++count1;
        }
        
        while (w->id != v->id) {
            w = pop(stk);
            w->onStack = false;
            if (!alreadyFound(w)) {
                strconnected1[count1] = w;
                ++count1;
            }
        }
        Vertex* breaker = new Vertex();
        breaker->id = -1;
        strconnected1[count1] = breaker;
        ++count1;
        ++globcnt;
    }
}

void tarjan(Graph* graph) {
    for (int i = 0; i < graph->nrVertices; ++i) {
        if (graph->vertices[i]->index = -1) {
            strongconnect(graph->vertices[i]);
        }
    }
}

void print_dfs_tree(Graph* graph)
{
    //first, we will represent the DFS tree as a parent array
    int n = 0; //the number of nodes
    int* p = NULL; //the parent array
    int* repr = NULL;

    int* transf = (int*)malloc(graph->nrVertices * sizeof(int));
    for (int i = 0; i < graph->nrVertices; ++i) {
        if (graph->vertices[i]->color == COLOR_BLACK) {
            transf[i] = n;
            ++n;
        }
        else {
            transf[i] = -1;
        }
    }
    if (n == 0) {
        //no DFS tree
        free(transf);
        return;
    }
    int root;
    R1* structKPV = new R1[n];
    int err = 0;
    p = (int*)malloc(n * sizeof(int));
    repr = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < graph->nrVertices && !err; ++i) {
        if (graph->vertices[i]->color == COLOR_BLACK) {
            if (transf[i] < 0 || transf[i] >= n) {
                err = 1;
            }
            else {
                repr[transf[i]] = graph->vertices[i]->id;
                if (graph->vertices[i]->parent == NULL) {
                    p[transf[i]] = -1;
                }
                else {
                    err = 1;
                    for (int j = 0; j < graph->nrVertices; ++j) {
                        if (graph->vertices[i]->parent == graph->vertices[j]) {
                            if (transf[j] >= 0 && transf[j] < n) {
                                p[transf[i]] = transf[j];
                                err = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    //Create structKPV used in previous lab for data structure R1
    for (int i = 0; i < n; ++i) {
        structKPV[i].key.id = repr[i];
        if (p[i] == -1) {
            root = repr[i];
        }
        else {
            structKPV[i].parent.id = repr[p[i]];
        }
    }
    free(transf);
    transf = NULL;

    if (!err) {
        // TODO: pretty print the BFS tree
        // the parrent array is p (p[k] is the parent for node k or -1 if k is the root)
        // when printing the node k, print repr[k] (it contains the row and column for that point)
        // you can adapt the code for transforming and printing multi-way trees from the previous labs

        printRR1(root, structKPV, n, 0, true);
    }

    if (p != NULL) {
        free(p);
        p = NULL;
    }
    if (repr != NULL) {
        free(repr);
        repr = NULL;
    }
    //printQueue(q2);
}

void demo() {
    //Build hard-coded DAG where topological sort will be performed
    Graph* topologicalDemo = newGraph(8);
    fillVerticesId(topologicalDemo, 0, 8);
    initializeVertices(topologicalDemo);
    addEdge(topologicalDemo, 1, 2);
    addEdge(topologicalDemo, 2, 4);
    addEdge(topologicalDemo, 4, 6);
    addEdge(topologicalDemo, 2, 6);
    addEdge(topologicalDemo, 1, 6);
    addEdge(topologicalDemo, 1, 3);
    addEdge(topologicalDemo, 3, 5);
    addEdge(topologicalDemo, 3, 7);
    cout << "Intial graph:\n";
    printGraph(topologicalDemo);
    dfs(topologicalDemo, topologicalDemo->vertices[1]);
    cout << "DFS tree:\n";
    print_dfs_tree(topologicalDemo);
    cout << "Graph after DFS:\n";
    printGraph(topologicalDemo);
    extractTopologicalSort(topologicalDemo);
    //Build a cyclical directed graph with multiple strongly connected components where Tarjan will be performed
    Graph* tarjanDemo = newGraph(8);
    fillVerticesId(tarjanDemo, 0, 8);
    initializeVertices(tarjanDemo);
    addEdge(tarjanDemo, 0, 1);
    addEdge(tarjanDemo, 1, 2);
    addEdge(tarjanDemo, 2, 0);
    addEdge(tarjanDemo, 3, 4);
    addEdge(tarjanDemo, 4, 5);
    addEdge(tarjanDemo, 5, 3);
    addEdge(tarjanDemo, 6, 7);
    addEdge(tarjanDemo, 7, 6);
    addEdge(tarjanDemo, 6, 2);
    addEdge(tarjanDemo, 7, 5);
    cout << "Intial graph:\n";
    /*stack data structure test
    Stack* test = newStack();
    push(tarjanDemo->vertices[1], test);
    push(tarjanDemo->vertices[2], test);
    push(tarjanDemo->vertices[5], test);
    cout << pop(test)->id << " ";
    cout << pop(test)->id << " ";
    cout << pop(test)->id << " ";
    */
    printGraph(tarjanDemo);
    tarjan(tarjanDemo);
    cout << "Strongly connected component sets:\n";
    for (int i = 0; i < count1; ++i) {
        cout << strconnected1[i]->id << " ";
    }
}

//int main() {
  //  demo();
//}