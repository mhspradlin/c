// short.c
// This C program finds the shortest path between two nodes in a directed
// acyclic graph. The nodes are specified by id and it is assumed that all nodes
// have unique ids.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 64

// Data structures

struct node {
    int num_forward;
    int *forward;
    int num_back;
    int *back;
    int id;
};

struct path {
    int length;
    int *elems;
};

// Protyptes
struct node *init_node (struct node **cache, int id, int num_forward, int *forward, int num_back, int *back);
struct node **init_cache (void);
struct node *get_node (struct node **cache, int id);
void insert_node (struct node **cache, int *input);
void print_path (struct path *path);
struct path *new_path (int id);
struct path *add_stop (struct path *path, int id);
struct path **init_path_cache (void);
struct path *find_shortest (struct node **graph, int id1, int id2);
struct path *find_shortest_ (struct path **cache, struct node **graph, int id1, int id2);

int main (int argc, char **argv) {
    struct node **graph = init_cache ();
    int node1[6] = {1, 3, 0, 2, 3, 5};
    int node2[6] = {2, 1, 2, 4, 1, 3};
    int node3[6] = {3, 2, 1, 2, 5, 1};
    int node4[5] = {4, 1, 1, 5, 2};
    int node5[5] = {5, 0, 2, 4, 3};
    insert_node (graph, node1);
    insert_node (graph, node2);
    insert_node (graph, node3);
    insert_node (graph, node4);
    insert_node (graph, node5);
    struct path *shortest = find_shortest (graph, 1, 5);
    print_path (shortest);
    return 0;
}

//Given a node cache and some information, basically just allocates a node and
//adds it to the cache
struct node *init_node (struct node **cache, int id, int num_forward, int *forward, int num_back, int *back) {
    struct node *new_node = malloc (sizeof (struct node));
    new_node->num_forward = num_forward;
    new_node->num_back = num_back;
    new_node->id = id;
    new_node->forward = malloc (num_forward * sizeof (struct node *));
    memcpy (new_node->forward, forward, num_forward * sizeof (int));
    new_node->back = malloc (num_back * sizeof (struct node *));
    memcpy (new_node->back, back, num_back * sizeof (int));
    // Note that we assume the cache is big enough for this to just index into
    cache[id] = new_node;
    return new_node;
}

// Initializes a node cache
// Just capped at 64 entries for now
struct node **init_cache (void) {
    struct node **cache = malloc (MAX_NODES * sizeof (struct node *));
    memset (cache, 0, MAX_NODES * sizeof (struct node *));
    return cache;
}

// Given an id, gets the node * from that
struct node *get_node (struct node **cache, int id) {
    return cache[id];
}

// Takes an array of numbers in the format [id, num_for, num_back, for, for,
// ..., back, back] and converts it into a node that is inserted into the cache
void insert_node (struct node **cache, int *input) {
    int id = input[0];
    int num_for = input[1];
    int num_bak = input[2];
    int *forward = malloc (num_for * sizeof (int));
    int *back = malloc (num_bak * sizeof (int));
    memcpy (forward, &input[3], num_for * sizeof (int));
    memcpy (back, &input[num_for + 3], num_bak * sizeof (int));
    init_node (cache, id, num_for, forward, num_bak, back);
    return;
}

// Prints a path *
void print_path (struct path *path) {
    int len = path->length;
    int i;
    for (i = 0; i < len; i++)
        printf ("%d\n", path->elems[i]);
    return;
}

// Makes a new path
struct path *new_path (int id) {
    struct path *stub = malloc (sizeof (struct path));
    stub->length = 1;
    stub->elems = malloc (sizeof (int));
    stub->elems[0] = id;
    return stub;
}

// Adds an element to the end of a path, returning a new path
struct path *add_stop (struct path *path, int id) {
    struct path *new_path = malloc (sizeof (struct path));
    new_path->length = path->length + 1;
    new_path->elems = malloc (new_path->length * sizeof (int));
    memcpy (new_path->elems, path->elems, path->length * sizeof (int));
    new_path->elems[new_path->length - 1] = id;
    return new_path;
}

// Initializes a path cache
struct path **init_path_cache (void) {
    struct path **cache = malloc (MAX_NODES * sizeof (struct path *));
    memset (cache, 0, MAX_NODES * sizeof (struct path *));
    return cache;
}

// Finds the shortest path between two given node ids, starting at the first and
// going to the second
struct path *find_shortest (struct node **graph, int id1, int id2) {
    struct path **path_cache = init_path_cache ();
    return find_shortest_ (path_cache, graph, id1, id2);
}

struct path *find_shortest_ (struct path **cache, struct node **graph, int id1, int id2) {
    printf ("id1: %d, id2: %d\n", id1, id2);
    if (id1 == id2) {
        struct path *stub = new_path (id1);
        cache[id1] = stub;
        return stub;
    }
    
    //struct node *start = get_node (graph, id1);
    struct node *end = get_node (graph, id2);
    // Get all of the shortest paths for the predecessors
    int i;
    for (i = 0; i < end->num_back; i++) {
        // Only do work if we have to
        if (cache[end->back[i]] == NULL)
            find_shortest_ (cache, graph, id1, end->back[i]);
    }
   // The shortest path to us is us plus the shortest of the paths to our
   // predecessors
   int min = 1024;
   int minpath = 0;
   for (i = 0; i < end->num_back; i++) {
       if (cache[end->back[i]]->length < min) {
           min = cache[end->back[i]]->length;
           minpath = end->back[i];
        }
   }
   // Add us to the cache and return
   struct path *our_shortest = add_stop (cache[minpath], id2);
   cache[id2] = our_shortest;
   return our_shortest;
}
