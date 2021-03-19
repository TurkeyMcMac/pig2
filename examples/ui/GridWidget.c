#include "GridWidget.h"
#include "Node.h"
#include "Widget.h"
#include <assert.h>
#include <stddef.h>

struct GridWidget {
	Object obj;
	int width, height;
	struct widget_pair dims, min_dims; // Cached after the calculation.
	struct tile {
		Object *obj;
		struct widget_pair dims, min_dims;
	} tiles[];
};

static const void *getter(const void *iid);

GridWidget *GridWidget_alloc(int width, int height)
{
	assert(width >= 0);
	assert(height >= 0);
	GridWidget *grid = (GridWidget *)Object_alloc(
		offsetof(GridWidget, tiles)
			+ width * height * sizeof(*grid->tiles));
	PIG2_SET_GETTER(grid, getter);
	grid->width = width;
	grid->height = height;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			grid->tiles[y * width + x].obj = NULL;
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
	if (x >= 0 && x < grid->width && y >= 0 && y < grid->height
	 && PIG2_GET(child, Widget_iid)) {
		grid->tiles[y * grid->width + x].obj = child;
		return true;
	}
	return false;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	GridWidget *self = self_void;
	dims->y = 0;
	min_dims->y = 0;
	for (int y = 0; y < self->height; ++y) {
		int row_height = 0;
		int min_row_height = 0;
		for (int x = 0; x < self->width; ++x) {
			struct tile *tile = &self->tiles[y * self->width + x];
			if (tile->obj) {
				const struct Widget_impl *impl =
					PIG2_GET(tile->obj, Widget_iid);
				assert(impl);
				impl->get_requested_dims(tile->obj,
					&tile->dims, &tile->min_dims);
				if (tile->dims.y > row_height)
					row_height = tile->dims.y;
				if (tile->min_dims.y > min_row_height)
					min_row_height = tile->min_dims.y;
			}
		}
		for (int x = 0; x < self->width; ++x) {
			struct tile *tile = &self->tiles[y * self->width + x];
			tile->dims.y = row_height;
			tile->min_dims.y = min_row_height;
		}
		dims->y += row_height;
		min_dims->y += min_row_height;
	}
	dims->x = 0;
	min_dims->x = 0;
	for (int x = 0; x < self->width; ++x) {
		int col_width = 0;
		int min_col_width = 0;
		for (int y = 0; y < self->height; ++y) {
			struct tile *tile = &self->tiles[y * self->width + x];
			if (tile->obj) {
				if (tile->dims.x > col_width)
					col_width = tile->dims.x;
				if (tile->min_dims.x > min_col_width)
					min_col_width = tile->min_dims.x;
			}
		}
		for (int y = 0; y < self->height; ++y) {
			struct tile *tile = &self->tiles[y * self->width + x];
			tile->dims.x = col_width;
			tile->min_dims.x = min_col_width;
		}
		dims->x += col_width;
		min_dims->x += min_col_width;
	}
	self->dims = *dims;
	self->min_dims = *min_dims;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	const GridWidget *self = self_void;
	bool x_small = self->dims.x > dims.x;
	bool x_min_small = self->min_dims.x > dims.x;
	bool y_small = self->dims.y > dims.y;
	bool y_min_small = self->min_dims.y > dims.y;
	struct widget_pair child_pos = { .x = 0, .y = pos.x };
	for (int y = 0; y < self->height; ++y) {
		struct widget_pair child_dims;
		const struct tile *tile = &self->tiles[y * self->width];
		if (y_min_small) {
			child_dims.y = tile->min_dims.y;
		} else if (y_small) {
			child_dims.y = tile->min_dims.y
				+ (dims.y - self->min_dims.y) / self->width;
		} else {
			child_dims.y = tile->dims.y;
		}
		if (child_pos.y + child_dims.y > dims.y)
			child_dims.y = dims.y - child_pos.y;
		child_pos.x = pos.x;
		for (int x = 0; x < self->width; ++x) {
			tile = &self->tiles[y * self->width + x];
			if (x_min_small) {
				child_dims.x = tile->dims.x;
			} else if (x_small) {
				child_dims.x = tile->dims.x
					+ (dims.x - self->min_dims.x)
						/ self->width;
			} else {
				child_dims.x = tile->dims.x;
			}
			if (child_pos.x + child_dims.x > dims.x)
				child_dims.x = dims.x - child_pos.x;
			if (tile->obj) {
				const struct Widget_impl *impl =
					PIG2_GET(tile->obj, Widget_iid);
				assert(impl);
				impl->draw(tile->obj, win,
					child_pos, child_dims);
			}
			child_pos.x += child_dims.x;
		}
		child_pos.y += child_dims.y;
	}
}

static bool focus(void *self)
{
	(void)self;
	return false;
}

static void for_each_child(const void *self_void, void *ctx,
	void (*each)(const void *child, void *ctx))
{
	const GridWidget *self = self_void;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			each(self->tiles[y * self->width + x].obj, ctx);
		}
	}
}

static void release(Object *self_obj)
{
	GridWidget *self = (GridWidget *)self_obj;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			Object_remove_ref(self->tiles[y * self->width + x].obj);
		}
	}
	Object_release((Object *)self);
}

static const void *getter(const void *iid)
{
	static const struct Widget_impl Widget_impl = {
		.get_requested_dims = get_requested_dims,
		.draw = draw,
		.focus = focus,
	};
	if (iid == Widget_iid) return &Widget_impl;

	static const struct Node_impl Node_impl = {
		.label = "GridWidget",
		.for_each_child = for_each_child,
	};
	if (iid == Node_iid) return &Node_impl;

	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	return Object_getter(iid);
}
