#include "ctree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TREE_REALLOC_INCREMENT 4
#define TREE_DEBUG 0

#if TREE_DEBUG
static void tree_dump(const char *func, int line, tree_t *self)
{
    int i = 0;

    if (self == NULL)
    {
        printf("(nil)");
        return;
    }

    if (self->num_nodes > 0)
    {
        printf(
            "%s:%d (%p, %p, %p, {",
            func,
            line,
            (void *)self,
            (void *)self->root,
            (void *)self->nodes);

        for (; i < self->num_nodes; ++i)
        {
            printf("%p", (void *)self->nodes[i]);

            if (i < self->num_nodes - 1)
            {
                printf(",");
            }
        }

        printf(
            "}, %d, %d)\n",
            self->num_nodes,
            self->capacity);
    }
    else
    {
        printf(
            "%s:%d (%p, %p, %p, {}, %d, %d)\n",
            func,
            line,
            (void *)self,
            (void *)self->root,
            (void *)self->nodes,
            self->num_nodes,
            self->capacity);
    }
}

#define TREE_DUMP(TREE) tree_dump(__func__, __LINE__, TREE)
#else
#define TREE_DUMP(TREE)
#endif /* TREE_DEBUG */

void tree_init(tree_t *self)
{
    if (self == NULL)
    {
        return;
    }

    memset(self, 0, sizeof(tree_t));

    TREE_DUMP(self);
}

void tree_clear(tree_t *self)
{
    int i = 0;

    if (self == NULL)
    {
        return;
    }

    node_dispose(self->root);

    for (; i < self->num_nodes; ++i)
    {
        node_dispose(self->nodes[i]);
    }

    free(self->nodes);
    memset(self, 0, sizeof(tree_t));
}

/* TODO use binary search version */
static int tree_find_node_pos(tree_t *self, const char *node_name)
{
    int i = 0;

    for (; i < self->num_nodes; ++i)
    {
        if ((self->nodes[i] != NULL) && strcmp(node_name, self->nodes[i]->object_name) == 0)
        {
            return i;
        }
    }

    /* not found */
    return -1;
}

/* TODO use binary search version */
static int tree_find_closest_larger(tree_t *self, const char *node_name)
{
    int i = 0;

    for (; i < self->num_nodes; ++i)
    {
        if (strcmp(node_name, self->nodes[i]->object_name) < 0)
        {
            return i;
        }
    }

    /* not found */
    return i;
}

node_t *tree_get_node(tree_t *self, const char *node_name)
{
    int pos = 0;

    TREE_DUMP(self);

    if ((self == NULL) || (node_name == NULL) || (self->root == NULL) || (strlen(node_name) == 0))
    {
        return NULL;
    }

    if (strcmp(self->root->object_name, node_name) == 0)
    {
        return self->root;
    }

    pos = tree_find_node_pos(self, node_name);

    if (pos == -1)
    {
        return NULL;
    }

    return self->nodes[pos];
}

static node_t *tree_insert_node(tree_t *self, node_t *new_node)
{
    /* find where the new node should be in the nodes table */
    int pos = tree_find_closest_larger(self, new_node->object_name);

    memmove(&self->nodes[pos + 1], &self->nodes[pos], (self->num_nodes - pos) * sizeof(node_t *));
    ++self->num_nodes;

    /* insert the new node */
    self->nodes[pos] = new_node;

    TREE_DUMP(self);

    /* success */
    return new_node;
}

static node_t **tree_grow_if_needed(tree_t *self)
{
    TREE_DUMP(self);

    if (self->num_nodes < self->capacity)
    {
        return self->nodes;
    }

    /* nodes table is too small - it needs to grow */
    node_t **new_nodes = realloc(self->nodes, (self->capacity + TREE_REALLOC_INCREMENT) * sizeof(node_t *));

    /* failed to reallocate memory */
    if (new_nodes == NULL)
    {
        return NULL;
    }

    self->nodes = new_nodes;
    self->capacity += TREE_REALLOC_INCREMENT;
    return self->nodes;
}

