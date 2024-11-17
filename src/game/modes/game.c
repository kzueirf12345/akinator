#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "game.h"
#include "game/verification/verification.h"
#include "tree/tree.h"

enum GameError mode_game_recursive_(tree_node_t** node, bool* const do_add_size);
enum GameError answer_handle_      (tree_node_t** node, bool* const do_add_size);
enum GameError question_handle_    (tree_node_t** node, bool* const do_add_size);

enum GameError mode_game(tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);
    lassert(tree->size, "");

    bool do_add_size = false;

    GAME_ERROR_HANDLE(mode_game_recursive_(&tree->Groot, &do_add_size));

    if (do_add_size)
        tree->size += 2;
    
    return GAME_ERROR_SUCCESS;
}

enum GameError mode_game_recursive_(tree_node_t** node, bool* const do_add_size)
{
    lassert(!is_invalid_ptr(*node), "");
    lassert((bool)(*node)->lt == (bool)(*node)->rt, "");

    if ((*node)->lt) GAME_ERROR_HANDLE(question_handle_(node, do_add_size));
    else             GAME_ERROR_HANDLE(answer_handle_  (node, do_add_size));

    return GAME_ERROR_SUCCESS;
}

#define NODE_DATA_SIZE_ 256
#define ANSWER_SIZE_ 10
enum GameError question_handle_(tree_node_t** node, bool* const do_add_size)
{
    lassert(!is_invalid_ptr(*node), "");
    lassert((bool)(*node)->lt == (bool)(*node)->rt, "");

    char* node_data_str = calloc(NODE_DATA_SIZE_, sizeof(char));
    if (data_to_str((*node)->data, (*node)->size, &node_data_str, NODE_DATA_SIZE_))
    {
        fprintf(stderr, "Can't data_to_str");
        return GAME_ERROR_TREE;
    }

    printf("Хуйня, что ты загадал %s?\n (Ответы 'да' или 'нет')\n", node_data_str);
    free(node_data_str); IF_DEBUG(node_data_str = NULL;)

    char answer[ANSWER_SIZE_] = {};
    if (scanf("%s", answer) != 1)
    {
        perror("Can't scanf answer");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    int is_rt = (strcmp(to_lower(answer), "да")  != 0 
              ?  strcmp(to_lower(answer), "нет") != 0 ? -1 : 0
              :  1);

    if (is_rt == -1)
    {
        printf("То есть ты реально не смог написать да или нет? ВАВАХВАХ, тогда зарандомлю\n");
        is_rt = rand()%2;
    }

    if (is_rt) GAME_ERROR_HANDLE(mode_game_recursive_(&(*node)->rt, do_add_size));
    else       GAME_ERROR_HANDLE(mode_game_recursive_(&(*node)->lt, do_add_size));

    return GAME_ERROR_SUCCESS;
}

enum GameError answer_handle_(tree_node_t** node, bool* const do_add_size)
{
    lassert(!is_invalid_ptr(*node), "");
    lassert((bool)(*node)->lt == (bool)(*node)->rt, "");

    char* node_data_str = calloc(NODE_DATA_SIZE_, sizeof(char));
    if (data_to_str((*node)->data, (*node)->size, &node_data_str, NODE_DATA_SIZE_))
    {
        fprintf(stderr, "Can't data_to_str");
        return GAME_ERROR_TREE;
    }

    fprintf(stdout, "Ты загадал '%s'? (Ответы 'да' или 'нет')\n", node_data_str);

    if (sleep(2) != 0)
    {
        perror("I can't sleep...");
        return GAME_ERROR_STANDARD_ERRNO;
    }
    fprintf(stdout, "Сосал?\n");

    char answer[ANSWER_SIZE_] = {};
    if (scanf("%s", answer) != 1)
    {
        perror("Can't scanf answer");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    int is_correct = (strcmp(to_lower(answer), "да")  != 0 
                    ?  strcmp(to_lower(answer), "нет") != 0 ? -1 : 0
                    :  1);

    if (is_correct == -1)
    {
        printf("Не умеешь писать - сосёшь бибу\n");
        return GAME_ERROR_SUCCESS;
    }

    if (is_correct)
    {
        printf("Очев + банально + очев. Я знал, ХАХ! Ты лох\n");
        return GAME_ERROR_SUCCESS;
    }
    else
    {
        printf("Пиздец, ну и что ты нагадал?\n");

        char answer_node_data[NODE_DATA_SIZE_] = {};
        if (scanf("\n%[^\n]", answer_node_data) != 1)
        {
            perror("Can't scanf answer_node_data");
            return GAME_ERROR_STANDARD_ERRNO;
        }

        printf("И чем же отличается '%s' от '%s'?\n", answer_node_data, node_data_str);

        char question_node_data[NODE_DATA_SIZE_] = {};
        if (scanf("\n%[^\n]", question_node_data) != 1)
        {
            perror("Can't scanf answer_node_data");
            return GAME_ERROR_STANDARD_ERRNO;
        }

        tree_node_t* answer_node = tree_node_ctor(answer_node_data, NODE_DATA_SIZE_);
        if (!answer_node)
        {
            fprintf(stderr, "Can't ctor answer_node\n");
            return GAME_ERROR_TREE;
        }

        tree_node_t* question_node = tree_node_ctor(question_node_data, NODE_DATA_SIZE_);
        if (!question_node)
        {
            fprintf(stderr, "Can't ctor question_node\n");
            return GAME_ERROR_TREE;
        }

        question_node->lt = *node;
        question_node->rt = answer_node;
        *node = question_node;

        *do_add_size = true;
    }

    free(node_data_str); IF_DEBUG(node_data_str = NULL;)

    return GAME_ERROR_SUCCESS;
}
#undef NODE_DATA_SIZE_
#undef ANSWER_SIZE_