#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "dumb.h"
#include "utils.h"
#include "logger/liblogger.h"

static const char* const HTML_INTRO_ =
    "\n<!DOCTYPE html>\n"
    "<html lang='en'>\n"
        "<head>\n"
            "<meta charset='UTF-8'>\n"
            // "<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n"
            // "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
            "<title>MEGA MEGA MEGA DUMB</title>\n"
        "</head>\n"
        "<body>\n"
            "<pre>\n";


#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* tree_dumb_strerror(const enum TreeDumbError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(TREE_DUMB_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(TREE_DUMB_ERROR_FAILURE);
    default:
        return "UNKNOWN_TREE_DUMB_ERROR";
    }
    return "UNKNOWN_TREE_DUMB_ERROR";
}
#undef CASE_ENUM_TO_STRING_



static struct 
{
    char* out_name;

    char* html_name;
    char* dot_name;
    char* svg_name;
    char* graph_count_name;

    FILE* dot_file;
    FILE* html_file;

    size_t graph_count;
} DUMBER_ = {};
#define MAX_FILENAME_SIZE_ 256

static void DUMBER_is_init_lasserts_(void)
{
    lassert(DUMBER_.html_name,          "DUMBER_ is not init");
    lassert(DUMBER_.html_file,          "DUMBER_ is not init");
    lassert(DUMBER_.dot_name,           "DUMBER_ is not init");
    lassert(DUMBER_.dot_file,           "DUMBER_ is not init");
    lassert(DUMBER_.svg_name,           "DUMBER_ is not init");
    lassert(DUMBER_.graph_count_name,   "DUMBER_ is not init");
}


enum TreeDumbError tree_dumb_ctor(void)
{
    lassert(!DUMBER_.html_name         || !DUMBER_.html_file, "");
    lassert(!DUMBER_.dot_name          || !DUMBER_.dot_file,  "");
    lassert(!DUMBER_.svg_name,                                "");
    lassert(!DUMBER_.graph_count_name,                        "");
    lassert(!DUMBER_.out_name,                                "");

    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_file("./log/dumb"));

    return TREE_DUMB_ERROR_SUCCESS;
}

enum TreeDumbError set_graph_count_          (void);
enum TreeDumbError write_graph_count_in_file_(void);

enum TreeDumbError tree_dumb_dtor(void)
{
    DUMBER_is_init_lasserts_();

    TREE_DUMB_ERROR_HANDLE(write_graph_count_in_file_());

    if (fclose(DUMBER_.html_file))
    {
        perror("Can't close html_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't close dot_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    free(DUMBER_.out_name);         IF_DEBUG(DUMBER_.out_name           = NULL;)
    free(DUMBER_.html_name);        IF_DEBUG(DUMBER_.html_name          = NULL;)
    free(DUMBER_.dot_name);         IF_DEBUG(DUMBER_.dot_name           = NULL;)
    free(DUMBER_.svg_name);         IF_DEBUG(DUMBER_.svg_name           = NULL;)
    free(DUMBER_.graph_count_name); IF_DEBUG(DUMBER_.graph_count_name   = NULL;)

    return TREE_DUMB_ERROR_SUCCESS;
}

//==========================================================================================

static bool is_set_graph_count_ = false;
enum TreeDumbError set_graph_count_(void) //NOTE - non assertable
{
    is_set_graph_count_ = true;

    if (access(DUMBER_.graph_count_name, F_OK))
    {
        errno = 0;
        DUMBER_.graph_count = 0;
        return TREE_DUMB_ERROR_SUCCESS;
    }

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "rb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (fscanf(graph_count_file, "%zu", &DUMBER_.graph_count) <= 0)
    {
        perror("Can't fscanf graph_count");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    return TREE_DUMB_ERROR_SUCCESS;
}

enum TreeDumbError write_graph_count_in_file_(void)
{
    DUMBER_is_init_lasserts_();

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "wb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (fprintf(graph_count_file, "%zu", DUMBER_.graph_count) <= 0)
    {
        perror("Can't fprintf graph_count");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return TREE_DUMB_ERROR_FAILURE;
    }

    return TREE_DUMB_ERROR_SUCCESS;
}

//==========================================================================================

enum TreeDumbError tree_dumb_set_out_file_(char*  const filename, FILE** const file, 
                                           char** const old_filename, const char* const mode,
                                           const char*  const file_extension);

enum TreeDumbError tree_dumb_set_out_filename_(char*  const filename, 
                                               const char*  const file_extension,
                                               char** const old_filename);

enum TreeDumbError tree_dumb_set_out_file(char* const filename)
{
    lassert(filename, "");

    TREE_DUMB_ERROR_HANDLE(
        tree_dumb_set_out_file_(filename, &DUMBER_.html_file, &DUMBER_.html_name, "ab", ".html")
    );
    TREE_DUMB_ERROR_HANDLE(
        tree_dumb_set_out_file_(filename, &DUMBER_.dot_file,  &DUMBER_.dot_name,  "wb", ".dot")
    );

    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_filename_(filename, "",     &DUMBER_.out_name));
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_filename_(filename, ".svg", &DUMBER_.svg_name));
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_filename_(filename, "_graph_count.txt", 
                                                       &DUMBER_.graph_count_name));

