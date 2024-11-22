#ifndef AKINATOR_SRC_GAME_MODES_CREATE_H
#define AKINATOR_SRC_GAME_MODES_CREATE_H

#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree_structs.h"

enum GameError mode_create(flags_objs_t* const flags_objs, tree_t* const tree);

#endif /* AKINATOR_SRC_GAME_MODES_CREATE_H */