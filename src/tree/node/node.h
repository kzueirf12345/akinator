#ifndef AKINATOR_SRC_TREE_NODE_NODE_H
#define AKINATOR_SRC_TREE_NODE_NODE_H

#include <stdio.h>

#include "tree/tree_structs.h"


tree_node_t* tree_node_ctor(const void* const data, const size_t size);
void         tree_node_dtor(tree_node_t* const tree_node);

#endif /* AKINATOR_SRC_TREE_NODE_NODE_H */