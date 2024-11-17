#include <stdint.h>

#include "create.h"
#include "game/verification/verification.h"
#include "flags/flags.h"
#include "tree/tree_structs.h"
#include "logger/liblogger.h"
#include "tree/tree.h"

enum GameError mode_create(flags_objs_t* const flags_objs, tree_t* const tree)
{
    lassert(!is_invalid_ptr(flags_objs), "");
    TREE_VERIFY(tree, NULL);
    
    printf(
        "Перед заполнением нового никому ненужного дерева "
            "стоит удалить прошлое никому не нужное дерево?\n"
        "Ответы 1 или 0, думаю, очевидно.\n");

    uint32_t temp_ans = 3;
    if (scanf("%u", &temp_ans) != 1)
    {
        perror("Can't scanf temp_ans");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    if (temp_ans != 1 && temp_ans != 0)
    {
        printf("Ой, иди ты в пизду, вернусь с ошибкой");
        return GAME_ERROR_TI_PIDOR;
    }

    if (temp_ans)
        tree_dtor(tree);
    else
    {
        printf(ITALIC_TEXT("Ссыкло...\n"));
        return GAME_ERROR_SUCCESS;
    }
    
    TREE_ERROR_HANDLE_(tree_ctor(tree, NULL));
    TREE_ERROR_HANDLE_(fill_tree_from_file(tree, flags_objs->in_filename));

    return GAME_ERROR_SUCCESS;
}