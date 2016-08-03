
#ifndef ZEBU_H_
#define ZEBU_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * @defgroup Zebu
 * @{
 * Abstract Syntax Tree
 * @}
 */

/**
 * Minimum amount of memory to allocate at once
 * @ingroup Zebu
 */
#define ZZ_BLOB_SIZE 1024

/** 
 * Data types that may be held by a zz_node
 * @ingroup Zebu
 */
enum {
	ZZ_NULL,                    /**< Node with no information */
	ZZ_INT,                     /**< Node with an integer */
	ZZ_UINT,                    /**< Node with an unsigned integer */
	ZZ_DOUBLE,                  /**< Node with a floating point number */
	ZZ_STRING,                  /**< Node with a string */
	ZZ_POINTER                  /**< Node with user-managed data */
};

/**
 * Doubly-linked list
 * @ingroup Zebu
 */
struct zz_list {
	/** Previous node */
	struct zz_list *prev;
	/** Next node */
	struct zz_list *next;
};

/**
 * Data that may be held by a node in the AST
 * @ingroup Zebu
 */
union zz_node_data {
	/** For ZZ_UINT */
	unsigned int uint_val;
	/** For ZZ_INT */
	int int_val;
	/** For ZZ_DOUBLE */
	double double_val;
	/** For ZZ_STRING */
	const char *str_val;
	/** For ZZ_POINTER */
	void *ptr_val;
};

/**
 * Node in an Abstract Syntax Tree 
 * @ingroup Zebu
 */
struct zz_node {
	/** Siblings */
	struct zz_list siblings;
	/** Children */
	struct zz_list children;
	/** Tree to which this node belongs */
	struct zz_tree *tree;
	/** Token for this node */
	int token;
	/** Type of data held by the node */
	int type;
	/** Data that depends on the node type */
	union zz_node_data data;
};

/**
 * Abstract Syntax Tree
 *
 * Includes not only the tree structure--in fact, it doesn't even know which
 * node is the root--, but knows node types and names, and is the factory for
 * producing new zz_node and strings.
 *
 * @ingroup Zebu
 */
struct zz_tree {
	/** All node names for this tree */
	const char *const *token_names;
	/** Size of _token_names */
	size_t num_tokens;
	/** Size of each node */
	size_t node_size;
	/** All memory managed by this tree */
	void *blobs;
	/** Index of all strings managed by the tree */
	void *strings;
	/** Function to report an error */
	void (* error)(void *, const char *);
};

/**
 * Initialize tree 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param node_size size of node; it must be at least sizeof(struct zz_node)
 * @param token_names all node names
 * @param num_tokens number of elements in _token_names_
 */
void zz_tree_init(struct zz_tree *tree, size_t node_size,
		const char *const *token_names, size_t num_tokens);
/**
 * Destroy tree 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 */
void zz_tree_destroy(struct zz_tree *tree);

/**
 * Get token name for token, or __NULL__ if nonexistent
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a valid token for _tree_
 * @return name of _tok_, or __NULL__
 */
const char *zz_tree_token_name(struct zz_tree *tree, int tok);
/**
 * Get token name for node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return name of the token of _node_, or __NULL__
 */
const char *zz_token_name(struct zz_node *node);

/**
 * Copy a node 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param node zz_node to copy
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_copy(struct zz_tree *tree, struct zz_node *node);
/**
 * Copy a node and all its children recursively 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param node zz_node to copy
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_copy_recursive(struct zz_tree *tree, struct zz_node *node);

/**
 * Create an empty node 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_null(struct zz_tree *tree, int tok);
/**
 * Create a node with int data 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @param val an integer
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_int(struct zz_tree *tree, int tok, int val);
/**
 * Create a node with unsigned int data 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @param val an unsigned integer
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_uint(struct zz_tree *tree, int tok, unsigned int val);
/**
 * Create a node with double data 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @param val a floating point number
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_double(struct zz_tree *tree, int tok, double val);
/**
 * Create a node with string data 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @param val a __NULL_ terminated string
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_string(struct zz_tree *tree, int tok, const char *val);
/**
 * Create a node with user-managed data data 
 *
 * @memberof zz_tree
 * @param tree a zz_tree
 * @param tok a token
 * @param val a pointer
 * @return a new zz_node allocated by _tree_
 */
struct zz_node *zz_pointer(struct zz_tree *tree, int tok, void *val);

