#ifndef CTREE_H_
#define CTREE_H_

#include "cnode.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * Describes a tree of nodes, where each node has a unique object_name
 */
typedef struct
{
    node_t *root;
    node_t **nodes; /**< excluding root */
    int num_nodes; /**< excluding root */
    int capacity; /**< excluding root */
    int depth; /**< -1 means invalid, call tree_depth() to re-calculate */
} tree_t;

/**
 * Initialize a tree_t structure to represent an empty tree
 *
 * @param[in,out] self - pointer to the tree structure to initialize, function does nothing if
 *                self is NULL
 */
void tree_init(tree_t *self);

/**
 * Clear a tree_t structure, dispose of its root and all its children
 *
 * @param[in] self - pointer to the tree structure to clear, function does nothing if self is NULL
 */
void tree_clear(tree_t *self);

/**
 * Get a node with particular name from the tree
 *
 * @param[in] self - the tree to search in
 * @param[in] node_name - the name of the node to look for
 *
 * @return NULL if self or node_name is NULL
 *         NULL if node_name is an empty string (ie. "")
 *         NULL if node_name was not found
 *         pointer to node with node_name if it was found
 */
node_t *tree_get_node(tree_t *self, const char *node_name);

/**
 * Add a new node to the tree
 *
 * @param[in] self - the tree to search in
 * @param[in] new_node - pointer to the node to be added
 * @param[in] parent_node_name - node which should become a parent of the added node, can be NULL
 *                if the tree is empty and we want to add a root node
 *
 * @return NULL if self is NULL,
 *         NULL if new_node is NULL,
 *         NULL if tree is not empty and parent_node_name is NULL
 *         NULL if tree is not empty and parent_node_name cannot be found
 *         new_node if node was added successfully, tree takes ownership of new_node
 */
node_t *tree_add_node(tree_t *self, node_t *new_node, const char *parent_node_name);

/**
 * Add a subtree to the tree
 *
 * @param[in] self - the tree to search in
 * @param[in] sub_tree - pointer to the subtree to be added
 * @param[in] parent_node_name - node which should become a parent of the root of the added tree,
 *                can be NULL if self tree is empty and we want to steal the entire subtree
 *
 * @return NULL if self is NULL,
 *         NULL if sub_tree is NULL,
 *         NULL if self tree is not empty and parent_node_name is NULL
 *         NULL if self tree is not empty and parent_node_name cannot be found
 *         NULL if self tree is not empty and sub_tree root name is already in the self tree
 *         pointer to the root of the subtree if subtree was added successfully, tree self takes
 *             ownership of all nodes of sub_tree (steals them), sub_tree is empty, any nodes
 *             of the subtree that have conflicting names with the parent tree are removed using
 *             tree_remove_node()
 */
node_t *tree_add_tree(tree_t *self, tree_t *sub_tree, const char *parent_node_name);

/**
 * Remove a node from the tree
 *
 * @param[in] self - the tree to remove a node from
 * @param[in] node_name - name of the node to be removed
 *
 * @return 0 if self is NULL,
 *         0 if node_name is NULL,
 *         0 if node with node_name is not found in self tree
 *         0 if node_name is root->object_name but root has children
 *         1 if node_name is root->object_name and root is childless
 *         1 if node_name is found and removed, removed nodes is disposed of using node_dispose(),
 *             children of the removed node become the children of its parent node
 */
int tree_remove_node(tree_t *self, const char *node_name);

/**
 * Remove a subtree from the tree
 *
 * @param[in] self - the tree to remove a subtree from
 * @param[in] sub_tree_root_name - name of the root of the subtree to be removed
 *
 * @return 0 if self is NULL,
 *         0 if sub_tree_root_name is NULL,
 *         0 if node with sub_tree_root_name is not found in self tree
 *         number of the nodes removed, removed nodes are disposed of using node_dispose()
 */
int tree_remove_tree(tree_t *self, const char *sub_tree_root_name);

/**
 * Get tree depth
 *
 * @param[in] self - the tree whose depth should be calculated
 *
 * @return depth of the tree or 0 if the tree is empty
 */
int tree_depth(tree_t *self);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CTREE_H_ */
