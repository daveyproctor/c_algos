/*
In state based design, a controller has a finite state machine representing states, which are functions to call. States return an ENUM of possible next states,
which the controller is meant to transition to, as the next function to call.

For example, the controller might have one state to read from the sensor, and then conditionally transition to two states, either store the data building up in the buffer,
or bypass. Finally, the decision model is always run.

A simple state machine would look like:

START -> read sensor
read sensor -> store buffer, run decision model
store buffer -> run decision model
run decision model -> DONE

and the microcontroller would loop repeatedly from DONE back to START. In this example, sometimes we visit the store buffer state, and sometimes we bypass it.

We want a packed encoding of a state history path, so that we can efficiently log what happened.
We have one megabyte of storage for the logs, and 1 millisecond average path time.
So there are 1000 log messages per second, and we quickly fill a megabyte.

The encoding should not be dependent on previous calls to the encoder.
Question: what is the benefit of this assumption, and potential costs?
    - Benefit: if you get turned off in the middle, you lose state.
    - Cost: prefix-based compression approaches might do very well on average if repeated state sequences occur.

The encoder and decoder can assume that path_encoder_init has been called once, and that
the state table will not change after path_encoder_init is called. The code should be 
generic to handle any possible state table conforming to the #defines below.

Assume:
Only valid state history paths:
    Every state in the state history path will be in the state table graph.
    For every sequential two states in the path, the second state will be a child of the first state.
    START will be the first path state, and DONE the last.
Only valid state tables:
    Every next state of a state in the state table will also be in the state table.
    Every state will have a unique name
    MAX_STATES will not be greater than 2^8 - 1 (255)
    START and DONE will exist as states.
    .numchildren is the length of the next_states array.

path_encoder_encode should return an encoding of path. The first byte should represent the number of bytes needed for the encoding. For example,
[ 3, 255, 214, 64, 0, 0, 0 ]
The 3 in the first place lets the storage layer ignore the trailing zeros (or whatever data is after the 3+1th index place)

path_encoder_decode should return the path, given the encoding.

Skeleton functions for path_encoder_* will be provided
main might be provided wih some test code.

state_t and state_graph_t structs will be provided.

compile:
gcc -g3 -std=c99 -pedantic -Wall -D IMPLEMENTATION=3 -o state_encoder_soln state_encoder_soln.c

run:
./state_encoder_soln

*/


#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

// do not change:
#define MAX_STATE_NAME_SIZE 16
#define MAX_STATES 100
#define MAX_CHILDREN 4
#define MAX_PATH_LEN 50

typedef struct {
    char * name;
    int num_children;
    char * next_states[MAX_CHILDREN];
} state_t;

typedef state_t state_graph_t[MAX_STATES];
typedef char path_t[MAX_PATH_LEN][MAX_STATE_NAME_SIZE];

// ENCODING_LEN should be defined based on the other parameters.
// #define IMPLEMENTATION 4
#if IMPLEMENTATION==1
#define ENCODING_LEN (MAX_PATH_LEN * (1 + MAX_STATE_NAME_SIZE))
// 50 * 17 
#elif IMPLEMENTATION==2
#define ENCODING_LEN MAX_PATH_LEN
// 50. implicit assumption is LOG2(MAX_STATES) <= 8
#elif IMPLEMENTATION==3
#define ENCODING_LEN MAX_PATH_LEN
// 50. implicit assumption is LOG2(MAX_CHILDREN) <= 8
#elif IMPLEMENTATION==4
#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))
#define LOG_4(n) (((n) >= 1<<4) ? (4 + LOG_2((n)>>4)) : LOG_2(n))
#define LOG_8(n) (((n) >= 1<<8) ? (8 + LOG_4((n)>>8)) : LOG_4(n))
#define LOG(n)   (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))
// This is the bits required to index n elements. For example, if there are 4 elements, I need 2 bits where 00, 01, 10, 11 are the indexes into the 4 elements.
// Can also be thought of as the base 2 "information" of a number.
// Note that n=0 and n=1 return 0, having no information to index these.
#define BITS_TO_INDEX(n) (((n) > 1) ? 1 + LOG(n-1) : 0)
#define ENCODING_LEN (1 + MAX_PATH_LEN * BITS_TO_INDEX(MAX_CHILDREN) / 8)
// 50*2 / 8 = 12
#endif