/**
 * Initialize a list
 *
 * Makes the prev and next pointers point to list: if this is intended to be a
 * sentinel node, it represents an empty list; if it is supposed to be a field
 * inside a node with data, it represents an unlinked node.
 *
 * @memberof zz_list
 * @param list a zz_list
 */
static inline void zz_list_init(struct zz_list *list)
{
	list->next = list;
	list->prev = list;
}
/**
 * Unlinks a node from a list
 *
 * Links together the prev and next nodes to close the gap; it doesn't relink
 * node to itself, use zz_list_init() for that.
 *
 * @memberof zz_list
 * @param node node to unlink
 */
static inline void zz_unlink(struct zz_list *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
}
/**
 * Inserts node before list
 *
 * If node was part of another list, it will be invalidated; you must use
 * zz_list_unlink() on it first.
 *
 * @memberof zz_list
 * @param prev a node in a list
 * @param node node to be inserted.
 */
static inline void zz_insert(struct zz_list *prev, struct zz_list *node)
{
	struct zz_list *next = prev->next;
	next->prev = node;
	prev->next = node;
	node->next = next;
	node->prev = prev;
}
/**
 * Append a node to a list
 *
 * @memberof zz_list
 * @param list sentinel node of a list
 * @param node node to be inserted
 */
static inline void zz_append(struct zz_list *list, struct zz_list *node)
{
	zz_insert(list->prev, node);
}
/**
 * Prepend a node to a list
 *
 * @memberof zz_list
 * @param list sentinel node of a list
 * @param node node to be inserted
 */
static inline void zz_prepend(struct zz_list *list, struct zz_list *node)
{
	zz_insert(list, node);
}
/**
 * Insert nodes before prev
 *
 * @memberof zz_list
 * @param prev a node in a list
 * @param list nodes to be inserted
 */
static inline void zz_splice(struct zz_list *prev, struct zz_list *list)
{
	struct zz_list *next = prev->next;
	list->prev->next = next;
	list->next->prev = prev;
	prev->next = list->next;
	next->prev = list->prev;
}
/**
 * Append nodes to a list
 *
 * @memberof zz_list
 * @param list sentinel node of a list
 * @param nodes nodes to be inserted
 */
static inline void zz_append_list(struct zz_list *list, struct zz_list *nodes)
{
	zz_splice(list->prev, nodes);
}
/**
 * Prepend nodes to a list
 *
 * @memberof zz_list
 * @param list sentinel node of a list
 * @param nodes nodes to be inserted
 */
static inline void zz_prepend_list(struct zz_list *list, struct zz_list *nodes)
{
	zz_splice(list, nodes);
}
/**
 * Replace old_node by node
 *
 * Links the prev and next nodes of old_node to node instead; it doesn't relink
 * old_node to itself, uze zz_list_init() for that; if node already belonged to
 * a list, it will be invalidaded, use zz_list_unlink() on it first.
 *
 * @memberof zz_node
 * @param old_node the node to be replaced
 * @param node a node to replace old_node
 */
static inline void zz_swap(struct zz_list *old_node, struct zz_list *node)
{
	struct zz_list *prev = old_node->prev;
	struct zz_list *next = old_node->next;
	prev->next = node;
	next->prev = node;
	node->next = next;
	node->prev = prev;
}
/**
 * Iterate on a zz_list
 * @memberof zz_list
 */
#define zz_list_foreach(iter, list) \
for ((iter) = (list)->next; (iter) != (list); (iter) = (iter)->next)
/**
 * Iterate on a zz_list, backwards
 * @memberof zz_list
 */
#define zz_list_reverse_foreach(iter, list) \
for ((iter) = (list)->prev; (iter) != (list); (iter) = (iter)->prev)

/**
 * Return element by index (slow)
 *
 * @memberof zz_list
 * @param list a zz_list
 * @param index index of the desired element
 * @return pointer to a node, or __NULL__
 */
static inline struct zz_list *zz_list_index(struct zz_list *list, size_t index)
{
	struct zz_list *iter;
	zz_list_foreach(iter, list) {
		if (index == 0)
			return iter;
		--index;
	}
	return NULL;
}
/**
 * Return integer data associated with node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return integer value associated with _node_
 */
static inline int zz_to_int(const struct zz_node *node)
{
	assert(node->type == ZZ_INT);
	return node->data.int_val;
}
/**
 * Return unsigned integer data associated with node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return unsigned integer value associated with _node_
 */
