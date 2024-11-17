#include "definition.h"
#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree.h"
#include "logger/liblogger.h"

enum GameError mode_definition(flags_objs_t* const flags_objs, tree_t* const tree)
{
    lassert(!is_invalid_ptr(flags_objs), "");
    TREE_VERIFY(tree, NULL);

    printf("\nСам определил и сам не запомнил) Ну пиши, как там твоя хуйня называется\n");

    char node_data[NODE_DATA_MAX_SIZE] = {};
    if (scanf("\n%[^\n]", node_data) != 1)
    {
        perror("Can't scanf node_data");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    tree_node_t* finded_node = tree_find_node(tree, node_data, NODE_DATA_MAX_SIZE);
    if (!finded_node)
    {
        printf("Может не будешь всякий бред придумывать, электричество тратить попросту?\n");
        return GAME_ERROR_SUCCESS;
    }
}