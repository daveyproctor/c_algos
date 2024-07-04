/*
Your task is to serialize a binary tree.

compile:
gcc -g3 -std=c99 -pedantic -Wall -o tree_serializer tree_serializer.c

run:
./tree_serializer

*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// helper macros
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Native tree interface using a recursive type definition and pointers.
typedef struct node_t {
    int value;
    struct node_t *left;
    struct node_t *right;
} node_t;

// Helper function to print a tree with an "in order" traversal (left subtree, me, right subtree).
void printTreeInOrder(node_t *tree){
    if (tree == NULL) return;
    printTreeInOrder(tree->left);
    printf("%d ", tree->value);
    printTreeInOrder(tree->right);
    return;
}

// New tree interface for serializing
typedef struct {
    int value;
    int left;
    int right;
} node2_t;
// max nodes to serialize
#define MAX_NODES 1024
typedef node2_t node_array_t[MAX_NODES];

/* serializeTree
 * serialize a tree represented in the 'tree' variable into the array in the 'tree_array' variable.
 * begin the serialization at array position 'idx'.
 * return the index of the final array position of the serialization.
*/
int serializeTree(node_t *tree, node_array_t tree_array, int idx){
    return -1;
    // TODO
}

/* deserializeTree
 * deserialize a tree represented in the 'tree_array' variable into its native
 * representation in the 'tree' variable.
 * The serialization is between 'start_idx' and 'end_idx'. 'tree' has at least (1+end_idx-start_idx) * sizeof(node_t)
 * consecutive available memory, starting at start_idx.
 * return a pointer to the root of the tree.
*/
node_t *deserializeTree(node_array_t tree_array, node_t *tree, int start_idx, int end_idx){
    return NULL;
    // TODO
}

int main(void){
    // Create test tree
    node_t tree0 = {.value = 4};
    node_t tree1 = {.value = 2};
    node_t tree2 = {.value = 5};
    node_t tree3 = {.value = 6};
    tree0.left = &tree1;
    tree0.right = &tree2;
    tree2.right = &tree3;
    // 4
    //    left:  2
    //    right: 5
    //         left:  NULL
    //         right: 6
    printf("Tree: ");
    printTreeInOrder(&tree0);
    printf("\n");

    // Serialize
    node_array_t tree_array;
    int end_idx = serializeTree(&tree0, tree_array, 0);
    tree0.value = 3;
    int end_idx2 = serializeTree(&tree0, tree_array, end_idx + 2);
    printf("Serialized Trees: ");
    for (int i = 0; i <= end_idx2; i++){
        printf("%d ", tree_array[i].value);
    }
    printf("\n");

    // Deserialize
    node_t new_tree[MAX_NODES];
    node_t *root = deserializeTree(tree_array, new_tree, 0, end_idx);
    printf("Deserialized Tree: ");
    printTreeInOrder(root);
    printf("\n");
    root = deserializeTree(tree_array, new_tree, end_idx + 2, end_idx2);
    printf("Deserialized Tree: ");
    printTreeInOrder(root);
    printf("\n");

    return 0;
}
