#include <stdlib.h>
#include <string.h>
#include "bfs.h"
#include <iostream>
#include <ctime>
using namespace std;

/* This lab I learned that BFSand DFS only differ in the way we select nodes; BFS uses a queue, while DFS uses a stack.Very similar algorithms which do the same thing but
// in a different order. Although DFS is yet to come in the next lab, I learned this while studying BFS. 

// Time complexity-wise, BFS runs in O(V + E) time, as the first part of the code runs a for loop that goes |V| steps and then we have the part where we enqueue every neighbor
// of every visited vertex that has not already been visited, essentially adding an edge in the output MWT of the algorithm. Important notice is that the output is a spanning
// tree, meaning it can have at most |V| - 1 edges. So, the complexity is represented as O(V + E) because we cannot know for sure which one will be bigger. There are cases where
// V is large and E is very small in comparison, and cases of little vertices and many more edges(up to |V|^2). Because we can have up to |V|^2 edges, both cases have to be included.

// The pretty print of the bfs tree was done using the previously implemented R1 structure from the sixth lab, consisting of a node and a parent. Same key-parent vector, same print
// recursive flow of algorithm, only difference was that this time key and parent are of type Point(x, y) instead of type int.

// One particular fact about BFS is that it allows us to compute the shortest path between two nodes in the aforementioned O(V + E) time. There are maany other algorithms for this task,
// but they all fall behind in performance when compared to BFS. That said, while BFS might be faster, it does not work on a directed or weighted graph. There are and will always be
// compromises, which are out of the scope of this lab. Shortest path is computed by simply walking from start node to end node via the parent relationship.

*/

Queue* newQueue() {
    Queue* newQ = new Queue();
    newQ->first = NULL;
    newQ->last = NULL;
    return newQ;
}

bool isQueueEmpty(Queue* queue) {
    if (queue->first == NULL) {
        return true;
    }
    return false;
}
void enqueue(Queue* queue, Node* data) {
    QNode* newQN = new QNode();
    newQN->next = NULL;
    newQN->data = data;
    if (queue->last != NULL) { //there exists a previous element
        queue->last->next = newQN;
        newQN->prev = queue->last;
    }
    else {
        newQN->prev = NULL;
        queue->first = newQN;
    }
    queue->last = newQN;
}

Node* dequeue(Queue* queue) {
    QNode* toReturn = queue->last;
    if (queue->last->prev != NULL) {
        queue->last = queue->last->prev;
        queue->last->next = NULL;
    }
    else {
        queue->first = NULL;
        queue->last = NULL;
    }
    return toReturn->data;
}

int get_neighbors(const Grid *grid, Point p, Point neighb[])
{
    int nrPoints = 0;
    int xPoint = p.row, yPoint = p.col;
    if (grid->mat[xPoint][yPoint] == 1) { //check inside wall for starting point
        return 0;
    }
    if (xPoint + 1 < grid->rows) { //check outside bounds
        if (grid->mat[xPoint + 1][yPoint] == 0) { //check inside wall
            neighb[nrPoints].row = xPoint + 1;
            neighb[nrPoints].col = yPoint;
            ++nrPoints;
        }
    }
    if (yPoint + 1 < grid->cols) { //check outside bounds
        if (grid->mat[xPoint][yPoint + 1] == 0) { //check inside wall
            neighb[nrPoints].row = xPoint;
            neighb[nrPoints].col = yPoint + 1;
            ++nrPoints;
        }
    }
    if (xPoint - 1 >= 0) { //check outside bounds
        if (grid->mat[xPoint - 1][yPoint] == 0) { //check inside wall
            neighb[nrPoints].row = xPoint - 1;
            neighb[nrPoints].col = yPoint;
            ++nrPoints;
        }
    }
    if (yPoint - 1 >= 0) { //check outside bounds
        if (grid->mat[xPoint][yPoint - 1] == 0) { //check inside wall
            neighb[nrPoints].row = xPoint;
            neighb[nrPoints].col = yPoint - 1;
            ++nrPoints;
        }
    }
    return nrPoints;
}