    TREE_DUMB_ERROR_HANDLE(set_graph_count_());

    return TREE_DUMB_ERROR_SUCCESS;
}

enum TreeDumbError tree_dumb_set_out_file_(char*  const filename, FILE** const file, 
                                           char** const old_filename, const char* const mode,
                                           const char*  const file_extension)
{
    lassert(filename, "");
    lassert(file, "");
    lassert(old_filename, "");
    lassert(file_extension, "");

    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_filename_(filename, file_extension, old_filename));

    if (*file && fclose(*file))
    {  
        perror("Can't close file");
        return TREE_DUMB_ERROR_FAILURE;
    }
    
    if (!(*file = fopen(*old_filename, mode))){
        perror("Can't open file");
        return TREE_DUMB_ERROR_FAILURE;
    }
    
    return TREE_DUMB_ERROR_SUCCESS;
}


enum TreeDumbError tree_dumb_set_out_filename_(char*  const filename, 
                                               const char*  const file_extension,
                                               char** const old_filename)
{
    lassert(filename, "");
    lassert(file_extension, "");

    free(*old_filename);

    *old_filename = calloc(MAX_FILENAME_SIZE_, sizeof(char));

    if (!*old_filename)
    {
        perror("Can't calloc old_filename");
        return TREE_DUMB_ERROR_FAILURE;
    }

    if (snprintf(*old_filename, MAX_FILENAME_SIZE_, "%s%s", filename, file_extension) <= 0)
    {
        perror("Can't snprintf old_filename");
        return TREE_DUMB_ERROR_FAILURE;
    }

    return TREE_DUMB_ERROR_SUCCESS;
}

//==========================================================================================

static const char* handle_invalid_ptr_(const void* const check_ptr);


int create_tree_dot_(const tree_t* const tree, const elem_to_str_t elem_to_str);
int create_tree_svg_(void);
int insert_tree_svg_(void);

#define DUMB_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf(DUMBER_.html_file, format, ##__VA_ARGS__);                                      \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)

void tree_dumb (const tree_t* const tree, elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    if (is_empty_file(DUMBER_.html_file) == 0) fprintf(DUMBER_.html_file, HTML_INTRO_);

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");   

    DUMB_AND_FPRINTF_("\n==TREE DUMB==\nDate: %s\nTime: %s\n\n", __DATE__, __TIME__);

    const char* tree_buf = handle_invalid_ptr_(tree);

    if (tree_buf)
    {
        DUMB_AND_FPRINTF_("tree[%s]\n", tree_buf);
        return;
    }
    DUMB_AND_FPRINTF_("tree[%p]\n\n", tree);

    const char* tree_groot_buf = handle_invalid_ptr_(tree->Groot);

    if (tree_groot_buf)
        DUMB_AND_FPRINTF_("tree->Groot[%s]\n", tree_groot_buf);
    else
        DUMB_AND_FPRINTF_("tree->Groot[%p]\n", tree->Groot);
    
    const char* tree_compare_buf = handle_invalid_ptr_(tree->compare);

    if (tree_compare_buf)
        DUMB_AND_FPRINTF_("tree->compare[%s]\n", tree_compare_buf);
    else
        DUMB_AND_FPRINTF_("tree->compare[%p]\n", tree->compare);
    
    DUMB_AND_FPRINTF_("tree->size = %zu\n", tree->size);


    if (!is_set_graph_count_ && set_graph_count_())
    {
        fprintf(stderr, "Can't set graph_count_\n");
        return;
    }

    if (create_tree_dot_(tree, elem_to_str))
    {
        DUMB_AND_FPRINTF_("Can't create tree dot\n");
        return;
    }

    if (create_tree_svg_())
    {
        DUMB_AND_FPRINTF_("Can't create tree svg\n");
        return;
    }

    if (insert_tree_svg_())
    {
        DUMB_AND_FPRINTF_("Can't insert tree svg\n");
        return;
    }

    ++DUMBER_.graph_count;

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");
}
#undef DUMB_AND_FPRINTF_

