/* tree.c     
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP5
 *
 * Propose: This file manages the binary search tree structures for the main lab5.c file.
 *          It is responsible for all creation, insertion, removal, search, printing, etc. 
 *          functions called from lab5.c
 *
 * Assumptions: The functions within this file should check for corner cases / out
 *          of bounds operations on the lists, but proper requests should be made
 *          by the user when possible. 
 *          
 *          When using the "verbose" option to print out trees, the number of initial tree 
 *          levels should be kept below approximately 6-7 depending on computer specs as the
 *          pretty-print function requires multiple passes through a 2D array and can become 
 *          quite slow with large trees. Without the -v option, much larger trees can be created.
 *
 * Bugs: Pretty-print function is wildly inneficient but works well on small trees. 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include "tree.h"


/* prototypes for functions in this file only */
void bst_debug_print_tree_node(tree_node_t *node, int level);
int bst_debug_validate_rec(tree_node_t *node, int min, int max, int *count);
int bst_internal_path_length_count(tree_node_t *node, int level);
void bst_debug_print_tree_pretty_build_array(tree_t *tree, int **node_arr);
void bst_debug_print_tree_pretty_add_array(tree_node_t *node, int **node_arr, int base, int l_or_r, int level, int max_level);
void bst_debug_print_tree_pretty_add_tees(tree_t *tree, int **node_arr, int height, int width);
void bst_debug_print_tree_pretty_add_corners(tree_node_t *node, int **node_arr, int height, int width);
void bst_debug_print_tree_pretty_add_bars(int **node_arr, int height, int width);
void bst_debug_print_tree_pretty_trim_array(int **node_arr, int height, int *width);

/* Create the header block for the tree. Initialize the root pointer to null. The tree_size stores the
 * current number of keys in the tree. The num_recent_key_comparisons stores the number of key
 * comparisons during the most recent search, insert, or remove. Use Standish’s definitions for the
 * number of comparisons even if your implementation is slightly different. That is, there is one
 * comparison to determine if the key is found at the current level and if the key is not found one
 * more comparison to determine if the next step is to the left or right. Do not count checks for null
 * pointers
 *
 * Inputs: None
 *
 * Outputs: Pointer to a newly created tree structrue
 */
tree_t *bst_construct(void){
    tree_t* new_tree = (tree_t*)malloc(sizeof(tree_t));
    new_tree->tree_size = 0;
    new_tree->num_recent_key_comparisons = 0;
    new_tree->root = NULL;
    return new_tree;
}

/* Frees all items stored in the tree including the memory block with the data and the tree_node_t
 * structure. Also frees the header block
 *
 * Inputs: tree - tree structure to free
 */
void bst_destruct(tree_t* tree){
    while (tree->tree_size){
        free(tree->root->data_ptr);
        bst_remove(tree, tree->root->key);
    }
    free(tree);
}

/* Insert the memory block pointed to by DataPointer into the tree with the associated key. The
 * function must return 0 if the key is already in the tree (in which case the data memory block is
 * replaced). The function must return 1 if the key was not already in the tree but was instead added
 * to the tree.
 *
 * Inputs: tree - pointer to tree structure to add node to
 *         Key - Key value to add to tree, used to determine if a node already exists
 *         DataPointer - pointer to data structure to add to either new or existing node
 *
 * Outputs: 0 - node already existed and datapointer was updated
 *          1 - node did not exist and was added 
 */
int bst_insert(tree_t *tree, tree_key_t Key, mydata_t *DataPointer){
    if (tree == NULL) return 1;
    tree->num_recent_key_comparisons = 0;

    tree_node_t *new_node = (tree_node_t*)malloc(sizeof(tree_node_t));
    new_node->data_ptr = DataPointer;
    new_node->key = Key;
    new_node->left = NULL;
    new_node->right = NULL;

    if (tree->tree_size == 0){
        tree->root = new_node;
        tree->tree_size++;
    } else {               
        tree_node_t *rover = tree->root;        
        do{
            tree->num_recent_key_comparisons++;
            if (Key == rover->key){
                free(rover->data_ptr);
                free(new_node);
                rover->data_ptr = DataPointer;
                return 0;
            }
            tree->num_recent_key_comparisons++;
            if (Key > rover->key){
                if (rover->right == NULL){
                    rover->right = new_node;
                    break;
                } else {
                    rover = rover->right;
                }
            } else {
                if (rover->left == NULL){
                    rover->left = new_node;
                    break;
                } else {
                    rover = rover->left;
                }
            }
        } while (rover != NULL);
        tree->tree_size++;
    }
    return 1;
}

