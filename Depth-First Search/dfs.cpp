#include <stdlib.h>
#include <string.h>
#include "dfs.h"
#include <iostream>
#include <ctime>
using namespace std;

/* CONCLUSIONS|OBSERVATIONS|INTERPRETATIONS
*
*       During this lab work I saw the differences between DFS and BFS, especially since I worked on the same framework with nice displaying of both processes.
*   As expected, BFS spreads uniformly across a frontier while DFS tries to go as deep as possible on one path before switching. From this it can be undestrood how
*   one might use DFS when finding something deep in a tree is necessary(especially if only one element of its kind is sufficient) and BFS when the destination is
*   expected to be closer to the source. 
*       Both algorithms run in O(V+E) time, depending on which one is bigger, we might have less than vertices and also have a complete graph where edges equal vertices
*   squared. Performance-wise, DFS seems to run a little slower than BFS on same input in my case, althoug the shape of their graphs look identical, showing that both increase
*   linearily. This might be due to the way of counting operations/introduction of constants which are irrelevant in rate of growth.
*   DFS looks
*
*/


void printNode(int dataX, int dataY, int indentation, bool useNewl);

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

int timeV;
Queue* q2 = newQueue();

Node* dequeueReversed(Queue* queue) {
    QNode* toReturn = queue->first;
    if (queue->first->next != NULL) {
        queue->first = queue->first->next;
        queue->first->prev = NULL;
    }
    else {
        queue->first = NULL;
        queue->last = NULL;
    }
    return toReturn->data;
}

void printQueue(Queue* queue) {
    cout << "Topological sort:\n";
    while (!isQueueEmpty(queue)) {
        Node* temp = dequeueReversed(queue);
        printNode(temp->position.row, temp->position.col, 0, false);
    }
    cout << "\n";
}


void dfs_visit(Graph* graph, Node* v, Operation* op) {
    if (op != NULL) op->count(3);
    ++timeV;
    v->time_start = timeV;
    v->color = COLOR_GRAY;
    for (int i = 0; i < v->adjSize; ++i) {
        if (op != NULL) op->count(1);
        if (v->adj[i]->color == COLOR_WHITE) {
            if (op != NULL) op->count(1);
            v->adj[i]->parent = v;
            dfs_visit(graph, v->adj[i], op);
            //this is where vertex is finished and will be inserted into the front of linked list
            //enqueue(q2, v);
        }
    }
    if (op != NULL) op->count(3);
    v->color = COLOR_BLACK;
    ++timeV;
    v->time_final = timeV;
}

void dfs(Graph *graph, Node *s, Operation *op) {
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (op != NULL) op->count(2);
        graph->v[i]->color = COLOR_WHITE;
        graph->v[i]->parent = NULL;
    }
    if (op != NULL) op->count(1);
    timeV = 0;
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (op != NULL) op->count(1);
        if (graph->v[i]->color == COLOR_WHITE) {
            dfs_visit(graph, graph->v[i], op);
        }
    }
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

void printNode(int dataX, int dataY, int indentation, bool useNewl) {
    for (int i = 0; i < indentation; ++i) {
        cout << "    ";
    }
    cout << "(" << dataX << "," << dataY << ")" << (useNewl? "\n":" ");
}

void printRR1(Point root, R1 structKPV[], int size, int indent, bool firstTimeRealRoot) {
    if (firstTimeRealRoot) {
        firstTimeRealRoot = false;
        printNode(root.row, root.col, indent, true);
        ++indent;
    }
    for (int i = 0; i < size; ++i) {
        if (root.col == structKPV[i].parent.col && root.row == structKPV[i].parent.row) {
            printNode(structKPV[i].key.row, structKPV[i].key.col, indent, true);
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
    //printQueue(q2);
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
    Profiler p("dfs");

    // vary the number of edges
    for(n=1000; n<=4500; n+=100){
        Operation op = p.createOperation("dfs-edges", n);
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
            //will perform dfs once on graph, without counting any operation, just to color nodes. If not all nodes are black, then graph is not connected. If that
            //is the case, run the whole generation thing again until we have a connected graph with the desired number of edges
            dfs(&graph, graph.v[0], NULL);
            //check if all nodes are black
            for (int i = 0; i < graph.nrNodes; ++i) {
                if (graph.v[i]->color != COLOR_BLACK) {
                    error = true;
                }
            }
        } while(error); //if lucky, error will be false and this will be done only once. considering we have 100 nodes and at least 1000 edges, probability of
                        //getting an unconnected graph is very low, but it can happen if many nodes link one to another multiple times by random chance
        dfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }
    // vary the number of vertices
    for(n=100; n<=200; n+=10){
        Operation op2 = p.createOperation("dfs-vertices", n);
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
            //will perform dfs once on graph, without counting any operation, just to color nodes. If not all nodes are black, then graph is not connected. If that
            //is the case, run the whole generation thing again until we have a connected graph with the desired number of edges
            dfs(&graph, graph.v[0], NULL);
            //check if all nodes are black
            for (int i = 0; i < graph.nrNodes; ++i) {
                if (graph.v[i]->color != COLOR_BLACK) {
                    error = true;
                }
            }
        } while (error);

        dfs(&graph, graph.v[0], &op2);
        free_graph(&graph);
    }

    p.showReport();
}
