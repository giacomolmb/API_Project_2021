/*
API Project A.A. 2020/21
Author: Giacomo Lombardo - Politecnico di Milano
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
    Structure representing a graph, according to the CSR graph representation.
*/
typedef struct Graph {
    unsigned int *nodes; // Nodes of the graph
    unsigned int *arcs; // Arcs of the graph
    unsigned int *weights; // Weight of each arc
} Graph;

/*
    Structure to memorize a graph and its total weight.
*/
typedef struct WeightedGraph {
    int id;
    unsigned int weight;
} WeightedGraph;

/*
    Returns the left child of a tree node
*/
unsigned int left(unsigned int i){ return (i * 2) + 1; }

/*
    Returns the right child of a tree node
*/
unsigned int right(unsigned int i){ return (i * 2) + 2; }

/*
    Returns the parent of a tree node
*/
unsigned int parent(unsigned int i){
    if(i == 0) return 0;
    return ((i + 1) / 2) - 1;
}

/*
    Swaps two weighted graphs.
*/
void swap(WeightedGraph *a, WeightedGraph *b){
    WeightedGraph temp = *a;
    *a = *b;
    *b = temp;
}

/*
    Tries to insert a weighted graph into the topK array of weighted graphs. 
    Returns 1 if the operation is successful, 0 if the graph is not inserted into the topK.
*/
int insert(WeightedGraph topK[], int id, unsigned int weight, unsigned int *size, unsigned int MAX_SIZE){
    if(*size == 0){ // The TopK is empty, the graph is inserted at the beginning of the array.
        topK[*size].id = id;
        topK[*size].weight = weight;
        (*size)++;
        return 1;
    } else {
        if (*size < MAX_SIZE) { // The TopK is not full, the graph is inserted at the end of the array
            topK[*size].id = id;
            topK[*size].weight = weight;
            unsigned int i = *size;
            while(topK[i].weight > topK[parent(i)].weight){ // Max-Heapify
                swap(&topK[i], &topK[parent(i)]);
                i = parent(i);
            }
            (*size)++;
            return 1;
        } else { // TopK is full
            if (weight >= topK[0].weight) // The graph is too "heavy" and is not inserted into the topK
                return 0;
            else {
                topK[0].id = id; // The graph is inserted as the root of the max heap
                topK[0].weight = weight;

                unsigned int i = 0; // Max-heapify operation
                unsigned int l, r, max;
                while(1){
                    l = left(i);
                    r = right(i);
                    if(l < *size && topK[l].weight > topK[i].weight)
                        max = l;
                    else
                        max = i;
                    if(r < *size && topK[r].weight > topK[max].weight)
                        max = r;
                    if(max == i)
                        break;
                    swap(&topK[i], &topK[max]);
                    i = max;
                }

                return 1;
            }
        }
    }
}

/*
    Prints the actual topK.
*/
void printTopK(WeightedGraph topK[], unsigned int size){
    if(size > 1){
        for (int i = 0; i < size - 1; ++i) {
            printf("%d ", topK[i].id);
        }
    }
    if(size > 0)
        printf("%d", topK[size - 1].id);
    printf("\n");
}

/*
    Basic implementation of Dijkstra's algorithm to calculate the total weight of a graph represented in CSR, with some optimizations.
*/
unsigned int dijkstra(Graph *graph, unsigned int GRAPH_SIZE){
    unsigned int sum = 0;
    unsigned int distances[GRAPH_SIZE];
    int visited[GRAPH_SIZE];
    int ended = 0;
    unsigned int actual = 0, next = 0;
    unsigned int min_weight = 0;

    memset(distances, 0, sizeof(unsigned int) * GRAPH_SIZE);
    memset(visited, 0, sizeof(int) * GRAPH_SIZE);

    visited[0] = 1;

    while(!ended){
        for (unsigned int i = graph->nodes[actual]; i < graph->nodes[actual + 1]; ++i) {
            if(graph->arcs[i] != actual && (distances[graph->arcs[i]] == 0 || distances[graph->arcs[i]] > (distances[actual] + graph->weights[i]))) {
                if (!visited[graph->arcs[i]]) {
                    distances[graph->arcs[i]] = distances[actual] + graph->weights[i];
                    if (min_weight == 0 || min_weight > distances[graph->arcs[i]]) {
                        min_weight = distances[graph->arcs[i]];
                        next = graph->arcs[i];
                    }
                }
            }
        }
        if(next == actual){
            min_weight = 0;
            for (int i = 0; i < GRAPH_SIZE; ++i) {
                if(!visited[i] && distances[i] != 0 && (min_weight == 0 || min_weight > distances[i])){
                    min_weight = distances[i];
                    next = i;
                }
            }
        }

        if(next == actual) {
            ended = 1;
        } else {
            actual = next;
            visited[actual] = 1;
            sum += distances[actual];
        }
    }

    return sum;
}