/* Remove the item in the tree with the matching key. Return the pointer to the data memory block
 * and free the tree_node_t memory block. If the key is not found in the tree, return NULL.
 *
 * Inputs: tree - pointer to tree structure to remove node from
 *         Key - Key value of node to remove from tree
 *
 * Outputs: pointer to the datapointer field of node that was removed (or NULL if node doesn't exist in tree)
 */
mydata_t *bst_remove(tree_t *tree, tree_key_t Key){
    if (tree==NULL) return NULL;
    if (tree->root == NULL) return NULL;
    tree->num_recent_key_comparisons = 0;
    tree_node_t *rover = tree->root;    
    tree_node_t *rover_prev = rover;    
    tree_node_t *rover_del = rover;    

    do{
        tree->num_recent_key_comparisons++;
        if (Key == rover->key){
            // if the value is in the bottom level
            if ((rover->left == NULL) && (rover->right==NULL)){
                // remove and return
                if (rover_prev->left == rover){
                    rover_prev->left = NULL;
                } else {
                    rover_prev ->right = NULL;
                }
                mydata_t *return_ptr = rover->data_ptr;
                
                free(rover);
                rover = NULL;
                tree->tree_size--;
                if (tree->tree_size == 0){
                    tree->root = NULL;
                }
                return return_ptr;                
            } else {
                rover_del = rover;
                if (rover->left != NULL){
                    // go down left branch once, find max, and replace
                    rover_prev = rover;
                    rover = rover->left;
                    if (rover->right != NULL){
                        while (rover->right != NULL){
                            rover_prev = rover;
                            rover = rover->right;
                        }
                        rover_prev->right = rover->left;
                    } else {
                        if (rover->left != NULL){
                            rover_prev->left = rover->left;
                        } else {
                            rover_prev->left = rover->right;
                        }
                    }
                } else {
                    // go down right branch once, find min, and replace
                    if (rover->right != NULL){
                        rover_prev = rover;
                        rover = rover->right;
                        if (rover->left != NULL){
                            while (rover->left != NULL){
                                rover_prev = rover;
                                rover = rover->left;
                            }
                            rover_prev->left = rover->right;
                        } else {
                            if (rover->right != NULL){
                                rover_prev->right = rover->right;
                            } else {
                                rover_prev->right = rover->left;
                            }
                        }
                    } 
                }
                mydata_t *return_ptr = rover_del->data_ptr;
                
                rover_del->data_ptr = rover->data_ptr;
                rover_del->key = rover->key;
                
                free(rover);
                rover = NULL;
                tree->tree_size--;
                return return_ptr;  
            }
        }
        rover_prev = rover;
        tree->num_recent_key_comparisons++;
        if (Key > rover->key){
            rover = rover->right;
        } else {
            rover = rover->left;
        }
    } while (rover != NULL);

    return NULL;
}

/* Find the tree element with the matching key and return a pointer to the data block that is stored in
 * this node in the tree. If the key is not found in the tree then return NULL
 *
 * Inputs: tree - pointer to tree structure to search
 *         Key - Key value to find in tree, used to determine if a node already exists
 *
 * Outputs: pointer to the datapointer field of node that was found (or NULL if node wasn't found in tree)
 */
mydata_t *bst_search(tree_t *tree, tree_key_t Key){
    if (tree == NULL) return NULL;
    tree_node_t *rover = tree->root;
    tree->num_recent_key_comparisons = 0;
    do {
        tree->num_recent_key_comparisons++;
        if (Key == rover->key) return rover->data_ptr;
        tree->num_recent_key_comparisons++;
        if (Key < rover->key){
            rover = rover->left;
        } else {
            rover = rover->right;
        }
    } while (rover != NULL);

    return NULL;
}

/* Return the number of keys in the tree.
 *
 * Inputs: tree - pointer to tree structure
 *
 * Outputs: number of nodes in the tree
 */
int bst_size(tree_t *tree){
    return tree->tree_size;
}

/* Return num_recent_key_comparisons, the number of key comparisons for the most recent call
 * to bst_insert, bst_remove, or bst_search.
 *
 * Inputs: tree - pointer to tree structure to search
 *
 * Outputs: number of recent comparisons
 */
int bst_stats(tree_t *tree){
    return tree->num_recent_key_comparisons;
}

