
#include "verification.h"
#include "utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* tree_strerror(const enum TreeError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(TREE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(TREE_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(TREE_ERROR_TREE_IS_NULL);
        CASE_ENUM_TO_STRING_(TREE_ERROR_TREE_IS_INVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_COMPARE_IS_NULL);
        CASE_ENUM_TO_STRING_(TREE_ERROR_COMPARE_IS_INVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_NODE_IS_NVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_SIZE_GREATER);
        CASE_ENUM_TO_STRING_(TREE_ERROR_NODE_DATA_IS_NULL);
        CASE_ENUM_TO_STRING_(TREE_ERROR_NODE_DATA_IS_INVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_NODE_SIZE_IS_ZERO);
        CASE_ENUM_TO_STRING_(TREE_ERROR_SIZE_LESSER);
        CASE_ENUM_TO_STRING_(TREE_ERROR_UNKNOWN);
        CASE_ENUM_TO_STRING_(TREE_ERROR_ELEM_TO_STR);
        default:
            return "UNKNOWN_TREE_ERROR";
    }
    return "UNKNOWN_TREE_ERROR";
}
#undef CASE_ENUM_TO_STRING_


#ifndef NDEBUG

static size_t node_count_ = 0;
enum TreeError tree_verify_nodes_(const tree_node_t* const node, const size_t tree_size);

enum TreeError tree_verify_NOT_USE(const tree_t* const tree)
{
    switch (is_invalid_ptr(tree))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_TREE_IS_NULL;
        case PTR_STATES_INVALID:     return TREE_ERROR_TREE_IS_INVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum tree, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    switch (is_invalid_ptr(tree->compare))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_COMPARE_IS_NULL;
        case PTR_STATES_INVALID:     return TREE_ERROR_COMPARE_IS_INVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum tree->compare, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    node_count_ = 0;
    TREE_ERROR_HANDLE(tree_verify_nodes_(tree->Groot, tree->size));
     
    if (node_count_ < tree->size)
        return TREE_ERROR_SIZE_GREATER;

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_verify_nodes_(const tree_node_t* const node, const size_t tree_size)
{
    switch (is_invalid_ptr(node))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_SUCCESS;
        case PTR_STATES_INVALID:     return TREE_ERROR_NODE_IS_NVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum node, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    ++node_count_;

    if (node_count_ > tree_size)
        return TREE_ERROR_SIZE_LESSER;

    switch (is_invalid_ptr(node->data))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_NODE_DATA_IS_NULL;
        case PTR_STATES_INVALID:     return TREE_ERROR_NODE_DATA_IS_INVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum node->data, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    if (node->size == 0) 
        return TREE_ERROR_NODE_SIZE_IS_ZERO;
    
    TREE_ERROR_HANDLE(tree_verify_nodes_(node->lt, tree_size));
    TREE_ERROR_HANDLE(tree_verify_nodes_(node->rt, tree_size));

    return TREE_ERROR_SUCCESS;
}

#endif /*NDEBUG*/