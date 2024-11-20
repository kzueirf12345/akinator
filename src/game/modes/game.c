#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "game.h"
#include "game/verification/verification.h"
#include "tree/tree.h"

//TODO переписать на нерекурсивный алгоритм, потому что надежнее и быстрее

enum GameError answer_handle_      (tree_node_t** node, tree_t* const tree);
enum GameError question_handle_    (tree_node_t** cur_node);

enum GameError mode_game(tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);
    lassert(tree->size, "");
    lassert(tree->Groot, "");

    tree_node_t* prev_node = tree->Groot;
    tree_node_t* cur_node = tree->Groot;

    while (cur_node->lt)
    {
        lassert((bool)cur_node->lt == (bool)cur_node->rt, "");

        prev_node = cur_node;

        GAME_ERROR_HANDLE(question_handle_(&cur_node));
    }

    lassert(prev_node, "");

    if (cur_node == tree->Groot)
    {
        GAME_ERROR_HANDLE(answer_handle_(&tree->Groot, tree));
    }
    else if (prev_node->lt == cur_node)
    {
        GAME_ERROR_HANDLE(answer_handle_(&prev_node->lt, tree));
    }
    else
    {
        GAME_ERROR_HANDLE(answer_handle_(&prev_node->rt, tree));
    }

    
    return GAME_ERROR_SUCCESS;
}

#define ANSWER_SIZE_ 10
enum GameError question_handle_(tree_node_t** cur_node)
{
    lassert(!is_invalid_ptr(cur_node), "");
    lassert(!is_invalid_ptr(*cur_node), "");
    lassert((bool)(*cur_node)->lt == (bool)(*cur_node)->rt, "");

    char* node_data_str = calloc(NODE_DATA_MAX_SIZE, sizeof(char));
    if (data_to_str((*cur_node)->data, (*cur_node)->size, &node_data_str, NODE_DATA_MAX_SIZE))
    {
        fprintf(stderr, "Can't data_to_str");
        return GAME_ERROR_TREE;
    }

    VOICINGF("Хуйня, что ты загадал %s?\n (Ответы 'да' или 'нет')\n", node_data_str);
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
        VOICINGF("То есть ты реально не смог написать да или нет? ВАВАХВАХ, тогда зарандомлю\n");
        is_rt = rand()%2;
    }

    if (is_rt) *cur_node = (*cur_node)->rt;
    else       *cur_node = (*cur_node)->lt;

    return GAME_ERROR_SUCCESS;
}

enum GameError answer_handle_ (tree_node_t** node, tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(node), "");
    lassert(!is_invalid_ptr(*node), "");
    lassert((bool)(*node)->lt == (bool)(*node)->rt, "");

    char* node_data_str = calloc(NODE_DATA_MAX_SIZE, sizeof(char));

    if (!node_data_str)
    {
        perror("Can't calloc node_data_str");
        return GAME_ERROR_SUCCESS;
    }

    if (data_to_str((*node)->data, (*node)->size, &node_data_str, NODE_DATA_MAX_SIZE))
    {
        fprintf(stderr, "Can't data_to_str");
        return GAME_ERROR_TREE;
    }

    VOICINGF("Ты загадал '%s'? (Ответы 'да' или 'нет')\n", node_data_str);

    if (sleep(2) != 0)
    {
        perror("I can't sleep...");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
        return GAME_ERROR_STANDARD_ERRNO;
    }
    VOICINGF("Сосал?\n");

    char answer[ANSWER_SIZE_] = {};
    if (scanf("%s", answer) != 1)
    {
        perror("Can't scanf answer");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
        return GAME_ERROR_STANDARD_ERRNO;
    }

    int is_correct = (strcmp(to_lower(answer), "да")  != 0 
                    ?  strcmp(to_lower(answer), "нет") != 0 ? -1 : 0
                    :  1);

    if (is_correct == -1)
    {
        VOICINGF("Не умеешь писать - сосёшь бибу\n");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
        return GAME_ERROR_SUCCESS;
    }

    if (is_correct)
    {
        VOICINGF("Очев + банально + очев. Я знал, ХАХ! Ты лох\n");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
        return GAME_ERROR_SUCCESS;
    }
    else
    {
        VOICINGF("Пиздец, ну и что ты нагадал?\n");

        char answer_node_data[NODE_DATA_MAX_SIZE] = {};
        if (scanf("\n%[^\n]", answer_node_data) != 1)
        {
            perror("Can't scanf answer_node_data");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
            return GAME_ERROR_STANDARD_ERRNO;
        }

        VOICINGF("И чем же отличается '%s' от '%s'?\n", answer_node_data, node_data_str);

        char question_node_data[NODE_DATA_MAX_SIZE] = {};
        if (scanf("\n%[^\n]", question_node_data) != 1)
        {
            perror("Can't scanf answer_node_data");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
            return GAME_ERROR_STANDARD_ERRNO;
        }

        tree_node_t* answer_node = tree_node_ctor(answer_node_data, NODE_DATA_MAX_SIZE);
        if (!answer_node)
        {
            fprintf(stderr, "Can't ctor answer_node\n");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
            return GAME_ERROR_TREE;
        }

        tree_node_t* question_node = tree_node_ctor(question_node_data, NODE_DATA_MAX_SIZE);
        if (!question_node)
        {
            fprintf(stderr, "Can't ctor question_node\n");
                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)
            return GAME_ERROR_TREE;
        }

        question_node->lt = *node;
        question_node->rt = answer_node;
        *node = question_node;

        tree->size += 2;
    }

                                                free(node_data_str); IF_DEBUG(node_data_str = NULL;)

    return GAME_ERROR_SUCCESS;
}
#undef NODE_DATA_SIZE_
#undef ANSWER_SIZE_