// do not change:
typedef uint8_t path_encoding_t[ENCODING_LEN];

// structures used in implementations 2, 3, 4
char state_names[MAX_STATES][MAX_STATE_NAME_SIZE];
int state_index(char *state){
    for (int i = 0; i < MAX_STATES; i++){
        if (0 == strcmp(state_names[i], state)) return i;
    }
    return -1;
}

// structures used in implementations 3, 4
state_graph_t _state_graph;
int child_index(state_t state, char *child){
    for (int i = 0; i < state.num_children; i++){
        if (0 == strcmp(state.next_states[i], child)) return i;
    }
    return -1;
}

// This will be called once. You may store any global variables that may be helpful for efficent encode/decode.
void path_encoder_init(state_graph_t state_graph){
    #if IMPLEMENTATION == 1
    #elif IMPLEMENTATION == 2 || IMPLEMENTATION == 3 || IMPLEMENTATION == 4
    // memset(state_names, 0, sizeof(state_names));
    for (int i=0; state_graph[i].name != NULL; i++){
        strcpy(state_names[i], state_graph[i].name);
    }
    #endif
    #if IMPLEMENTATION == 3 || IMPLEMENTATION == 4
    memcpy(&_state_graph, state_graph, sizeof(state_graph_t));
    #endif
}

// store num_bits low order bits from value into bit index idx of the bit_array.
bool bit_array_set(uint8_t * bit_array, uint8_t idx, uint8_t num_bits, uint8_t value){
    int byte_position = idx / 8;
    int bit_position = idx % 8; // e.g. 14 is remainder 6 into second byte
    if (num_bits > 8) return false;
    uint8_t value_low_order_bits = value & ((1 << num_bits) - 1);
    uint8_t value_shifted = value_low_order_bits << bit_position;

    // store the first 8 - bit_position bits of value in the bit_position and above high order bits of byte_position.
    bit_array[byte_position] &= (1 << bit_position) - 1; // clear the high order bits of byte_position
    bit_array[byte_position] |= value_shifted;

    int num_bits_remaining = num_bits - (8 - bit_position);
    if (num_bits_remaining <= 0) return true; // done

    value_shifted = value_low_order_bits >> (8 - bit_position);

    bit_array[byte_position + 1] = bit_array[byte_position + 1] >> num_bits_remaining << num_bits_remaining; // clear the low order bits of this position
    bit_array[byte_position + 1] |= value_shifted;
    return true;
}

uint8_t bit_array_get(uint8_t * bit_array, uint8_t idx, uint8_t num_bits){
    int byte_position = idx / 8;
    int bit_position = idx % 8;
    int num_bits_remaining = num_bits - (8 - bit_position);

    uint8_t low_order_bits = bit_array[byte_position] >> bit_position;
    if (num_bits_remaining <= 0) return low_order_bits & ((1 << num_bits) - 1); // clear the high order bits not asked for, then done.
    uint8_t high_order_bits = bit_array[byte_position+1] & ((1 << num_bits_remaining) - 1); // keep the low order bits of the second byte.

    return (high_order_bits << (8 - bit_position)) | low_order_bits;
}

