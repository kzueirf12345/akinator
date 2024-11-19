#ifndef AKINATOR_SRC_GAME_MODES_DEFINITION_H
#define AKINATOR_SRC_GAME_MODES_DEFINITION_H

#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree_structs.h"
#include "stack_on_array/libstack.h"

enum GameError mode_definition(tree_t* const tree);

bool fill_def_stack(const tree_node_t* const node, const void* const data, stack_key_t stack);

#endif /* AKINATOR_SRC_GAME_MODES_DEFINITION_H */