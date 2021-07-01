#ifndef CNODE_H_
#define CNODE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * Pointer to a compare handler for a particular value type
 *
 * Such a function should return:
 *    -1 if 1st value < 2nd value
 *     0 if 1st value == 2nd value
 *     1 if 1st value > 2nd value
 */
typedef int (*value_compare_fun_t)(void *, void *);

/**
 * Serialization handler for a particular value type
 *
 * Such a function should serialize a value passed in the 1st argument to JSON.
 * The JSON output is written to an output buffer passed in the 2nd argument.
 * Size of the buffer should be passed in the third argument.
 * Should return the number of characters written to the buffer, without the terminating NULL.
 * Should return less than zero if error occurred.
 *
 * @warning Please follow the snprintf convention: Only when the returned number is non-negative
 *          and less than buffer size, the JSON string has been completely written.
 */
typedef int (*value_to_json_fun_t)(void *, char *, int);

/**
 * Dispose handler for a particular value type
 *
 * Should perform any necessary cleanup/memory deallocation operations on the value passed
 * in the argument.
 */
typedef void (*value_dispose_fun_t)(void *);

typedef struct
{
    value_compare_fun_t compare_fun;
    value_to_json_fun_t to_json_fun;
    value_dispose_fun_t dispose_fun;
} value_handlers_t;

/**
 * Describes a tree node for a particular value type, which is determined by the value_handlers
 * field.
 */
typedef struct node_s
{
    char *object_name;
    void *value;
    struct node_s *parent;
    struct node_s **children;
    int num_children;
    int capacity;
    value_handlers_t *value_handlers;
} node_t;

/**
 * Initialize value_handlers_t
 *
 * @param[in] handlers - pointer to a structure with handlers for a particular type of value
 * @param[in] compare_fun - pointer to a function which compares two values
 * @param[in] to_json_fun - pointer to a function which serializes a value to JSON in C string (can be NULL)
 * @param[in] dispose_fun - pointer to a function which disposes of the value (can be NULL)
 *
 * @return NULL if handlers or compare_fun is NULL, handlers otherwise
 */
value_handlers_t *value_handlers_init(
    value_handlers_t *handlers,
    value_compare_fun_t compare_fun,
    value_to_json_fun_t to_json_fun,
    value_dispose_fun_t dispose_fun);

/**
 * Allocate memory and initialize a node structure
 *
 * @param[in] value_handlers - pointer to a structure with handlers for a particular type of value
 * @param[in] node_name - name of the node being created (passed as C string)
 * @param[in] value - pointer to a value associated with the node (NULL is allowed)
 *
 * @return NULL if value_handlers or node_name is NULL, allocated and initialized node struct
 *         pointer otherwise
 *
 * @warning The new node takes ownership of the associated value and will dispose of it when
 *          node_dispose() is called
 */
node_t *node_create(
    value_handlers_t *value_handlers,
    const char *node_name,
    void *value);


/**
 * Dispose of a node structure
 *
 * @param[in] self - pointer to a node structure
 *
 * @warning All memory allocated for the node structure in node_create() is freed.
 *          If dispose_fun in value_handlers is not NULL, then the associated value will
 *          be disposed of using dispose_fun.
 */
void node_dispose(node_t *self);

/**
 * Get child of a node
 *
 * @param[in] self - pointer to a node structure
 * @param[in] child_name - NULL terminated C string with the name of the child to look for
 *
 * @return NULL if self or child_name is NULL, NULL if self does not contain a node with child_name,
 *         valid pointer to a child node structure otherwise.
 */
node_t *node_get_child(node_t *self, const char *child_name);

/**
 * Add child to a node
 *
 * @param[in] self - pointer to a node structure
 * @param[in] new_child - pointer to a node structure of the child node to be added
 *
 * @return NULL if self or new_child is NULL, NULL if self already contains a node with child_name,
 *         valid pointer to a child node structure otherwise.
 */
node_t *node_add_child(node_t *self, node_t *new_child);

/**
 * Remove child of a node
 *
 * @param[in] self - pointer to a node structure
 * @param[in] child_name - NULL terminated C string with the name of the child to remove
 *
 * @return 0 if self or child_name is NULL, 0 if self does not contain a node with child_name,
 *         1 if node with child_name has been successfully removed.
 *
 * @warning All memory allocated for the child node structure is freed.
 *          If dispose_fun in value_handlers is not NULL, then the value associated with the
 *          removed node will be disposed of using dispose_fun.
 */
int node_remove_child(node_t *self, const char *child_name);

/**
 * Serialize node to JSON
 *
 * @param[in] self - pointer to a node structure
 * @param[out] buffer - pointer to a char buffer
 * @param[in] buffer_size - size of the char buffer
 *
 * @return 0 if self or buffer is NULL
 *         0 if buffer size is too small
 *         0 if serialization failed for any other reason,
 *         number of characters written to buffer otherwise.
 *
 * @warning It is caller's responsibility to provide sufficient buffer size.
 */
int node_to_json(node_t *self, char *buffer, int buffer_size);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CNODE_H_ */
