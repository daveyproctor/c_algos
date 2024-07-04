/*
In state based design, there is a finite state machine of states, and transitions between states.
Each state will perform some action, then return the name of the next state.

For example, there could be one state to read from the sensor, and then conditionally transition to two states, 
either store the data building up in the buffer, or bypass. Finally, the decision model is always run. 
Here is that state machine:

START -> read sensor
read sensor -> store buffer, run decision model
store buffer -> run decision model
run decision model -> DONE

and the microcontroller would loop repeatedly from DONE back to START. 
In this example, sometimes we visit the store buffer state, and sometimes we bypass it.

As the controller loops through the state machine many times,
how do we efficiently log which states were visited?

Your task is to efficiently encode a state history path, which is a list of states from START to DONE which were visited.
Implement path_encoder_encode and path_encoder_decode, to encode and decode a state history path.
Optionally implement path_encoder_init, called first, which is given the state table, 
and can store global variables to help with subsequent calls to encode and decode.
Your code should work for any possible state table, but you may
assume the state table is constant, not changing.

compile:
gcc -g3 -std=c99 -pedantic -Wall -o state_encoder state_encoder.c

run:
./state_encoder

*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_STATE_NAME_SIZE 16
#define MAX_STATES 100
#define MAX_CHILDREN 4
#define MAX_PATH_LEN 50
// TODO: ENCODING_LEN should be defined based on the other parameters.
// This is the maximum number of bytes you expect to need to encode a path
#define ENCODING_LEN (MAX_STATES * MAX_STATE_NAME_SIZE)

typedef struct {
    char * name;
    int num_children;
    char * next_states[MAX_CHILDREN];
} state_t;
typedef state_t state_graph_t[MAX_STATES];
typedef char path_t[MAX_PATH_LEN][MAX_STATE_NAME_SIZE];
typedef uint8_t path_encoding_t[ENCODING_LEN];

// This will be called once. You may store any global variables that may be helpful for encode/decode.
void path_encoder_init(state_graph_t state_graph){
    // TODO (optional)
}

/* path_encoder_encode
 * fills 'encoding' with the encoding of 'path'.
 * return true if successful, else false.
 * the first byte should represent the length of the encoding.
 * for example, [ 3, 255, 214, 64, 0, 0, 0, ... ] indicates that the encoding is 3 bytes long.
 */ 
bool path_encoder_encode(path_t path, path_encoding_t encoding){
    memset(encoding, 0, sizeof(path_encoding_t));
    // TODO
    return true;
}

/* path_encoder_decode
 * inverse of path_encoder_encode
 * fill 'path' with the state history encoded by 'encoding'. Assume encoding was produced by path_encoder_encode.
 * return true if successful, else false.
*/
bool path_encoder_decode(path_encoding_t encoding, path_t path){
    memset(path, 0, sizeof(path_t));
    // TODO
    return true;
}

int main(void){
    // You may change/add to these tests if you like.
    state_graph_t state_graph = {
        {.name = "START", .num_children = 3, .next_states = {"A", "B", "C"}},
        {.name = "A", .num_children = 3, .next_states = {"B", "C", "FAILED"}},
        {.name = "B", .num_children = 1, .next_states = {"D"}},
        {.name = "C", .num_children = 4, .next_states = {"DONE", "FAILED", "A", "D"}},
        {.name = "D", .num_children = 4, .next_states = {"A", "B", "C", "FAILED"}},
        {.name = "FAILED", .num_children = 1, .next_states = {"DONE"}},
        {.name = "DONE", .num_children = 0}
    };
    path_t path = {"START", "B", "D", "A", "C", "D", "C", "A", "B", "D", "FAILED", "DONE", ""};

    printf("State machine:\n");
    for (int i=0; ; i++){
        printf("%s", state_graph[i].name);
        for (int j = 0; j < state_graph[i].num_children; j++){
            if (j == 0) printf(" -> ");
            if (j > 0) printf(", ");
            printf("%s", state_graph[i].next_states[j]);
        }
        printf("\n");
        if (0 == strcmp(state_graph[i].name, "DONE")) break;
    }
    printf("Path: ");
    for (int i=0; 0 != strcmp("", path[i]); i++){
        if (i > 0) printf(" -> ");
        printf("%s", path[i]);
    }
    printf("\n");

    printf("encoder init\n");
    path_encoder_init(state_graph);

    path_encoding_t encoding;
    printf("encoder encode\n");
    path_encoder_encode(path, encoding);
    printf("Encoding: Length %d. Value: ", encoding[0]);
    for (int i=1; i < encoding[0]+1; i++){
        printf("%d ", encoding[i]);
    }
    printf("\n");

    path_t decoded_path;
    printf("encoder decode\n");
    path_encoder_decode(encoding, decoded_path);
    printf("Decoded path: ");
    for (int i=0; 0 != strcmp("", decoded_path[i]); i++){
        if (i > 0) printf(" -> ");
        printf("%s", decoded_path[i]);
    }
    printf("\n");

    return 0;
}
