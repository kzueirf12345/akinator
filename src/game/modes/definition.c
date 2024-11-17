#include <stdbool.h>

#include "definition.h"
#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree.h"
#include "logger/liblogger.h"
#include "stack_on_array/libstack.h"

static stack_key_t stack = 0;

bool fill_def_stack_(const tree_node_t* const node, const void* const data);

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

    STACK_ERROR_HANDLE_(STACK_CTOR(&stack, NODE_DATA_MAX_SIZE, 0));

    if (!fill_def_stack_(tree->Groot, node_data))
    {
        printf("Может не будешь всякий бред придумывать, электричество тратить попросту?\n");
        stack_dtor(&stack);
        return GAME_ERROR_SUCCESS;
    }

    printf("Определение '%s':\n", node_data);
    while (!stack_is_empty(stack))
    {
        STACK_ERROR_HANDLE_(stack_pop(&stack, node_data));
        puts(node_data);
    }

    stack_dtor(&stack);
    return GAME_ERROR_SUCCESS;
}

bool fill_def_stack_(const tree_node_t* const node, const void* const data)
{
    if (node == NULL) return NULL;
    
    lassert(!is_invalid_ptr(node), "");
    lassert(!is_invalid_ptr(data), "");

    if (memcmp(node->data, data, NODE_DATA_MAX_SIZE) == 0)
        return true;

    bool result_lt = fill_def_stack_(node->lt, data); 
    bool result_rt = false;
    if (!result_lt)
        result_rt = fill_def_stack_(node->rt, data);
    
    lassert(!(result_lt && result_rt), "");
    bool result = result_lt ^ result_rt;

    if (result)
    {
        char thesis[NODE_DATA_MAX_SIZE] = {};
        if (result_lt && !strncat(thesis, "Не ", NODE_DATA_MAX_SIZE))
        {
            perror("Can't strncat 'не ' to thesis");
            return GAME_ERROR_STANDARD_ERRNO;
        }
        if (!strncat(thesis, node->data, NODE_DATA_MAX_SIZE))
        {
            perror("Can't strncat node->data to thesis\n");
            return GAME_ERROR_STANDARD_ERRNO;
        }
        STACK_ERROR_HANDLE_(stack_push(&stack, thesis));
    }
    
    return result;
}