node_t *tree_add_node(tree_t *self, node_t *new_node, const char *parent_node_name)
{
    TREE_DUMP(self);

    /* invalid tree or invalid node */
    if ((self == NULL) || (new_node == NULL))
    {
        return NULL;
    }

    /* our tree is empty */
    if (self->root == NULL)
    {
        self->root = new_node;
        self->depth = 1;
        TREE_DUMP(self);
        return new_node;
    }

    /* our tree already contains a node with such name */
    if (tree_get_node(self, new_node->object_name) != NULL)
    {
        return NULL;
    }

    /* invalid parent node name */
    if (parent_node_name == NULL)
    {
        return NULL;
    }

    node_t *parent = tree_get_node(self, parent_node_name);

    /* our tree does not contain such node */
    if (parent == NULL)
    {
        return NULL;
    }

    if (tree_grow_if_needed(self) == NULL)
    {
        return NULL;
    }

    /* add the new node to its parent */
    node_add_child(parent, new_node);

    /* invalidate depth */
    self->depth = -1;

    /* success */
    return tree_insert_node(self, new_node);
}

static void tree_remove_conflicts(tree_t *self, tree_t *other_tree)
{
    int i = 0;

    tree_remove_node(other_tree, self->root->object_name);

    for (; i < self->num_nodes; ++i)
    {
        tree_remove_node(other_tree, self->nodes[i]->object_name);
    }
}

node_t *tree_add_tree(tree_t *self, tree_t *sub_tree, const char *parent_node_name)
{
    node_t *parent = NULL;
    node_t *sub_tree_root = NULL;
    int i = 0;

    /* invalid inputs */
    if ((self == NULL) || (sub_tree == NULL) || (sub_tree->root == NULL))
    {
        return NULL;
    }

    /* our tree is empty - steal everything */
    if (self->root == NULL)
    {
        memcpy(self, sub_tree, sizeof(tree_t));
        memset(sub_tree, 0, sizeof(tree_t));
        return self->root;
    }

    /* only allowed if our tree is empty */
    if (parent_node_name == NULL)
    {
        return NULL;
    }

    parent = tree_get_node(self, parent_node_name);

    /* no such node exists */
    if (parent == NULL)
    {
        return NULL;
    }

    /* sub tree root name is in conflict with one of our nodes */
    if (tree_get_node(self, sub_tree->root->object_name) != NULL)
    {
        return NULL;
    }

    /* Remove conflicts from sub tree */
    tree_remove_conflicts(self, sub_tree);

    /* steal root from sub_tree */
    sub_tree_root = tree_add_node(self, sub_tree->root, parent->object_name);

    /* steal all other nodes from sub tree */
    for (i = 0; i < sub_tree->num_nodes; ++i)
    {
        if (tree_grow_if_needed(self) == NULL)
        {
            return NULL;
        }

        tree_insert_node(self, sub_tree->nodes[i]);
    }

    /* clear sub tree */
    memset(sub_tree, 0, sizeof(tree_t));

    /* invalidate depth */
    self->depth = -1;

    /* done */
    return sub_tree_root;
}

int tree_remove_node(tree_t *self, const char *node_name)
{
    int i = 0;
    int pos = 0;
    node_t *node = NULL;

    /* invalid inputs or empty tree */
    if ((self == NULL) || (node_name == NULL) || (self->root == NULL))
    {
        return 0;
    }

    if (strcmp(self->root->object_name, node_name) == 0)
    {
        /* cannot remove root if it has children */
        if (self->root->num_children > 0)
        {
            return 0;
        }
        else
        {
            tree_clear(self);
            return 1;
        }
    }

    pos = tree_find_node_pos(self, node_name);

    /* not found */
    if (pos == -1)
    {
        return 0;
    }

    node = self->nodes[pos];

    /* attach children of removed node to its parent */
    for (i = 0; i < node->num_children; ++i)
    {
        node_add_child(node->parent, node->children[i]);
    }

    /*
     * memmove bug - cannot shift left
     * https://github.com/fingolfin/memmove-bug/blob/master/glibc-memcpy.patch
     */
    for (i = pos; i < self->num_nodes; ++i)
    {
        self->nodes[i] = self->nodes[i + 1];
    }

    /* remove the node from its parent's children and dispose of it */
    node_remove_child(node->parent, node->object_name);
    --self->num_nodes;

    /* invalidate depth */
    self->depth = -1;

    return 1;
}