void grid_to_graph(const Grid *grid, Graph *graph)
{
    //we need to keep the nodes in a matrix, so we can easily refer to a position in the grid
    Node *nodes[MAX_ROWS][MAX_COLS];
    int i, j, k;
    Point neighb[4];

    //compute how many nodes we have and allocate each node
    graph->nrNodes = 0;
    for(i=0; i<grid->rows; ++i){
        for(j=0; j<grid->cols; ++j){
            if(grid->mat[i][j] == 0){
                nodes[i][j] = (Node*)malloc(sizeof(Node));
                memset(nodes[i][j], 0, sizeof(Node)); //initialize all fields with 0/NULL
                nodes[i][j]->position.row = i;
                nodes[i][j]->position.col = j;
                ++graph->nrNodes;
            }else{
                nodes[i][j] = NULL;
            }
        }
    }
    graph->v = (Node**)malloc(graph->nrNodes * sizeof(Node*));
    k = 0;
    for(i=0; i<grid->rows; ++i){
        for(j=0; j<grid->cols; ++j){
            if(nodes[i][j] != NULL){
                graph->v[k++] = nodes[i][j];
            }
        }
    }

    //compute the adjacency list for each node
    for(i=0; i<graph->nrNodes; ++i){
        graph->v[i]->adjSize = get_neighbors(grid, graph->v[i]->position, neighb);
        if(graph->v[i]->adjSize != 0){
            graph->v[i]->adj = (Node**)malloc(graph->v[i]->adjSize * sizeof(Node*));
            k = 0;
            for(j=0; j<graph->v[i]->adjSize; ++j){
                if( neighb[j].row >= 0 && neighb[j].row < grid->rows &&
                    neighb[j].col >= 0 && neighb[j].col < grid->cols &&
                    grid->mat[neighb[j].row][neighb[j].col] == 0){
                        graph->v[i]->adj[k++] = nodes[neighb[j].row][neighb[j].col];
                }
            }
            if(k < graph->v[i]->adjSize){
                //get_neighbors returned some invalid neighbors
                graph->v[i]->adjSize = k;
                graph->v[i]->adj = (Node**)realloc(graph->v[i]->adj, k * sizeof(Node*));
            }
        }
    }
}

void free_graph(Graph *graph)
{
    if(graph->v != NULL){
        for(int i=0; i<graph->nrNodes; ++i){
            if(graph->v[i] != NULL){
                if(graph->v[i]->adj != NULL){
                    free(graph->v[i]->adj);
                    graph->v[i]->adj = NULL;
                }
                graph->v[i]->adjSize = 0;
                free(graph->v[i]);
                graph->v[i] = NULL;
            }
        }
        free(graph->v);
        graph->v = NULL;
    }
    graph->nrNodes = 0;
}

void bfs(Graph *graph, Node *s, Operation *op)
{
    // TODO: implement the BFS algorithm on the graph, starting from the node s
    // at the end of the algorithm, every node reachable from s should have the color BLACK
    // for all the visited nodes, the minimum distance from s (dist) and the parent in the BFS tree should be set
    // for counting the number of operations, the optional op parameter is received
    // since op can be NULL (when we are calling the bfs for display purposes), you should check it before counting:
    // if(op != NULL) op->count();
    for (int i = 0; i < graph->nrNodes; ++ i) {
        if (op != NULL) op->count(3);
        graph->v[i]->color = COLOR_WHITE;
        graph->v[i]->dist = INFINITE;
        graph->v[i]->parent = NULL;
    }
    if (op != NULL) op->count(4);
    s->color = COLOR_GRAY;
    s->dist = 0;
    s->parent = NULL;
    Queue* q = newQueue();
    enqueue(q, s);
    if (op != NULL) op->count(1);
    while (!isQueueEmpty(q)) {
        if (op != NULL) op->count(2);
        Node* u = dequeue(q);
            for (int i = 0; i < u->adjSize; ++i) {
                if (op != NULL) op->count(1);
                if (u->adj[i]->color == COLOR_WHITE) {
                    if (op != NULL) op->count(4);
                    u->adj[i]->color = COLOR_GRAY;
                    u->adj[i]->dist = u->dist + 1;
                    u->adj[i]->parent = u;
                    enqueue(q, u->adj[i]);
                }
            }
            if (op != NULL) op->count(4);
        u->color = COLOR_BLACK;
    }
}

