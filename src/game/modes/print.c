#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "print.h"
#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "verification/verification.h"

enum OutputMode
{
    OUTPUT_MODE_AGAIN    = 0,
    OUTPUT_MODE_INORDER  = 1,
    OUTPUT_MODE_PREORDER = 2,
    OUTPUT_MODE_DUMB     = 3,
};
static_assert(OUTPUT_MODE_AGAIN == 0);
static_assert(sizeof(enum OutputMode) == sizeof(uint32_t));

enum GameError mode_print(flags_objs_t* const flags_objs, const tree_t* const tree)
{
    lassert(!is_invalid_ptr(flags_objs), "");
    TREE_VERIFY(tree, NULL);
    lassert(tree->size, "");

    enum OutputMode output_mode = OUTPUT_MODE_AGAIN;

    while (!output_mode)
    {
        printf(
            "\n"
            BOLD_TEXT("%-5u.") " Inorder со скобочками. Для компактных пусек, как ты\n"
            BOLD_TEXT("%-5u.") " Preorder в виде базы данных."
                               " Для настоящих любителей жести (не для слабонервныч)\n"
            BOLD_TEXT("%-5u.") " ЧТО ПРОИСХОДИИИИИИТ?!?!?! + картинка(опционально)\n"
            "Как будем выводить?\n",
            (uint32_t)OUTPUT_MODE_INORDER,
            (uint32_t)OUTPUT_MODE_PREORDER,
            (uint32_t)OUTPUT_MODE_DUMB
        );

        if (scanf("%u", (uint32_t*)&output_mode) != 1)
        {
            perror("Can't scanf output_mode");
            return GAME_ERROR_STANDARD_ERRNO;
        }

        switch(output_mode)
        {
            case OUTPUT_MODE_INORDER:
            {
                TREE_ERROR_HANDLE_(tree_print_inorder(flags_objs->out_file, tree, NULL));
                break;
            }
            case OUTPUT_MODE_PREORDER:
            {
                TREE_ERROR_HANDLE_(tree_print_preorder(flags_objs->out_file, tree, NULL));
                break;
            }
            case OUTPUT_MODE_DUMB:
            {
                tree_dumb(tree, NULL);
                break;
            }

            case OUTPUT_MODE_AGAIN:
            default:
            {
                printf("Выбирай заново, хули. В этот раз " RED_TEXT("ЧИТАТЬ") " будешь?\n");
                output_mode = OUTPUT_MODE_AGAIN;
                break;
            }
        }
    }
    
    return GAME_ERROR_SUCCESS;
}