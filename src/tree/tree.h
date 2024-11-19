#ifndef AKINATOR_SRC_TREE_TREE_H
#define AKINATOR_SRC_TREE_TREE_H

#include <stdio.h>

#include "tree_structs.h"
#include "verification/verification.h"
#include "utils/utils.h"
#include "node/node.h"


enum TreeError tree_ctor          (tree_t* const tree, const tree_compare_t compare);
void           tree_dtor          (tree_t* const tree);

enum TreeError fill_tree_from_file(tree_t* const tree, const char* const input_filename);

enum TreeError tree_insert(tree_t* const tree, tree_node_t* const tree_node);

tree_node_t*   tree_find_node(const tree_t* const tree, const void* const data, const size_t size);

enum TreeError tree_print_inorder (FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str);
enum TreeError tree_print_preorder(FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str);


#endif /*AKINATOR_SRC_TREE_TREE_H*/