// fills the encoding variable with the encoding of path.
// return true if successful, else false.
bool path_encoder_encode(path_t path, path_encoding_t encoding){
    memset(encoding, 0, sizeof(path_encoding_t));
    #if IMPLEMENTATION == 1
    int j = 1;
    for (int i=0, len=0; 0 != strcmp(path[i], ""); i++){
        len = strlen(path[i]);
        if (j + len + 1 > sizeof(path_encoding_t)) return false;
        memcpy(&encoding[j], path[i], len+1);
        j += len+1;
    }
    encoding[0] = j-1;
    #elif IMPLEMENTATION == 2
    int i = 0;
    for ( ; 0 != strcmp(path[i], ""); i++){
        int state_idx = state_index(path[i]);
        if (state_idx == -1){
            printf("state %s not found in _state_graph\n", path[i]);
            return false;
        }
        encoding[i+1] = state_index(path[i]);
        if (i > sizeof(path_encoding_t)) return false;
    }
    encoding[0] = i;
    #elif IMPLEMENTATION == 3
    int i = 0;
    for ( ; 0 != strcmp(path[i+1], ""); i++){
        int state_idx = state_index(path[i]);
        if (state_idx == -1){
            printf("state %s not found in _state_graph\n", path[i]);
            return false;
        }
        int child_idx = child_index(_state_graph[state_idx], path[i+1]);
        if (child_idx == -1) {
            printf("state %s in path is not a next state of %s\n", path[i+1], path[i]);
            return false; // child is not in children
        }
        encoding[i+1] = child_idx;
    }
    encoding[0] = i;
    #elif IMPLEMENTATION == 4
    int j = 8;
    for (int i = 0; 0 != strcmp(path[i+1], ""); i++){
        int state_idx = state_index(path[i]);
        if (state_idx == -1){
            printf("state %s not found in _state_graph\n", path[i]);
            return false;
        }
        int child_idx = child_index(_state_graph[state_idx], path[i+1]);
        if (child_idx == -1) {
            printf("state %s in path is not a next state of %s\n", path[i+1], path[i]);
            return false; // child is not in children
        }
        int bits_needed_to_index = BITS_TO_INDEX(_state_graph[state_idx].num_children);
        if (bits_needed_to_index > 0){
            bit_array_set(encoding, j, bits_needed_to_index, child_idx);
            j += bits_needed_to_index;
        }
    }
    encoding[0] = (j-8 + 7) / 8; // -8 is because j started at 8; +7 is because we want ceiling division by 8, not floor division.
    #endif

    return true;
}

// fill path with the state history encoded by encoding. Assume encoding was produced by path_encoder_encode.
bool path_encoder_decode(path_encoding_t encoding, path_t path){
    memset(path, 0, sizeof(path_t));
    #if IMPLEMENTATION == 1
    int i, j, len;
    for (i=1, j=0, len=0; i < encoding[0]; i++, len++){
        if (encoding[i] == 0){
            if (len > MAX_STATE_NAME_SIZE) return false;
            memcpy(path[j++], &encoding[i-len], len);
            len = -1;
            // if (encoding[i+1] == 0) break;
        }
    }
    strcpy(path[j], "");
    #elif IMPLEMENTATION == 2
    for (int i=0; i < encoding[0]; i++){ // sizeof(path_encoding_t) && encoding[i] != 0
        if (0 == strcmp("",state_names[encoding[i+1]])){
            printf("encoded digit does not name a state: %d\n", encoding[i+1]);
            return false;
        }
        strcpy(path[i], state_names[encoding[i+1]]);
    }
    #elif IMPLEMENTATION == 3
    char curr[MAX_STATE_NAME_SIZE] = "START";
    for (int i = 0; ; i++){
        strcpy(path[i], curr);
        if (i == encoding[0]) return true;
        int child_idx = encoding[i+1];
        int state_idx = state_index(curr);
        if (state_idx == -1){
            printf("encoded state %s not found\n", curr);
            return false;
        }
        if (_state_graph[state_idx].next_states[child_idx] == NULL) {
            printf("encoded digit breaks state semantics: %d\n", encoding[i+1]);
            return false;
        }
        strcpy(curr, _state_graph[state_idx].next_states[child_idx]);
    }
    #elif IMPLEMENTATION == 4
    char curr[MAX_STATE_NAME_SIZE] = "START";
    for (int i = 0, j = 8; ; i++){
        strcpy(path[i], curr);
        if (j / 8 >= encoding[0] + 1 || 0 == strcmp(curr, "DONE")) return true;
        int state_idx = state_index(curr);
        if (state_idx == -1){
            printf("encoded state %s not found\n", curr);
            return false;
        }
        int bits_needed_to_index = BITS_TO_INDEX(_state_graph[state_idx].num_children);
        int child_idx = bit_array_get(encoding, j, bits_needed_to_index);
        j += bits_needed_to_index;
        if (child_idx >= _state_graph[state_idx].num_children) {
            printf("encoded digit breaks state semantics: %d\n", encoding[j/8]);
            return false;
        }
        strcpy(curr, _state_graph[state_idx].next_states[child_idx]);
    }
    #endif

    return true;
}

