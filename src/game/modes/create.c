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

    VOICINGF("А тебе случаем не надо поменять ввод? ");
    VOICINGF("(Если да, то отправь какое-нибудь правдивое целое число)\n");

    int do_switch_in = 0;

    if (scanf("%d", &do_switch_in) != 1)
    {
        perror("Can't scanf do_switch_in");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    if (!*flags_objs->in_filename || do_switch_in)
    {
        VOICINGF("Эм, а данные брать откуда?\n");
        
        if (scanf("%s", flags_objs->in_filename) != 1)
        {
            perror("Can't scanf in_filename");
            return GAME_ERROR_STANDARD_ERRNO;
        }

        if (flags_objs->in_file && fclose(flags_objs->in_file))
        {
            perror("Can't fclose in_file");
            return GAME_ERROR_STANDARD_ERRNO;
        }
    }
    
    VOICINGF(
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
        VOICINGF("Ой, иди ты в пизду, вернусь с ошибкой");
        return GAME_ERROR_TI_PIDOR;
    }

    if (temp_ans)
        tree_dtor(tree);
    else
    {
        VOICINGF(ITALIC_TEXT("Ссыкло...\n"));
        return GAME_ERROR_SUCCESS;
    }
    
    TREE_ERROR_HANDLE_(tree_ctor(tree, NULL));
    TREE_ERROR_HANDLE_(fill_tree_from_file(tree, flags_objs->in_filename));

    return GAME_ERROR_SUCCESS;
}