#ifndef AKINATOR_SRC_TREE_TREE_H
#define AKINATOR_SRC_TREE_TREE_H

#include <stdio.h>

#include "tree_structs.h"
#include "../verification/verification.h"
#include "utils.h"


tree_node_t* tree_node_ctor(const void* const data, const size_t size);
void         tree_node_dtor(tree_node_t* const tree_node);


enum TreeError tree_ctor(tree_t* const tree, const tree_compare_t compare);
void           tree_dtor(tree_t* const tree);

enum TreeError tree_insert(tree_t* const tree, tree_node_t* const tree_node);

enum TreeError tree_print(FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str);

#endif /*AKINATOR_SRC_TREE_TREE_H*/