/* Return the internal path length of the tree
 *
 * Inputs: node - pointer to the root node of a BST
 *
 * Outputs: total length of all internal paths in the BST
 */
int bst_internal_path_length_count(tree_node_t *node, int level){
    if (node == NULL){
        return 0;
    }
    return (bst_internal_path_length_count(node->left, level+1) +
        bst_internal_path_length_count(node->right, level+1) + level);
}

/* Return the internal path length of the tree
 *
 * Inputs: tree - pointer to tree structure to search
 *
 * Outputs: total length of all internal paths in the BST
 */
int bst_internal_path_len(tree_t *tree){
    return (bst_internal_path_length_count(tree->root, 0)); 
}

/* Returns the height of the tree's longest branch, essentially providing the height dimension
 * used in displaying the pretty-form of the tree
 *
 * Inputs: node - pointer to the root node of a BST
 *
 * Outputs: integer # of levels in the longest branch
 */
int bst_get_max_level(tree_node_t *node){
    if (node == NULL) return 0;
    return MAX(bst_get_max_level(node->left), bst_get_max_level(node->right))+1;
}

/* Prints both a simple (rotated) representation of the tree as well as 
 * a more user-friendly, graphical version of the tree
 *
 * Inputs: tree - pointer to tree structure to search
 */
void bst_debug_print_tree(tree_t *tree){
    if (tree == NULL || tree->tree_size == 0){
        printf("Tree is empty.\n");
        return;
    }
    printf("Total nodes in tree: %d\n\n", tree->tree_size);
    bst_debug_print_tree_node(tree->root, 0);
    printf("\n------------------------------------------\n\n");

    int max_level = bst_get_max_level(tree->root);

    // create 2D array
    int **node_arr;
    node_arr = (int**)malloc(sizeof(int*)*max_level*2-1);
    for (int i=0; i<max_level*2-1; i++){
        node_arr[i] = (int*)malloc(sizeof(int)*((int)pow(2,max_level)-1));
    }

    bst_debug_print_tree_pretty_build_array(tree, node_arr);

    if (max_level>1){ 
        bst_debug_print_tree_pretty_add_tees(tree, node_arr, max_level, ((int)pow(2,max_level)-1));
        bst_debug_print_tree_pretty_add_corners(tree->root, node_arr, max_level*2-1, ((int)pow(2,max_level)-1));
        bst_debug_print_tree_pretty_add_bars(node_arr, max_level*2-1, ((int)pow(2,max_level)-1));
    }
    
    int width_final = ((int)pow(2,max_level)-1); 

    bst_debug_print_tree_pretty_trim_array(node_arr, max_level*2-1, &width_final);

    for (int i=0; i<max_level*2-1; i++){
        printf("   ");
        for (int j=0; j<width_final; j++){
            if ((node_arr)[i][j] > 0){
                if (node_arr[i][j] > 99){
                    printf("%3d", node_arr[i][j]);
                } else if (node_arr[i][j] > 9){
                    printf("%3d", node_arr[i][j]);
                } else {
                    printf(" %d ", node_arr[i][j]);
                }
            } else {
                switch (node_arr[i][j]){
                    case CONN_BAR:
                        printf("───");
                        break;
                    case CONN_LEFT_ANG:
                        printf(" ┌─");
                        break;
                    case CONN_RIGHT_ANG:
                        printf("─┐ ");
                        break;
                    case CONN_TEE:
                        printf("─┴─");
                        break;
                    default:
                        printf("   ");
                        break;
                }
            }
        }
        printf("\n");
    }
    printf("\n");

    for (int i=0; i<max_level*2-1; i++){
        free(node_arr[i]);
    }
    free(node_arr);
}


/* Builds a 2D array to be used by the pretty-print functions. This tree consists of the following:
 *      The key of each node in its correct position
 *      -1 for an empty space
 *      -2 for a bar connecting children nodes to their parents
 *      -3 for an angled connection leading into each left child
 *      -4 for an angled connection leading into each right chile
 *      -5 for a tee connector out of each parent
 *
 *  This method made it easy to build and clean up the array after populating to minimize its size and make it
 *  look cleaner but is wildly inefficient and struggles more as the height and / or width of the tree increases.
 *
 * Inputs: tree - pointer to tree structure
 *         node_arr - pointer to 2D array containing representation of current pretty tree
*/
void bst_debug_print_tree_pretty_build_array(tree_t *tree, int **node_arr){

    int max_level = bst_get_max_level(tree->root);
    for (int i=0; i<max_level*2-1; i++){
        for (int j=0; j<(int)pow(2,max_level)-1; j++){
            node_arr[i][j] = -1;
        }
    }
    int base = (int)pow(2,max_level-1)-1;
    node_arr[0][base] = tree->root->key;
    bst_debug_print_tree_pretty_add_array(tree->root->left, node_arr, base, LEFT, 1, max_level);
    bst_debug_print_tree_pretty_add_array(tree->root->right, node_arr, base, RIGHT, 1, max_level);
}