static int tree_mark_for_squeeze(tree_t *self, node_t *node)
{
    int i = 0;
    int removed_cnt = 0;

    for (; i < node->num_children; ++i)
    {
        removed_cnt += tree_mark_for_squeeze(self, node->children[i]);
    }

    self->nodes[tree_find_node_pos(self, node->object_name)] = NULL;
    node_dispose(node);
    ++removed_cnt;

    return removed_cnt;
}

static void remove_child_from_list(node_t *parent, node_t *child)
{
    int i = 0;
    int j = 0;
    /* find it */
    for (; i < parent->num_children; ++i)
    {
        if (strcmp(parent->children[i]->object_name, child->object_name) == 0)
        {
            break;
        }
    }

    /*
     * memmove bug - cannot shift left
     * https://github.com/fingolfin/memmove-bug/blob/master/glibc-memcpy.patch
     */
    for (j = i; j < parent->num_children; ++j)
    {
        parent->children[j] = parent->children[j + 1];
    }

    --parent->num_children;
}

static void tree_squeeze(tree_t *self)
{
    int i = 0;
    int last_non_null = -1;

    /* shift left to remove all NULLs */
    while ((i < self->num_nodes) && (last_non_null < self->num_nodes))
    {
        if (self->nodes[i] == NULL)
        {
            if (last_non_null == -1)
            {
                last_non_null = i + 1;
            }

            while (last_non_null < self->num_nodes)
            {
                if (self->nodes[last_non_null] != NULL)
                {
                    break;
                }

                ++last_non_null;
            }

            self->nodes[i] = self->nodes[last_non_null];

            ++last_non_null;
        }

        ++i;
    }
}

int tree_remove_tree(tree_t *self, const char *sub_tree_root_name)
{
    node_t *sub_tree_root = NULL;
    int pos = 0;
    int removed_cnt = 0;

    /* invalid inputs or empty tree */
    if ((self == NULL) || (sub_tree_root_name == NULL) || (self->root == NULL))
    {
        return 0;
    }

    /* remove entire tree */
    if (strcmp(sub_tree_root_name, self->root->object_name) == 0)
    {
        removed_cnt = self->num_nodes + 1;
        tree_clear(self);
        return removed_cnt;
    }

    pos = tree_find_node_pos(self, sub_tree_root_name);

    /* subtree root not found */
    if (pos == -1)
    {
        return 0;
    }

    sub_tree_root = self->nodes[pos];
    remove_child_from_list(sub_tree_root->parent, sub_tree_root);
    removed_cnt = tree_mark_for_squeeze(self, sub_tree_root);
    tree_squeeze(self);

    self->num_nodes -= removed_cnt;

    /* invalidate depth */
    self->depth = -1;

    return removed_cnt;
}

static void tree_depth_recursive(node_t *node, int *depth, int local_depth)
{
    int i = 0;

    if (node->num_children == 0)
    {
        return;
    }

    ++local_depth;

    if (*depth < local_depth)
    {
        *depth = local_depth;
    }

    for (; i < node->num_children; ++i)
    {
        tree_depth_recursive(node->children[i], depth, local_depth);
    }
}

int tree_depth(tree_t *self)
{
    if (self == NULL)
    {
        return 0;
    }

    if (self->root == NULL)
    {
        return 0;
    }

    if (self->num_nodes == 0)
    {
        self->depth = 1;
        return self->depth;
    }

    /* still valid */
    if (self->depth >= 0)
    {
        return self->depth;
    }

    self->depth = 1;

    /* need to re-calculate */
    tree_depth_recursive(self->root, &self->depth, 1);

    return self->depth;
}
