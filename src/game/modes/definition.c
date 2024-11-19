#include <stdbool.h>

#include "definition.h"
#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree.h"
#include "logger/liblogger.h"
#include "stack_on_array/libstack.h"

enum GameError mode_definition(tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);

    printf("\nСам определил и сам не запомнил) Ну пиши, как там твоя хуйня называется\n");

    char node_data[NODE_DATA_MAX_SIZE] = {};
    if (scanf("\n%[^\n]", node_data) != 1)
    {
        perror("Can't scanf node_data");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    stack_key_t stack = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack, NODE_DATA_MAX_SIZE, 0));

    if (!fill_def_stack(tree->Groot, node_data, stack))
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

bool fill_def_stack(const tree_node_t* const node, const void* const data, stack_key_t stack)
{
    if (node == NULL) return NULL;
    
    lassert(!is_invalid_ptr(node), "");
    lassert(!is_invalid_ptr(data), "");
    lassert(!is_invalid_ptr(node->data), "");

    if (strcmp(node->data, data) == 0)
        return true;

    bool result_lt = fill_def_stack(node->lt, data, stack); 
    bool result_rt = false;
    if (!result_lt)
        result_rt = fill_def_stack(node->rt, data, stack);
    
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