/* Adds keys of each node to the 2D array in its correct position
 *
 * Inputs: node - pointer to root node of BST
 *         node_arr - pointer to 2D array containing representation of current pretty tree
 *         base - column that the previous added added node was at, used to calculate the position of the
 *                  next node. This is based on the levels of the previous and current node
 *         l_or_r - integer representing if the node being added is on the left (-1) or right (1) of the previous node
 *         level - row that the current node should be added on
 *         max_level - height of the BST (longest branch), used in calculating the base for the next node
 */
void bst_debug_print_tree_pretty_add_array(tree_node_t *node, int **node_arr, int base, int l_or_r, int level, int max_level){
    if (node==NULL) return;
    if (l_or_r == LEFT){
        base -= (int)pow(2,max_level-level-1);
        node_arr[level][base] = node->key;
    } else {
        base += (int)pow(2,max_level-level-1);
        node_arr[level][base] = node->key;
    }
    bst_debug_print_tree_pretty_add_array(node->left, node_arr, base, LEFT, level+1, max_level);
    bst_debug_print_tree_pretty_add_array(node->right, node_arr, base, RIGHT, level+1, max_level);
}

/* Adds tee connectors coming out of each node (potential parent) to 2D array. The nodes
 * that are not parents will have their tee connectors removed at the end
 *
 * Inputs: tree - pointer to tree structure
 *         node_arr - pointer to 2D array containing representation of current pretty tree
 *         height - height of 2D array (rows)
 *         width - width of 2D array (columns)
 */
void bst_debug_print_tree_pretty_add_tees(tree_t *tree, int **node_arr, int height, int width){
    // add blank lines between each existing line where we will draw connectors
    for (int i=height-1; i>0; i--){
            for (int j=0; j<width; j++){
                node_arr[i*2][j] = node_arr[i][j];
                node_arr[i][j] = EMPTY_SPACE;
            }
    }
    // find nodes and add a tee below each node
    for (int i=0; i<(height-1)*2; i++){
        for (int j=0; j<width; j++){
            if (node_arr[i][j] > 0){
                node_arr[i+1][j] = CONN_TEE;
            }
        }
    }
}

/* Adds angled connectors going into each child to the 2D array.
 *
 * Inputs: tree - pointer to tree structure
 *         node_arr - pointer to 2D array containing representation of current pretty tree
 *         height - height of 2D array (rows)
 *         width - width of 2D array (columns)
 */
void bst_debug_print_tree_pretty_add_corners(tree_node_t *node, int **node_arr, int height, int width){
    if (node==NULL) return;
    // search 2D array for left child and add left connector
    if(node->left != NULL){
        for (int i=height-1; i>0; i--){
            for (int j=0; j<width; j++){
                if (node_arr[i][j] == node->left->key){
                    node_arr[i-1][j] = CONN_LEFT_ANG;
                }
            }
        }   
    }
    // search 2D array for right child and add right connector
    if(node->right != NULL){
        for (int i=height-1; i>0; i--){
            for (int j=0; j<width; j++){
                if (node_arr[i][j] == node->right->key){
                    node_arr[i-1][j] = CONN_RIGHT_ANG;
                }
            }
        }   
    }
    bst_debug_print_tree_pretty_add_corners(node->left, node_arr, height, width);
    bst_debug_print_tree_pretty_add_corners(node->right, node_arr, height, width);
}

/* Adds horizontal bars connecting each parent to its children to the 2D array
 *
 * Inputs: node_arr - pointer to 2D array containing representation of current pretty tree
 *         height - height of 2D array (rows)
 *         width - width of 2D array (columns)
 */