int main(void){
    state_graph_t state_graph = {
        {.name = "START", .num_children = 3, .next_states = {"A", "B", "C"}},
        {.name = "A", .num_children = 3, .next_states = {"B", "C", "FAILED"}},
        {.name = "B", .num_children = 1, .next_states = {"D"}},
        {.name = "C", .num_children = 4, .next_states = {"DONE", "FAILED", "A", "D"}},
        {.name = "D", .num_children = 4, .next_states = {"A", "B", "C", "FAILED"}},
        {.name = "FAILED", .num_children = 1, .next_states = {"DONE"}},
        {.name = "DONE", .num_children = 0}
    };

    printf("encoder init\n");
    path_encoder_init(state_graph);
    printf("States: ");
    for (int i=0; 0 != strcmp(state_names[i], ""); i++){
        printf("%s,", state_names[i]);
    }
    printf("\nencoder init done\n");
    printf("Path: ");
    path_t path = {"START", "B", "D", "A", "C", "D", "C", "A", "B", "D", "FAILED", "DONE", ""};
    // path_t path = {"START", "B", "D", "A", "C", "D", "A", "B", "D", "FAILED", "DONE", ""};

    for (int i=0; 0 != strcmp("", path[i]); i++){
        if (i>0) printf(" -> ");
        printf("%s", path[i]);
    }
    path_encoding_t encoding;
    printf("\nencoder encode\n");
    path_encoder_encode(path, encoding);
    printf("Encoding: Length %d. Value: ", encoding[0]);
    for (int i=1; i < encoding[0]+1; i++){
        printf("%d ", encoding[i]);
    }
    printf("\nencoder decode\n");
    path_t decoded_path;
    path_encoder_decode(encoding, decoded_path);
    printf("Decoded path: ");
    for (int i=0; 0 != strcmp("", decoded_path[i]); i++){
        if (i>0) printf(" -> ");
        printf("%s", decoded_path[i]);
    }
    printf("\n");

    // simulate data storage malfunction
    encoding[1] = (1 << 8) - 1;
    path_encoder_decode(encoding, decoded_path);
    printf("Decoded path: ");
    for (int i=0; 0 != strcmp("", decoded_path[i]); i++){
        if (i>0) printf(" -> ");
        printf("%s", decoded_path[i]);
    }
    printf("\n");

    return 0;
}

/*
Path: START:3 -> B:1 -> D:4 -> A:3 -> C:4 -> D:4 -> C:4 -> A:3 -> B:1 -> D:4 -> FAILED:1 -> DONE:0
indexes:      1       0      0     1      3       2      2      0      0      3          0

1: 01 goes START to B AUTO D
0: 00 goes D to A
1: 01 goes A to C
3: 11 goes C to D
2: 01 goes D to C
2: 01 goes C to A
0: 00 goes A to B AUTO D
3: 11 goes D to FAILED AUTO DONE

209
11010001

202
11001010
*/