/*
    MAIN
*/
int main(){
    char *input;
    char *input_pointer;
    unsigned int parsed_int;
    char *token;
    const char delim[2] = " ";
    unsigned int GRAPH_SIZE = 0, TOPK_SIZE = 0;

    int counter = 0;
    unsigned int read_numbers;
    unsigned int added_numbers = 0;
    int NULL_WEIGHT;
    int TOPK_ONLY_ZEROS = 0; // Flag activated if the topK is composed only of 0-weight graph.
    
    input = malloc(sizeof (char) * 8192);

    if(fgets(input, 8192, stdin)) { // Reads the first two numbers, representing graph and topK sizes.
        token = strtok(input, delim);
        GRAPH_SIZE = (unsigned int) strtoul(token, &token, 10);
        token = strtok(NULL, delim);
        TOPK_SIZE = (unsigned int) strtoul(token, &token, 10);
    }

    WeightedGraph topK[TOPK_SIZE];
    unsigned int actual_size = 0;

    unsigned int MALLOC_SIZE = (unsigned int) sizeof(unsigned int) * GRAPH_SIZE;

    Graph graph;
    graph.nodes = malloc(MALLOC_SIZE + 1);
    graph.nodes[0] = 0;
    graph.arcs = malloc(MALLOC_SIZE * MALLOC_SIZE);
    graph.weights = malloc(MALLOC_SIZE * MALLOC_SIZE);

    while(fgets(input, 8192, stdin)){
        if(strncmp(input, "AggiungiGrafo", 13) == 0){
            NULL_WEIGHT = 0;
            for (int i = 0; i < GRAPH_SIZE; i++) {
                if(fgets(input, 8192, stdin)) {
                    if(!TOPK_ONLY_ZEROS){
                        read_numbers = 0;
                        added_numbers = 0;
                        input_pointer = input;
                        while(*input_pointer){
                            if(isdigit(*input_pointer)){
                                parsed_int = strtoul(input_pointer, &input_pointer, 10);
                                if(parsed_int != 0 && read_numbers != i && read_numbers != 0){
                                    graph.arcs[graph.nodes[i] + added_numbers] = read_numbers;
                                    graph.weights[graph.nodes[i] + added_numbers] = parsed_int;
                                    added_numbers++;
                                }
                                read_numbers++;
                            } else {
                                input_pointer++;
                            }
                        }
                    }
                }
                if(!TOPK_ONLY_ZEROS){
                    if(i == 0 && added_numbers == 0){
                        NULL_WEIGHT = 1;
                        break;
                    }
                    graph.nodes[i + 1] = added_numbers + graph.nodes[i];
                }
            }

            if(!TOPK_ONLY_ZEROS) {
                insert(topK, counter, NULL_WEIGHT ? 0 : dijkstra(&graph, GRAPH_SIZE), &actual_size, TOPK_SIZE);
            }
            if(actual_size == TOPK_SIZE && topK[0].weight == 0){
                TOPK_ONLY_ZEROS = 1;
            }
            counter++;
        } else if(strncmp(input, "TopK", 4) == 0){
            printTopK(topK, actual_size);
        }
    }

    return 0;
}