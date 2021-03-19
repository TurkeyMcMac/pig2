#include "Node.h"
#include "String.h"
#include <pig2.h>

struct tree_ctx {
	String *str;
	int depth;
};

static void tree_to_str(const void *node, void *ctx_void)
{
	struct tree_ctx *ctx = ctx_void;
	const struct Node_impl *impl;
	if (node && (impl = PIG2_GET(node, Node_iid))) {
		for (int i = 0; i < ctx->depth; ++i) {
			String_append(ctx->str, "  ");
		}
		String_append(ctx->str, impl->label);
		String_append(ctx->str, "\n");
		if (impl->for_each_child) {
			struct tree_ctx child_ctx =
				{ ctx->str, ctx->depth + 1 };
			impl->for_each_child(node, &child_ctx, tree_to_str);
		}
	}
}

String *Node_tree_to_str(const void *node)
{
	struct tree_ctx ctx = { String_alloc(), 0 };
	tree_to_str(node, &ctx);
	return ctx.str;
}

const char Node_iid[1];
