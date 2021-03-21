#ifndef NODE_H_
#define NODE_H_

#include "String.h"

// Node is an interface for debugging and such. Each node has a static label and
// a dynamic number of children.

struct Node_impl {
	// This is the short name of the node type.
	const char *label;
	// Iterates through all the children of self. each is passed each child
	// alongside the context ctx. each may be passed children which are NULL
	// or which do not implement Node themselves, and it will ignore these.
	// If for_each_child is NULL, the node self has no children.
	void (*for_each_child)(const void *self, void *ctx,
		void (*each)(const void *child, void *ctx));
};

// This is the Node interface identifier.
extern const char Node_iid[1];

// Converts a directed acyclic graph of nodes whose root is node into a string
// representation of a tree of nodes. Indentation is used to show children.
String *Node_tree_to_str(const void *node);

#endif /* NODE_H_ */
