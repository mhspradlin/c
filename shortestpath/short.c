// short.c
// This C program finds the shortest path between two nodes in a directed
// acyclic graph. The nodes are specified by id and it is assumed that all nodes
// have unique ids.
//
// TODO:
// Catch cycles in paths
// Make sure input file exists first
// Output shortest paths back to file(s)
// Correctness proof/convincing argument?

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Data structures

struct node {
    int num_back;
    int *back;
    int id;
};

struct path {
    int length;
    int numelems;
    int *elems;
};

// Protyptes
struct node **init_cache (int num_nodes);
struct node *get_node (struct node **cache, int id);
void print_path (struct path *path);
struct path *new_path (int id);
struct path *add_stop (int **weights, struct path *path, int from, int to);
struct path **init_path_cache (int num_nodes);
struct path *find_shortest  (int **weights, struct node **graph, int num_nodes, int id1, int id2);
struct path *find_shortest_ (int **weights, struct path **cache, struct node **graph, int id1, int id2);

int num_nodes (const char *name);
void construct_from_file (int **weights, struct node **graph, const char *name);

int main (int argc, char **argv) {
    // If bad input, bail
    if (argc != 4)
        return 1;

    int num_verts = num_nodes (argv[1]);
    int **weights = malloc (num_verts * sizeof (int *));
    int i;
    // Don't need to zero, as construct_f_f will write over every entry
    for (i = 0; i < num_verts; i++)
        weights[i] = malloc (num_verts * sizeof (int));
    struct node **graph = init_cache (num_verts);
    construct_from_file (weights, graph, argv[1]);

    struct path *shortest = find_shortest (weights, graph, num_verts, atoi(argv[2]), atoi(argv[3]));
    print_path (shortest);
    return 0;
}

// Reads in a file and returns the integer that's hopefully on the first line
int num_nodes (const char *name) {
    FILE *fp = fopen (name, "r");
    int num;
    if (fscanf (fp, "%d", &num)) {
        fclose (fp);
        return num;
    } else {
        return 0;
    }
}

// Reads in a file and constructs a weight array and a graph from an adjacency
// matrix
void construct_from_file (int **weights, struct node **graph, const char *name) { 
    // Strategy:
    // Walk a line at a time, reading one number at a time. If the entry is
    // zero, then just set the weight to zero in weights (it will never be
    // accessed). If the entry is nonzero, add the appropriate back edge to the
    // node in the graph and add it to the weights. That's pretty much it.
    FILE *fp = fopen (name, "r");
    int num;
    int toss = fscanf (fp, "%d\n", &num);
    int i;
    int j;
    int w;
    for (i = 0; i < num; i++) {
        for (j = 0; j < num; j++) {
            toss = fscanf (fp, "%d", &w);
            // If the weight is nonzero
            if (w) {
                weights[i][j] = w;
                graph[j]->num_back += 1;
                graph[j]->back = realloc (graph[j]->back, graph[j]->num_back * sizeof (int));
                graph[j]->back[graph[j]->num_back - 1] = i;
            } else {
                weights[i][j] = 0;
            }
        }
        // Eat a newline/whitespace
        toss = fscanf (fp, " ");
        if (toss)
            printf ("Shouldn't see this?\n");
    }   
}


// Initializes a node cache
struct node **init_cache (int num_nodes) {
    struct node **cache = malloc (num_nodes * sizeof (struct node *));
    memset (cache, 0, num_nodes * sizeof (struct node *));
    int i;
    for (i = 0; i < num_nodes; i++) {
        cache[i] = malloc (sizeof (struct node));
        cache[i]->id = i;
    }
    return cache;
}

// Given an id, gets the node * from that
struct node *get_node (struct node **cache, int id) {
    return cache[id];
}

