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
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack, sizeof(bool), 0));

    if (!fill_def_stack(tree->Groot, node_data, stack))
    {
        printf("Может не будешь всякий бред придумывать, электричество тратить попросту?\n");
                                                                                 stack_dtor(&stack);
        return GAME_ERROR_SUCCESS;
    }

    if (stack_is_empty(stack))
    {
                                                                                 stack_dtor(&stack);
        return GAME_ERROR_SUCCESS;
    }

    printf("Определение '%s':\n", node_data);

    tree_node_t* cur_node = tree->Groot;

    while (!stack_is_empty(stack))
    {
        GAME_ERROR_HANDLE(thesis_handle(stack, &cur_node, node_data, NULL),    stack_dtor(&stack););
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
    const bool result = result_lt ^ result_rt;

    if (result)
    {
        STACK_ERROR_HANDLE_(stack_push(&stack, &result_rt));
    }
    return result;
}

enum GameError thesis_handle(stack_key_t stack, tree_node_t** const node, char* node_data, 
                             bool* const stack_back_elem)
{
    lassert(stack, "");
    lassert(!is_invalid_ptr(node), "");
    lassert(!is_invalid_ptr(*node), "");
    lassert(!stack_is_empty(stack), "");

    bool temp_back_elem = false;

    STACK_ERROR_HANDLE_(stack_pop(&stack, &temp_back_elem));

    if (snprintf(node_data, NODE_DATA_MAX_SIZE, "%s%s", 
                    (temp_back_elem ? "" : "не "), (char*)(*node)->data) <= 0)
    {
        perror("Can't snprintf thesis on node_data");
        return GAME_ERROR_STANDARD_ERRNO;
    }
    *node = (temp_back_elem ? (*node)->rt : (*node)->lt);

    if (!is_invalid_ptr(stack_back_elem)) *stack_back_elem = temp_back_elem;

    return GAME_ERROR_SUCCESS;
}