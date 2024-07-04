/*
It is helpful to be able to generate all possible transitions. Two use cases for this would be:
1. override the transition from a state, for the sake of testing. For instance, it might be rare that a given state is transitioned to for real, but you want to 
test what will happen if such a transition happens. A testing suite sends you into all possible states and transitions. You can determine if you are able to meet the SLA of 
START -> DONE in an appropriate amount of time, no matter the transitions that happen. Controllers can also time-box the individual states to garauntee critical path length.

2. If the device realizes it is stuck, not making progress, it can similarly go into a toggle-all-states routine where it takes the above enumeration to start trying things
that it might not ordinarily do. This toggling can increase the likelihood that the controller gets un-stuck.

An example of using this to get physically unstuck, for a robot controller, would be the following state machine, where states are physical actions it can take.
START -> forward
forward -> jump
jump -> turn
jump -> duck
jump -> forward
jump -> DONE
duck -> backward
backward -> DONE

The program should return an enumeration of all possible transitions.
The transitions will be two-string arrays of start and end state.
The transitions will be listed in a pre-order traversal, such that all the transitions originating from a given state are listed after one another.
The controller will loop from DONE back to START automatically, so these states are always expected, but no link between them is stated.

for example, given:
state_table_t state_table = {
    {"START", "A"}, 
    {"START", "B"},
    {"START", "C"},
    {"A", "B"},
    {"A", "C"},
    {"A", "FAILED"},
    {"B", "D"},
    {"D", "DONE"},
    {"D", "A"},
    {"FAILED", "DONE"}
    };

return, where the index is listed above the returned matrix. Duplicates are ok, like 2 and 3, but should be minimized.
0
START A
A B
B D
D DONE
FAILED DONE


1
START B
A B
B D
D DONE
FAILED DONE


2
START C
A B
B D
D DONE
FAILED DONE


3
START C
A B
B D
D DONE
FAILED DONE


4
START A
A C
B D
D DONE
FAILED DONE

...

Ideally the implementation should not need additional memory.
*/

    state_table_t state_table = {
        {"START", "A"}, 
        {"START", "B"},
        {"START", "C"},
        {"A", "B"},
        {"A", "C"},
        {"A", "FAILED"},
        {"B", "D"},
        {"D", "DONE"},
        {"D", "A"},
        {"FAILED", "DONE"}
        };

    state_table_t pruned_table;
    for (int j = 0; j < 20; j++){
        printf("\n\n%d\n", j);
        memset(pruned_table, 0, sizeof(pruned_table));
        enum_state_tables(state_table, pruned_table, j);
        for (int i=0; pruned_table[i][0] != NULL; i++){
            printf("%s %s\n", pruned_table[i][0], pruned_table[i][1]);
        }
    }

// SCRAP:
#define MAX_TRANSITIONS 200
typedef char * state_table_t [MAX_TRANSITIONS][2]; 
void enum_state_tables(state_table_t state_table, state_table_t pruned_table, uint64_t n){
    // TODO
    char * state = NULL;
    int j = 0; // index into pruned_table
    for (int i=0, len=0; state_table[i][0] != NULL; i=i+len){
        state = state_table[i][0];
        len=0;
        while (state_table[i+len][0] != NULL && strcmp(state, state_table[i+len][0]) == 0){
            len++;
        }
        int num_bits = 8*sizeof(len) - __builtin_clz(len-1); // ceiling of log2(len). e.g. when len is 4, binary of 4-1 is 011 low order bits is 2 =ceiling(log2(4)).
        int idx = n & ((1 << num_bits) - 1); // num_bits low order bits of n;
        if (idx > len-1){ // duplicates can happen here. Not sure how to avoid this.
            idx = len-1;
        }
        // printf("%s %d\n", state, len);
        // printf("num_bits %d\n", num_bits);
        // printf("idx %d\n", idx);
        // printf("copy in: %s %s\n", state_table[i+idx][0], state_table[i+idx][1]);
        memcpy(&pruned_table[j++], &state_table[i+idx], sizeof(state_table[i+idx]));
        n = n >> num_bits;
    }
    return;
}

    memcpy(&_state_graph, &state_graph, sizeof(state_graph_t));
// dummy structures
state_graph_t _state_graph;


    printf("\nState Table:\n");
    for (int i=0, j=0; state_graph[i].name != NULL; i++){
        for(j=0; state_graph[i].next_states[j]!=NULL;j++);
        printf("%s has %d children\n", state_graph[i].name, j);

        printf("%s -> ", state_graph[i].name);
        for(j=0; state_graph[i].next_states[j]!=NULL;j++){
            printf("%s, ", state_graph[i].next_states[j]);
        }
        printf("\n");
    }
    // int indices[MAX_PATH_LEN] = { 0 };
    for (int i=0, j=0, k=0; path[i+1] != NULL; i++){
        printf("%d: %s to %s\n", i, path[i], path[i+1]);
        // seek to path[i]'s position in the table
        for (j=0; state_graph[j].name != NULL && 0 != strcmp(state_graph[j].name, path[i]); j++);
        if (state_graph[j].name == NULL) return -1; // path element not in state table
        // seek within next_states to path[i+1]'s position
        for (k=0; state_graph[j].next_states[k] != NULL && 0 != strcmp(state_graph[j].next_states[k], path[i+1]); k++);
        // save this index as next.
        printf("%s index %d\n", path[i], k);
    }

#define LOG2(x) (8* sizeof(x) - __builtin_clz(x-1))
for (int i=0; i<20; i++){
    printf("LOG2 %d: %d\n", i, (int) LOG2(i));
}

    // printf("8* sizeof(1) - __builtin_clz(1-1): %d\n", (int) (8* sizeof(1) - __builtin_clz(1-1)));
    // printf("LOG2(1): %d\n", (uint32_t) LOG2((uint32_t) 1));

    // unsigned int i = 1;
    // printf("LOG2(1): %d\n", (uint32_t) LOG2((uint32_t) i));
    // return 0;
        // if (i == 1) {
        //     printf("LOG2(1): %d\n", (int) LOG2((int) 1));
        //     printf("LOG2(1): %d\n", (int) LOG2((int) i));
        // }

    for (int i=0; i<20; i++){
        printf("BITS_TO_INDEX %d: %d\n", i, (int) BITS_TO_INDEX(i));
    }

    memset(encoding, 0, sizeof(path_encoding_t));
    printf("set the middle 4 bits\n");
    bit_array_set(encoding, 6, 4, (uint8_t) 15);
    printf("encoding[0]: %d\n", encoding[0]);
    printf("encoding[1]: %d\n", encoding[1]);
    printf("bit_array_get(encoding, 6, 6): %d\n", bit_array_get(encoding, 6, 6));
    printf("bit_array_get(encoding, 8, 4): %d\n", bit_array_get(encoding, 8, 4));
    printf("set all bits\n");
    bit_array_set(encoding, 0, 8, (uint8_t) (1 << 8) - 1);
    bit_array_set(encoding, 8, 8, (uint8_t) (1 << 8) - 1);
    printf("encoding[0]: %d\n", encoding[0]);
    printf("encoding[1]: %d\n", encoding[1]);
    printf("clear the middle 4 bits\n");
    bit_array_set(encoding, 6, 4, 0);
    printf("encoding[0]: %d\n", encoding[0]);
    printf("encoding[1]: %d\n", encoding[1]);

        printf("%s:%d", path[i], state_graph[state_index(path[i])].num_children);