void printNode(int dataX, int dataY, int indentation) {
    for (int i = 0; i < indentation; ++i) {
        cout << "    ";
    }
    cout << "(" << dataX << "," << dataY << ")" << "\n";
}

void printRR1(Point root, R1 structKPV[], int size, int indent, bool firstTimeRealRoot) {
    if (firstTimeRealRoot) {
        firstTimeRealRoot = false;
        printNode(root.row, root.col, indent);
        ++indent;
    }
    for (int i = 0; i < size; ++i) {
        if (root.col == structKPV[i].parent.col && root.row == structKPV[i].parent.row) {
            printNode(structKPV[i].key.row, structKPV[i].key.col, indent);
            printRR1(structKPV[i].key, structKPV, size, indent + 1, firstTimeRealRoot);
        }
    }
}

void print_bfs_tree(Graph *graph)
{
    //first, we will represent the BFS tree as a parent array
    int n = 0; //the number of nodes
    int *p = NULL; //the parent array
    Point *repr = NULL; //the representation for each element in p

    //some of the nodes in graph->v may not have been reached by BFS
    //p and repr will contain only the reachable nodes
    int *transf = (int*)malloc(graph->nrNodes * sizeof(int));
    for(int i=0; i<graph->nrNodes; ++i){
        if(graph->v[i]->color == COLOR_BLACK){
            transf[i] = n;
            ++n;
        }else{
            transf[i] = -1;
        }
    }
    if(n == 0){
        //no BFS tree
        free(transf);
        return;
    }
    Point root;
    R1* structKPV = new R1[n];
    int err = 0;
    p = (int*)malloc(n * sizeof(int));
    repr = (Point*)malloc(n * sizeof(Node));
    for(int i=0; i<graph->nrNodes && !err; ++i){
        if(graph->v[i]->color == COLOR_BLACK){
            if(transf[i] < 0 || transf[i] >= n){
                err = 1;
            }else{
                repr[transf[i]] = graph->v[i]->position;
                if(graph->v[i]->parent == NULL){
                    p[transf[i]] = -1;
                }else{
                    err = 1;
                    for(int j=0; j<graph->nrNodes; ++j){
                        if(graph->v[i]->parent == graph->v[j]){
                            if(transf[j] >= 0 && transf[j] < n){
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
        structKPV[i].key = repr[i];
        if (p[i] == -1) {
            root = repr[i];
        }
        else {
            structKPV[i].parent = repr[p[i]];
        }
    }
    free(transf);
    transf = NULL;

    if(!err){
        // TODO: pretty print the BFS tree
        // the parrent array is p (p[k] is the parent for node k or -1 if k is the root)
        // when printing the node k, print repr[k] (it contains the row and column for that point)
        // you can adapt the code for transforming and printing multi-way trees from the previous labs

        printRR1(root, structKPV, n, 0, true);
    }

    if(p != NULL){
        free(p);
        p = NULL;
    }
    if(repr != NULL){
        free(repr);
        repr = NULL;
    }
}

int trace_route(Graph* graph, Node* start, Node* path[]) {
    Node* node = start;
    int count = 0;
    if (node == NULL) {
        return -1;
    }
    while (node != NULL) {
        path[count] = node;
        ++count;
        node = node->parent;
    }
    return count;
}

int shortest_path(Graph* graph, Node* start, Node* end, Node* path[]) {
    bfs(graph, end, NULL);
    return trace_route(graph, start, path);
}

int randomNumber(int low, int high) {
    return(rand() % (high - low + 1) + low);
}

bool existsEdge(Node* v1, Node* v2) {
    for (int i = 0; i < v1->adjSize; ++i) {
        if (v1->adj[i] == v2) {
            return true;
        }
    }
    return false;
}

void performance()
{
    srand(time(NULL));
    int n, i;
    Profiler p("bfs");

    // vary the number of edges
    for(n=1000; n<=4500; n+=100){
        Operation op = p.createOperation("bfs-edges", n);
        Graph graph;
        graph.nrNodes = 100;
        //initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for(i=0; i<graph.nrNodes; ++i){
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
            graph.v[i]->adjSize = 0;
            graph.v[i]->adj = (Node**)malloc((graph.nrNodes - 1) * sizeof(Node*));
        }
        // TODO: generate n random edges
        // make sure the generated graph is connected
        bool error;
        int randomNodeIndex1, randomNodeIndex2;
        do {
            error = false;
            for (int i = 0; i < n; ++i) { //pick a random node from the list of all available nodes, pick another random node, and as long as one does not equal another,
                                          //link them together with an edge, else try another random node until the condition is met
                do {
                    randomNodeIndex1 = randomNumber(0, graph.nrNodes - 1);
                    randomNodeIndex2 = randomNumber(0, graph.nrNodes - 1);
                    //cout << randomNodeIndex1 << "\n";
                    //cout << randomNodeIndex2 << "\n";
                } while (randomNodeIndex1 == randomNodeIndex2 || existsEdge(graph.v[randomNodeIndex1], graph.v[randomNodeIndex2])); //assures node we want to link with an edge from current node isn't the current node itself
                //each iteration of the for loop will create a new edge between two random nodes
                graph.v[randomNodeIndex1]->adj[graph.v[randomNodeIndex1]->adjSize] = graph.v[randomNodeIndex2];
                ++graph.v[randomNodeIndex1]->adjSize;
                graph.v[randomNodeIndex2]->adj[graph.v[randomNodeIndex2]->adjSize] = graph.v[randomNodeIndex1];
                ++graph.v[randomNodeIndex2]->adjSize;
            }
            //will perform bfs once on graph, without counting any operation, just to color nodes. If not all nodes are black, then graph is not connected. If that
            //is the case, run the whole generation thing again until we have a connected graph with the desired number of edges
            bfs(&graph, graph.v[0], NULL);
            //check if all nodes are black
            for (int i = 0; i < graph.nrNodes; ++i) {
                if (graph.v[i]->color != COLOR_BLACK) {
                    error = true;
                }
            }
        } while(error); //if lucky, error will be false and this will be done only once. considering we have 100 nodes and at least 1000 edges, probability of
                        //getting an unconnected graph is very low, but it can happen if many nodes link one to another multiple times by random chance
        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    // vary the number of vertices
    for(n=100; n<=200; n+=10){
        Operation op = p.createOperation("bfs-vertices", n);
        Graph graph;
        graph.nrNodes = n;
        //initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for(i=0; i<graph.nrNodes; ++i){
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
            graph.v[i]->adjSize = 0;
            graph.v[i]->adj = (Node**)malloc((graph.nrNodes - 1) * sizeof(Node*));
        }
        // TODO: generate 4500 random edges
        // make sure the generated graph is connected

        bool error;
        int randomNodeIndex1, randomNodeIndex2;
        do {
            error = false;
            for (int i = 0; i < 4500; ++i) { //pick a random node from the list of all available nodes, pick another random node, and as long as one does not equal another,
                                          //link them together with an edge, else try another random node until the condition is met
                do {
                    randomNodeIndex1 = randomNumber(0, graph.nrNodes - 1);
                    randomNodeIndex2 = randomNumber(0, graph.nrNodes - 1);
                    //cout << randomNodeIndex1 << "\n";
                    //cout << randomNodeIndex2 << "\n";
                } while (randomNodeIndex1 == randomNodeIndex2 || existsEdge(graph.v[randomNodeIndex1], graph.v[randomNodeIndex2])); //assures node we want to link with an edge from current node isn't the current node itself
                //each iteration of the for loop will create a new edge between two random nodes
                graph.v[randomNodeIndex1]->adj[graph.v[randomNodeIndex1]->adjSize] = graph.v[randomNodeIndex2];
                ++graph.v[randomNodeIndex1]->adjSize;
                graph.v[randomNodeIndex2]->adj[graph.v[randomNodeIndex2]->adjSize] = graph.v[randomNodeIndex1];
                ++graph.v[randomNodeIndex2]->adjSize;
            }
            //will perform bfs once on graph, without counting any operation, just to color nodes. If not all nodes are black, then graph is not connected. If that
            //is the case, run the whole generation thing again until we have a connected graph with the desired number of edges
            bfs(&graph, graph.v[0], NULL);
            //check if all nodes are black
            for (int i = 0; i < graph.nrNodes; ++i) {
                if (graph.v[i]->color != COLOR_BLACK) {
                    error = true;
                }
            }
        } while (error);

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    p.showReport();
}
