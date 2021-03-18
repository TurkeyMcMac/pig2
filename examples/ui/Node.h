#ifndef NODE_H_
#define NODE_H_

struct Node_impl {
	const char *label;
	void (*for_each_child)(const void *self, void *ctx,
		void (*each)(const void *child, void *ctx));
};

extern const char Node_iid[1];

#endif /* NODE_H_ */
