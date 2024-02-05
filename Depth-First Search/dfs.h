#ifndef __DFS_H__
#define __DFS_H__

#include "Profiler.h"

#define MAX_ROWS 100
#define MAX_COLS 100


typedef struct{
    int rows;
    int cols;
    int mat[MAX_ROWS][MAX_COLS];
}Grid;

typedef struct{
    int row;
    int col;
}Point;

enum{
    COLOR_WHITE = 0,
    COLOR_GRAY,
    COLOR_BLACK
};

typedef struct R1 {
    Point key;
    Point parent;
};

typedef struct _Node{
    Point position;
    int adjSize;
    int time_start, time_final;
    struct _Node **adj;

    int color;
    int dist;
    struct _Node *parent;
}Node;

typedef struct QNode {
    QNode* prev, * next;
    Node* data;
};

typedef struct Queue{
    QNode* first, * last;
};

typedef struct{
    int nrNodes;
    Node **v;
}Graph;

int get_neighbors(const Grid *grid, Point p, Point neighb[]);
void grid_to_graph(const Grid *grid, Graph *graph);
void free_graph(Graph *graph);
void bfs(Graph* graph, Node* s, Operation* op = NULL);
void dfs(Graph* graph, Node* s, Operation* op = NULL);
void print_bfs_tree(Graph *graph);
void printQueue(Queue *queue);
int shortest_path(Graph *graph, Node *start, Node *end, Node *path[]);
void performance();
Queue* newQueue();
bool isQueueEmpty(Queue *queue);
void enqueue(Queue* queue, Node* data);
Node* dequeue(Queue* queue);
Node* dequeueReversed(Queue* queue);


#endif