void bst_debug_print_tree_pretty_add_bars(int **node_arr, int height, int width){
    // search 2D array for left angled connector
    for (int i=height-1; i>0; i--){
        for (int j=0; j<width; j++){
            if (node_arr[i][j] == CONN_LEFT_ANG){
                j++;
                // connect angled connector to parent tee
                while(node_arr[i][j] != CONN_TEE){
                    node_arr[i][j] = CONN_BAR;
                    j++;
                }
            }
        }
    }
    // search 2D array for right angled connector
    for (int i=height-1; i>0; i--){
        for (int j=width-1; j>=0; j--){
            if (node_arr[i][j] == CONN_RIGHT_ANG){
                j--;
                // connect angled connector to parent tee
                while(node_arr[i][j] != CONN_TEE){
                    node_arr[i][j] = CONN_BAR;
                    j--;
                }
            }
        }
    }
}

/* Cleans up the 2D array after everything has been added. This scans each column and determines if a key exists,
 * and removes those columns where no key exists. This essentially cuts out unnecessary dead space.
 *
 * Inputs: node_arr - pointer to 2D array containing representation of current pretty tree
 *         height - height of 2D array (rows)
 *         width - width of 2D array (columns)
 */
void bst_debug_print_tree_pretty_trim_array(int **node_arr, int height, int *width){
    for (int col=0; col<(*width); col++){
        for (int row=0; row<height; row++){
            // search columns for node
            if (node_arr[row][col] >= 0){
                // dont check if the 2D array is too small
                if (*width>1 && row<height-1){
                    // check first column for disconnected tees 
                    if (col == 0){
                        if (node_arr[row+1][col+1] == EMPTY_SPACE){
                            // remove tee
                            node_arr[row+1][col] = EMPTY_SPACE;
                        }
                    // check last column for disconnected tees
                    } else if (col == (*width)){
                        if (node_arr[row+1][col-1] == EMPTY_SPACE){
                            // remove tee
                            node_arr[row+1][col] = EMPTY_SPACE;
                        }
                    // check all other columns for disconnected tees
                    } else {
                        if (node_arr[row+1][col-1] == EMPTY_SPACE && node_arr[row+1][col+1] == EMPTY_SPACE){
                            // remove tee
                            node_arr[row+1][col] = EMPTY_SPACE;
                        }
                    }
                }
                // if a node is found, move on to the next column
                break;
            }
            // no node was found, begin removing that column once the last row is reached
            if (row == height-1){
                // copy all columns left 1 space
                for (int col2=col; col2<(*width)-1; col2++){
                    for (int row2=0; row2<height; row2++){
                        node_arr[row2][col2] = node_arr[row2][col2+1];
                    }
                }
                // populate empty space into the last column
                for (int col2=(*width)-1;col2<=(*width)-1; col2++){
                    for (int row2=0; row2<height; row2++){
                        node_arr[row2][col2] = EMPTY_SPACE;
                    }
                }
                // decrement the width so future iterations dont check known empty space
                (*width)--;
                // need to recheck the current column as it has new, unchecked values
                col--;
            }
        }
    }
}

/* This prints a basic, CCW rotated version of the tree without any graphical connections
 * between parents and children
 *
 * Inputs: node - pointer to root node of BST
 *         level - level of current node, used to insert enough spaces
 */
void bst_debug_print_tree_node(tree_node_t *node, int level){
    if (node == NULL) return;
    bst_debug_print_tree_node(node->right, level+1);
    printf("%*s%d\n", level*5, "", node->key);
    bst_debug_print_tree_node(node->left, level+1);
}

/* Rudimentary tree validation function that recursively checks if each parent is greater than
 * its left child and greater than its right child
 *
 * Inputs: node - pointer to root node of BST
 *         min - lowest value encountered in the current branch
 *         max - highest value encountered in the current branch
 *         count - pointer to integer that records number of nodes encountered
 *
 * Outputs: FALSE - nodes are out of order
 *          TRUE - nodes are in order
 */
int bst_debug_validate_rec(tree_node_t *node, int min, int max, int *count){
    if (node == NULL) return TRUE;
    if (node->key <= min || node->key >= max) return FALSE;
    assert (node->data_ptr != NULL);
    *count += 1;
    return bst_debug_validate_rec(node->left, min, node->key, count) &&
        bst_debug_validate_rec(node->right, node->key, max, count);
}

/* This is the main tree validation function
 *
 * Inputs: tree - pointer to tree structure
 */
void bst_debug_validate(tree_t *tree){
    int size=0;
    assert(bst_debug_validate_rec(tree->root, INT_MIN, INT_MAX, &size) == TRUE);
    assert(size == tree->tree_size);
    printf("Tree validation passed\n");
}

