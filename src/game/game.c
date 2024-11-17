#include <assert.h>
#include <stdint.h>

#include "game.h"
#include "logger/liblogger.h"
#include "tree/tree.h"

#include "modes/test.h"
#include "modes/print.h"
#include "modes/create.h"
#include "modes/game.h"

enum Mode
{
    MODE_END            = 0,
    MODE_GAME           = 1,
    MODE_DEFINITION     = 2,
    MODE_COMPARE        = 3,
    MODE_CREATE         = 4,
    MODE_PRINT          = 5,
    MODE_TEST           = 1488,
};
static_assert(MODE_END == 0);
static_assert(sizeof(enum Mode) == sizeof(uint32_t));


enum GameError init_tree_(tree_t* const tree);
enum GameError choose_mode_(enum Mode* const mode);

enum GameError do_akinator(flags_objs_t* const flags_objs)
{
    lassert(flags_objs, "");

    tree_t tree = {};
    GAME_ERROR_HANDLE(init_tree_(&tree));

    enum Mode mode = MODE_GAME;

    while (mode)
    {
        GAME_ERROR_HANDLE(choose_mode_(&mode),
                                                                                 tree_dtor(&tree););
        switch(mode)
        {
            case MODE_END:
            {
                printf(RED_TEXT(
                    "\nНУ И СЪЁБЫВАЙ!!!\n"
                    "Донаты: 1423 5234 7543 6345\n"
                ));
                break;
            }
            case MODE_GAME:
            {
                GAME_ERROR_HANDLE(mode_game(&tree),
                                                                                 tree_dtor(&tree););
                break;
            }
            case MODE_DEFINITION:
            {
                GAME_ERROR_HANDLE(mode_definition(flags_objs, &tree),
                                                                                 tree_dtor(&tree););
                break;
            }
            case MODE_COMPARE:
            {
                break;
            }
            case MODE_CREATE:
            {
                GAME_ERROR_HANDLE(mode_create(flags_objs, &tree),
                                                                                 tree_dtor(&tree););
                break;
            }
            case MODE_PRINT:
            {
                GAME_ERROR_HANDLE(mode_print(flags_objs, &tree),
                                                                                 tree_dtor(&tree););
                break;
            }
            case MODE_TEST:
            {
                GAME_ERROR_HANDLE(mode_test(flags_objs),
                                                                                 tree_dtor(&tree););
                break;
            }
            default:
                printf("Научись читать, карсак!\n");
                break;
        }

    }

                                                                                   tree_dtor(&tree);

    return GAME_ERROR_SUCCESS;
}

enum GameError init_tree_(tree_t* const tree)
{
    lassert(!is_invalid_ptr(tree), "");

    TREE_ERROR_HANDLE_(tree_ctor(tree, NULL));

    tree_node_t* tree_node = tree_node_ctor("хз что", sizeof("хз что"));
    if (!tree_node)
    {
        fprintf(stderr, "Can't create tree_node\n");
                                                                                   tree_dtor(tree);
        return GAME_ERROR_TREE;
    }

    TREE_ERROR_HANDLE_(tree_insert(tree, tree_node), 
                                                       tree_dtor(tree); tree_node_dtor(tree_node););

    return GAME_ERROR_SUCCESS;
}

enum GameError choose_mode_(enum Mode* const mode)
{
    lassert(!is_invalid_ptr(mode), "");

    printf(
        GREEN_TEXT("\nЭто программа Хуепокрыватор, приносим извенения за все оскоробления!\n\n")
        BOLD_TEXT("%-5u.")" Пойти нахуй\n"
        BOLD_TEXT("%-5u.")" Гамаем\n"
        BOLD_TEXT("%-5u.")" Определяем ориентацию\n"
        BOLD_TEXT("%-5u.")" В чём разница между дедом и квадробером?\n"
        BOLD_TEXT("%-5u.")" Загружаем твоё говно\n"
        BOLD_TEXT("%-5u.")" Выгружаем твоё говно (оно реально тебе нужно?)\n"
        BOLD_TEXT("%-5u.")" Для хуесосов (программистов)\n"
        "Укажите режим игры:\n",
        (uint32_t)MODE_END,
        (uint32_t)MODE_GAME,
        (uint32_t)MODE_DEFINITION,
        (uint32_t)MODE_COMPARE,
        (uint32_t)MODE_CREATE,
        (uint32_t)MODE_PRINT,
        (uint32_t)MODE_TEST
    );
    
    if (scanf("%u", (uint32_t*)mode) != 1)
    {
        perror("Can't scanf mode");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    printf("Получше ничего не придумал?\n");

    return GAME_ERROR_SUCCESS;
}