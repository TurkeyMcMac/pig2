#include "GridWidget.h"
#include "Node.h"
#include "Widget.h"
#include <assert.h>
#include <stddef.h>

struct GridWidget {
	Object obj;
	int width, height;
	Object *tiles[];
};

static const void *getter(const void *iid);

GridWidget *GridWidget_alloc(int width, int height)
{
	assert(width >= 0);
	assert(height >= 0);
	GridWidget *grid = (GridWidget *)Object_alloc(
		offsetof(GridWidget, tiles)
			+ width * height * sizeof(grid->tiles));
	PIG2_SET_GETTER(grid, getter);
	grid->width = width;
	grid->height = height;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			grid->tiles[y * width + x] = NULL;
		}
	}
	return grid;
}

int GridWidget_width(const GridWidget *grid)
{
	return grid->width;
}

int GridWidget_height(const GridWidget *grid)
{
	return grid->height;
}

bool GridWidget_place(GridWidget *grid, int x, int y, Object *child)
{
	if ((x >= 0 && x < grid->width) || (y >= 0 && y < grid->height)
	 || !PIG2_GET(child, Widget_iid))
		return false;
	grid->tiles[y * grid->width + x] = child;
	return true;
}

static void for_each_child(const void *self_void, void *ctx,
	void (*each)(const void *child, void *ctx))
{
	const GridWidget *self = self_void;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			each(self->tiles[y * self->width + x], ctx);
		}
	}
}

static void release(Object *self_obj)
{
	GridWidget *self = (GridWidget *)self_obj;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			Object_remove_ref(self->tiles[y * self->width + x]);
		}
	}
	Object_release((Object *)self);
}

static const void *getter(const void *iid)
{
#if 0
	static const struct Widget_impl Widget_impl = {
		.draw = draw,
		.focus = focus,
		.recv_input = recv_input,
		.unfocus = unfocus,
	};
	if (iid == Widget_iid) return &Widget_impl;
#endif

	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	static const struct Node_impl Node_impl = {
		.label = "GridWidget",
		.for_each_child = for_each_child,
	};
	if (iid == Node_iid) return &Node_impl;

	return Object_getter(iid);
}