static inline unsigned int zz_to_uint(const struct zz_node *node)
{
	assert(node->type == ZZ_UINT);
	return node->data.uint_val;
}
/**
 * Return float data associated with node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return floating point value associated with _node_
 */
static inline double zz_to_double(const struct zz_node *node)
{
	assert(node->type == ZZ_DOUBLE);
	return node->data.double_val;
}
/**
 * Return string data associated with node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return string value associated with _node_
 */
static inline const char *zz_to_string(const struct zz_node *node)
{
	assert(node->type == ZZ_STRING);
	return node->data.str_val;
}
/**
 * Return pointer data associated with node 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return pointer value associated with _node_
 */
static inline void *zz_to_pointer(const struct zz_node *node)
{
	assert(node->type == ZZ_POINTER);
	return node->data.ptr_val;
}
/**
 * Append child to node
 *
 * @memberof zz_node
 * @param pt parent for ch
 * @param ch child for pt
 */
static inline void zz_append_child(struct zz_node *pt, struct zz_node *ch)
{
	zz_append(&pt->children, &ch->siblings);
}
/**
 * Prepend child to node
 *
 * @memberof zz_node
 * @param pt parent for ch
 * @param ch child for pt
 */
static inline void zz_prepend_child(struct zz_node *pt, struct zz_node *ch)
{
	zz_prepend(&pt->children, &ch->siblings);
}
/**
 * Append children to node
 *
 * @memberof zz_node
 * @param pt parent for ch
 * @param ch children for pt
 */
static inline void zz_append_children(struct zz_node *pt, struct zz_list *ch)
{
	zz_append_list(&pt->children, ch);
}
/**
 * Prepend children to node
 *
 * @memberof zz_node
 * @param pt parent for ch
 * @param ch children for pt
 */
static inline void zz_prepend_children(struct zz_node *pt, struct zz_list *ch)
{
	zz_prepend_list(&pt->children, ch);
}

/**
 * Return next sibling
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return pointer to the next sibling of _node_
 */
static inline struct zz_node *zz_next(const struct zz_node *node)
{
	return (void *)node->siblings.next;
}
/**
 * Return previous sibling
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return pointer to the prev sibling of _node_
 */
static inline struct zz_node *zz_prev(const struct zz_node *node)
{
	return (void *)node->siblings.prev;
}
/**
 * Return first child 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return pointer to the first child of _node_, or __NULL__
 */
static inline struct zz_node *zz_first_child(const struct zz_node *node)
{
	return (void *)node->children.next;
}
/**
 * Return last child 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return pointer to the last child of _node_, or __NULL__
 */
static inline struct zz_node *zz_last_child(const struct zz_node *node)
{
	return (void *)node->children.prev;
}

/**
 * Print the full tree whose root is _node_ to _f_ 
 *
 * @memberof zz_node
 * @param node a zz_node
 * @param f a FILE
 */
void zz_print_node(struct zz_node *node, FILE * f);
/**
 * Print the full forest whose roots are _list_ to _f_ 
 *
 * @memberof zz_list
 * @param list a zz_list
 * @param f a FILE
 */
void zz_print_list(struct zz_list *list, FILE * f);

/**
 * Match node
 *
 * If the token for _node_ is not _tok_, call zz_node_error() and return -1.
 *
 * @memberof zz_node
 * @param node a zz_node
 * @param tok a token
 * @return 0 on success, -1 on failure
 */
int zz_match(struct zz_node *node, int tok);
/**
 * Match end of list
 *
 * If _node->next_ is not __NULL__, call zz_node_error() and return -1.
 *
 * @memberof zz_node
 * @param node a zz_node
 * @return 0 on success, -1 on failure
 */
int zz_match_end(struct zz_node *node, struct zz_list *list);
/**
 * Print error from node
 *
 * Shorthand for node->tree->error(node, msg)
 *
 * @memberof zz_node
 * @param node a zz_node
 * @param msg error messabe to be printed
 */
void zz_node_error(struct zz_node *node, const char *msg);
/**
 * Print error message
 *
 * Prints an error message including the file name _file_ and line _line_, then
 * prints the offending line and a caret pointing at the offending column.
 *
 * @ingroup Zebu
 * @param msg error messabe to be printed
 * @param file path to the file with the error
 * @param line offending line
 * @param first_column first offending column
 * @param last_column last offending column
 */
void zz_error(const char *msg, const char *file, size_t first_line,
		size_t first_column, size_t last_line, size_t last_column);

#endif
