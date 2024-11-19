#include "compare.h"
#include "game/verification/verification.h"
#include "tree/tree.h"
#include "logger/liblogger.h"
#include "stack_on_array/libstack.h"
#include "definition.h"

enum GameError mode_compare(const tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);

    printf("Сравнительно похуй, но давай, вводи две хуйни\n");

    char node_data1[NODE_DATA_MAX_SIZE] = {};
    if (scanf("\n%[^\n]", node_data1) != 1)
    {
        perror("Can't scanf node_data1");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    char node_data2[NODE_DATA_MAX_SIZE] = {};
    if (scanf("\n%[^\n]", node_data2) != 1)
    {
        perror("Can't scanf node_data2");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    stack_key_t stack1 = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack1, sizeof(bool), 0));

    stack_key_t stack2 = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack2, sizeof(bool), 0),
                                                                              stack_dtor(&stack1););

    if (!fill_def_stack(tree->Groot, node_data1, stack1))
    {
        printf("Нихуя нету такого, съебал\n");
                                                            stack_dtor(&stack1);stack_dtor(&stack2);
        return GAME_ERROR_SUCCESS;
    }

    if (!fill_def_stack(tree->Groot, node_data2, stack2))
    {
        printf("Нихуя нету такого, съебал\n");
                                                            stack_dtor(&stack1);stack_dtor(&stack2);
        return GAME_ERROR_SUCCESS;
    }

    tree_node_t* cur_node1 = tree->Groot;
    tree_node_t* cur_node2 = tree->Groot;

    printf("И '%s', и '%s' имеют свойства:\n", node_data1, node_data2);
    
    while (!stack_is_empty(stack1) && !stack_is_empty(stack2))
    {
        bool stack_back_elem1 = false;
        bool stack_back_elem2 = false;

        GAME_ERROR_HANDLE(thesis_handle(stack1, &cur_node1, node_data1, &stack_back_elem1),
                                                          stack_dtor(&stack1);stack_dtor(&stack2););
        GAME_ERROR_HANDLE(thesis_handle(stack2, &cur_node2, node_data2, &stack_back_elem2),
                                                          stack_dtor(&stack1);stack_dtor(&stack2););

        if (stack_back_elem1 != stack_back_elem2)
            break;
        
        puts(node_data1);
    }

    printf("\nКлючевое различие состоит в том, что первая хуйня - %s. А вот вторая - %s\n",
            node_data1, node_data2);
    

    printf("\nПри этом у первой хуйни отличительные черты:\n");

    while (!stack_is_empty(stack1))
    {
        GAME_ERROR_HANDLE(thesis_handle(stack1, &cur_node1, node_data1, NULL),
                                                          stack_dtor(&stack1);stack_dtor(&stack2););
        puts(node_data1);
    }

    printf("\nА у второй хуйни отличительные черты:\n");

    while (!stack_is_empty(stack2))
    {
        GAME_ERROR_HANDLE(thesis_handle(stack2, &cur_node2, node_data2, NULL),
                                                          stack_dtor(&stack1);stack_dtor(&stack2););
        puts(node_data2);
    }

                                                            stack_dtor(&stack1);stack_dtor(&stack2);

    return GAME_ERROR_SUCCESS; 
}