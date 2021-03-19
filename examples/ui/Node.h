#ifndef NODE_H_
#define NODE_H_

#include "String.h"

struct Node_impl {
	const char *label;
	void (*for_each_child)(const void *self, void *ctx,
		void (*each)(const void *child, void *ctx));
};

extern const char Node_iid[1];

String *Node_tree_to_str(const void *node);

#endif /* NODE_H_ */
