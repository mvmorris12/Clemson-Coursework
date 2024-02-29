 /*tree.h 
 * Lab5: Binary Trees
 * ECE 2230 Fall 2023 
 * Michael Morris
 * Mvmorri
 *
 * Purpose: This header files serves tree.c with function prototyhpes, structure
 *          definitions, and macro function(s).
 */
#define TRUE            1
#define FALSE           0
#define LEFT            -1
#define RIGHT           1
#define CONN_LEFT_ANG   -3
#define CONN_RIGHT_ANG  -4
#define CONN_TEE        -5
#define CONN_BAR        -2
#define EMPTY_SPACE     -1

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
typedef int tree_key_t;
typedef int mydata_t;

typedef struct NodeTag {
    tree_key_t key;
    mydata_t *data_ptr;
    struct NodeTag *left;
    struct NodeTag *right;
} tree_node_t;

typedef struct TreeTag {
    tree_node_t *root;
    int tree_size;
    int num_recent_key_comparisons;
} tree_t;

tree_t *bst_construct();
void bst_destruct(tree_t*);
int bst_insert(tree_t*, tree_key_t Key, mydata_t *DataPointer);
mydata_t *bst_remove(tree_t*, tree_key_t Key);
mydata_t *bst_search(tree_t*, tree_key_t Key);
int bst_size(tree_t*);
int bst_stats(tree_t*);
int bst_internal_path_len(tree_t*);
void bst_debug_print_tree(tree_t*);
void bst_debug_validate(tree_t*);

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