// Prints a path *
void print_path (struct path *path) {
    int len = path->numelems;
    // If there's no path, say so and leave
    if (len == 0) {
        printf ("No path exists.\n");
        return;
    }
    int i;
    printf ("Path order: %d", path->elems[0]);
    for (i = 1; i < len; i++)
        printf (", %d", path->elems[i]);
    printf ("\nCost: %d\n", path->length);
    return;
}

// Makes a new path
struct path *new_path (int id) {
    struct path *stub = malloc (sizeof (struct path));
    stub->length = 0;
    stub->numelems = 1;
    stub->elems = malloc (sizeof (int));
    stub->elems[0] = id;
    return stub;
}

// Makes a special 'no path' path
struct path *no_path (void) {
    struct path *stub = malloc (sizeof (struct path));
    memset (stub, 0, sizeof (struct path));
    stub->numelems = 0;
    return stub;
}

// Adds an element to the end of a path, returning a new path
struct path *add_stop (int **weights, struct path *path, int from, int to) {
    struct path *new_path = malloc (sizeof (struct path));
    new_path->length = path->length + weights[from][to];
    new_path->numelems = path->numelems + 1;   
    new_path->elems = malloc (new_path->numelems * sizeof (int));
    memcpy (new_path->elems, path->elems, path->numelems * sizeof (int));
    new_path->elems[new_path->numelems - 1] = to;
    return new_path;
}

// Detects if a given path has a cycle involving the last added element
bool has_cycle (struct path *path) {
    int last_stop = path->elems[path->numelems - 1];
    int i;
    for (i = 0; i < path->numelems - 1; i++) {
        printf ("last: %d, considering: %d\n", last_stop, path->elems[i]);
        if (last_stop == path->elems[i])
            return true;
    }
    return false;
}

// Initializes a path cache
struct path **init_path_cache (int num_nodes) {
    struct path **cache = malloc (num_nodes * sizeof (struct path *));
    memset (cache, 0, num_nodes * sizeof (struct path *));
    return cache;
}

// Finds the shortest path between two given node ids, starting at the first and
// going to the second
struct path *find_shortest (int **weights, struct node **graph, int num_nodes, int id1, int id2) {
    struct path **path_cache = init_path_cache (num_nodes);
    return find_shortest_ (weights, path_cache, graph, id1, id2);
}

struct path *find_shortest_ (int **weights, struct path **cache, struct node **graph, int id1, int id2) {
    printf ("id1: %d, id2: %d\n", id1, id2);
    if (id1 == id2) {
        struct path *stub = new_path (id1);
        cache[id1] = stub;
        return stub;
    }

    // If the path passed to us is a cycl
    
    //struct node *start = get_node (graph, id1);
    struct node *end = get_node (graph, id2);

    // Get all of the shortest paths for the predecessors
    // Also, the shortest path to us is us plus the shortest of the paths to our
    // predecessors
    int i;
    int min = 65535;
    int minpath = 0;
    for (i = 0; i < end->num_back; i++) {
        struct path *cur = cache[end->back[i]];
        // Only do work if we have to
        if (cur == NULL) {
            cur = find_shortest_ (weights, cache, graph, id1, end->back[i]);
        }
	    // Now we definitely have a value
        if (cur->numelems > 0 && cur->length + weights[end->back[i]][id2] < min) {
            min = cur->length + weights[end->back[i]][id2];
            minpath = end->back[i];
        }
    }

    // If there's no path, return that
    // We also get here if there's nobody behind us
    if (min == 65535)
        return no_path ();


    // Add us to the cache and return
    struct path *our_shortest = add_stop (weights, cache[minpath], minpath, id2);

    // If we've cycled (only happens if there's a negative cycle) then print out
    // that we have a negative cycle and the path that has it
    // Might not terminate if we do... Hmm...
    if (has_cycle (our_shortest)) {
        printf ("======== We have a cycle =======\n");
        print_path (our_shortest);
        printf("================\n");
    }

    // Add us to the cache
    cache[id2] = our_shortest;
    return our_shortest;
}