static const char* handle_invalid_ptr_(const void* const check_ptr)
{
    switch (is_invalid_ptr(check_ptr))
    {
    case PTR_STATES_VALID:
        return NULL;
    case PTR_STATES_NULL:
        return "NULL";
    case PTR_STATES_INVALID:
        return "INVALID";
    case PTR_STATES_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

    return "MIPT SHIT";
}

static size_t node_count_ = 0;
int create_tree_dot_recursive_(const tree_node_t* const node, const size_t tree_size, 
                               const elem_to_str_t elem_to_str);

int create_tree_dot_(const tree_t* const tree, const elem_to_str_t elem_to_str)
{
    if (is_invalid_ptr(tree))          return -1;
    if (is_invalid_ptr(elem_to_str))   return -1;

    fprintf(DUMBER_.dot_file, "digraph {\n"
                              "rankdir=TB;\n");

    node_count_ = 0;
    if (create_tree_dot_recursive_(tree->Groot, tree->size, elem_to_str))
        return -1;

    fprintf(DUMBER_.dot_file, "}\n");

    return 0;
}

int create_tree_dot_recursive_(const tree_node_t* const node, const size_t tree_size, 
                               const elem_to_str_t elem_to_str)
{
    if (!node)                          return  0;
    if (is_invalid_ptr(node))           return  0;
    if (is_invalid_ptr(elem_to_str))    return  0;
    if (is_invalid_ptr(node->data))     return  0;
    if (!node->size)                    return  0;
    if (node_count_ >= tree_size)       return  0;

    ++node_count_;

    const size_t data_str_size = 4 * node->size;
    char* data_str = calloc(data_str_size, sizeof(char));

    if (data_str && !elem_to_str(node->data, node->size, &data_str, data_str_size)) 
    {
        fprintf(DUMBER_.dot_file, 
                "node%zu [shape=Mrecord; label = \" %zu | data = %s\"];\n",
                node_count_, node_count_, data_str);
    }

    free(data_str); data_str = NULL;

    const size_t parent_num = node_count_;

    if (node->lt)
    {
        fprintf(DUMBER_.dot_file, "node%zu -> node%zu [color=green]\n", 
                parent_num, node_count_ + 1);
        if (create_tree_dot_recursive_(node->lt, tree_size, elem_to_str))
            return -1;
    }

    if (node->rt)
    {
        fprintf(DUMBER_.dot_file, "node%zu -> node%zu [color=red]\n", 
                parent_num, node_count_ + 1);
        if (create_tree_dot_recursive_(node->rt, tree_size, elem_to_str))
            return -1;
    }

    return 0;
}

int create_tree_svg_(void)
{
    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't fclose dot file");
        return -1;
    }

    static const size_t CREATE_SVG_CMD_SIZE = 256;
    char* create_svg_cmd = calloc(CREATE_SVG_CMD_SIZE, sizeof(char));

    if (!create_svg_cmd)
    {
        fprintf(stderr, "Can't calloc create_svg_cmd\n");
        return -1;
    }

    if (snprintf(create_svg_cmd, CREATE_SVG_CMD_SIZE, 
                 "dot -Tsvg %s -o %s%zu.svg >/dev/null", 
                 DUMBER_.dot_name, DUMBER_.out_name, DUMBER_.graph_count) <= 0)
    {
        fprintf(stderr, "Can't snprintf creare_svg_cmd\n");
        free(create_svg_cmd); create_svg_cmd = NULL;
        return -1;
    }
    
    if (system(create_svg_cmd))
    {
        fprintf(stderr, "Can't call system create svg\n");
        free(create_svg_cmd); create_svg_cmd = NULL;
        return -1;
    }

    free(create_svg_cmd); create_svg_cmd = NULL;

    if (!(DUMBER_.dot_file = fopen(DUMBER_.dot_name, "wb")))
    {
        perror("Can't fopen dot file");
        return -1;
    }

    return 0;
}

int insert_tree_svg_(void)
{
    const char* filename_without_path = DUMBER_.out_name;
    while (strchr(filename_without_path, '/') != NULL)
    {
        filename_without_path = strchr(filename_without_path, '/') + 1;
    }

    fprintf(DUMBER_.html_file, "<img src=%s%zu.svg width=500>\n", 
            filename_without_path, DUMBER_.graph_count);

    return 0;
}

#undef MAX_FILENAME_SIZE_