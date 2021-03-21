#include "GridWidget.h"
#include "Node.h"
#include "Widget.h"
#include <assert.h>
#include <stddef.h>

struct GridWidget {
	Object obj;
	int width, height;
	// Cached after the calculation.
	struct widget_pair dims, min_dims;
	// The focused element position. These are negative if no focus has yet
	// been set.
	int focus_x, focus_y;
	// The tiles are in row-major order, so the tile at (x, y) on grid is
	// grid->tiles[y * grid->width + x]. NULL obj means an empty tile.
	struct tile {
		Object *obj;
		// Cached after the calculation.
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
	grid->focus_x = -1;
	grid->focus_y = -1;
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
	if (x >= 0 && x < grid->width && y >= 0 && y < grid->height) {
		assert(!child || PIG2_GET(child, Widget_iid));
		Object_remove_ref(grid->tiles[y * grid->width + x].obj);
		grid->tiles[y * grid->width + x].obj = child;
		return true;
	}
	// The reference is in possession of the grid, so it must destroy it.
	Object_remove_ref(child);
	return false;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	GridWidget *self = self_void;
	dims->y = 0;
	min_dims->y = 0;
	// Calculate the requested dims of all the children, the heights of all
	// the rows, and the total height (preferred and minimum.)
	for (int y = 0; y < self->height; ++y) {
		int row_height = 0;
		int min_row_height = 0;
		// Calculate the dimensions and the row height.
		for (int x = 0; x < self->width; ++x) {
			struct tile *tile = &self->tiles[y * self->width + x];
			if (tile->obj) {
				const struct Widget_impl *impl =
					PIG2_GET(tile->obj, Widget_iid);
				impl->get_requested_dims(tile->obj,
					&tile->dims, &tile->min_dims);
				if (tile->dims.y > row_height)
					row_height = tile->dims.y;
				if (tile->min_dims.y > min_row_height)
					min_row_height = tile->min_dims.y;
			}
		}
		// Set all elements in the row to the same height.
		for (int x = 0; x < self->width; ++x) {
			struct tile *tile = &self->tiles[y * self->width + x];
			tile->dims.y = row_height;
			tile->min_dims.y = min_row_height;
		}
		// Accumulate the height.
		dims->y += row_height;
		min_dims->y += min_row_height;
	}
	dims->x = 0;
	min_dims->x = 0;
	// Calculate the column widths and the total width (preferred and min.)
	for (int x = 0; x < self->width; ++x) {
		int col_width = 0;
		int min_col_width = 0;
		// Calculate the column width.
		for (int y = 0; y < self->height; ++y) {
			struct tile *tile = &self->tiles[y * self->width + x];
			if (tile->obj) {
				if (tile->dims.x > col_width)
					col_width = tile->dims.x;
				if (tile->min_dims.x > min_col_width)
					min_col_width = tile->min_dims.x;
			}
		}
		// Set all elements in the column to the same width.
		for (int y = 0; y < self->height; ++y) {
			struct tile *tile = &self->tiles[y * self->width + x];
			tile->dims.x = col_width;
			tile->min_dims.x = min_col_width;
		}
		// Accumulate the width.
		dims->x += col_width;
		min_dims->x += min_col_width;
	}
	// Cache the dims.
	self->dims = *dims;
	self->min_dims = *min_dims;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	const GridWidget *self = self_void;
	// Each widget has some distance between its minimum and preferred
	// dimensions. The x and y expand are the portion of that difference
	// above the minimum dimension and widget should expand in the
	// respective axes.
	double x_expand, y_expand;
	if (self->min_dims.x > dims.x) {
		// With too little space, the widgets shouldn't expand at all.
		x_expand = 0;
	} else if (self->dims.x > dims.x) {
		// With enough space for the minimum dimensions, this formula
		// makes the widgets just about fill the available space.
		x_expand = (double)(dims.x - self->min_dims.x)
			/ (self->dims.x - self->min_dims.x);
	} else {
		// With ample space, the widgets can expand to their preferred
		// dimensions.
		x_expand = 1;
	}
	if (self->min_dims.y > dims.y) {
		y_expand = 0;
	} else if (self->dims.y > dims.y) {
		y_expand = (double)(dims.y - self->min_dims.y)
			/ (self->dims.y - self->min_dims.y);
	} else {
		y_expand = 1;
	}
	// rel_pos is where to draw the current child widget relative to pos.
	struct widget_pair rel_pos = { .x = 0, .y = 0 };
	for (int y = 0; y < self->height; ++y) {
		struct widget_pair child_dims;
		// Set the child height depending on the first tile in the row,
		// all the tiles in the row have the same height.
		const struct tile *tile = &self->tiles[y * self->width];
		child_dims.y = tile->min_dims.y
			+ (tile->dims.y - tile->min_dims.y) * y_expand;
		// Prevent overflow of the area.
		if (rel_pos.y + child_dims.y > dims.y)
			child_dims.y = dims.y - rel_pos.y;
		// The relative x position is reset for each row.
		rel_pos.x = 0;
		for (int x = 0; x < self->width; ++x) {
			tile = &self->tiles[y * self->width + x];
			// Calculate the width of the child.
			child_dims.x = tile->min_dims.x
				+ (tile->dims.x - tile->min_dims.x) * x_expand;
			if (rel_pos.x + child_dims.x > dims.x)
				child_dims.x = dims.x - rel_pos.x;
			// Draw the child if it exists and is visible.
			if (tile->obj && child_dims.x > 0 && child_dims.y > 0) {
				const struct Widget_impl *impl =
					PIG2_GET(tile->obj, Widget_iid);
				// Convert the relative position to absolute.
				struct widget_pair child_pos = {
					pos.x + rel_pos.x, pos.y + rel_pos.y
				};
				impl->draw(tile->obj, win,
					child_pos, child_dims);
			}
			// Move over to draw the next widget.
			rel_pos.x += child_dims.x;
		}
		// Move down to draw the next widget.
		rel_pos.y += child_dims.y;
	}
}

static bool focus(void *self_void)
{
	GridWidget *self = self_void;
	// Remember the last focused child when refocusing on the grid.
	if (self->focus_x >= 0 && self->focus_y >= 0) {
		void *focus = self->tiles[
			self->focus_y * self->width + self->focus_x].obj;
		if (focus) {
			const struct Widget_impl *impl =
				PIG2_GET(focus, Widget_iid);
			if (impl->focus(focus)) return true;
		}
	}
	// The first time, find a focus starting at the top left.
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			void *child = self->tiles[y * self->width + x].obj;
			if (child) {
				const struct Widget_impl *impl =
					PIG2_GET(child, Widget_iid);
				if (impl->focus(child)) {
					// Remember the focus position.
					self->focus_x = x;
					self->focus_y = y;
					return true;
				}
			}
		}
	}
	return false;
}

static bool recv_input(void *self_void, int key)
{
	GridWidget *self = self_void;
	void *focus =
		self->tiles[self->focus_y * self->width + self->focus_x].obj;
	const struct Widget_impl *focus_impl = NULL;
	// Try to send the input to the focus.
	if (focus) {
		focus_impl = PIG2_GET(focus, Widget_iid);
		if (focus_impl->recv_input(focus, key)) return true;
	}
	// Otherwise, allow for navigation with the arrow keys.
	// dx and dy are the increments with which to search the tiles.
	int dx = 0, dy = 0;
	switch (key) {
	case KEY_RIGHT:
		dx = 1;
		break;
	case KEY_DOWN:
		dy = 1;
		break;
	case KEY_LEFT:
		dx = -1;
		break;
	case KEY_UP:
		dy = -1;
		break;
	default:
		return false;
	}
	// Search for new foci in the direction specified by the arrow key.
	for (int x = self->focus_x + dx, y = self->focus_y + dy;
	     x >= 0 && x < self->width && y >= 0 && y < self->height;
	     x += dx, y += dy) {
		void *child = self->tiles[y * self->width + x].obj;
		bool found = false;
		// Since the widgets form a directed acyclic graph, the focused
		// widget may appear twice. When moving focus to another copy of
		// the same widget, it isn't necessary to unfocus and refocus.
		if (focus && child == focus) {
			found = true;
		} else if (child) {
			const struct Widget_impl *child_impl =
				PIG2_GET(child, Widget_iid);
			found = child_impl->focus(child);
			// Unfocus if necessary.
			if (found && focus_impl) focus_impl->unfocus(focus);
		}
		if (found) {
			// Move the focus.
			self->focus_x = x;
			self->focus_y = y;
			return true;
		}
	}
	return false;
}

static void unfocus(void *self_void)
{
	GridWidget *self = self_void;
	void *focus =
		self->tiles[self->focus_y * self->width + self->focus_x].obj;
	// Unfocus if necessary.
	if (focus) {
		const struct Widget_impl *impl = PIG2_GET(focus, Widget_iid);
		impl->unfocus(focus);
	}
}

static void for_each_child(const void *self_void, void *ctx,
	void (*each)(const void *child, void *ctx))
{
	const GridWidget *self = self_void;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			// each will accept NULLs and things which don't
			// implement Node.
			each(self->tiles[y * self->width + x].obj, ctx);
		}
	}
}

static void release(Object *self_obj)
{
	GridWidget *self = (GridWidget *)self_obj;
	for (int y = 0; y < self->height; ++y) {
		for (int x = 0; x < self->width; ++x) {
			// Object_remove_ref accepts NULL.
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
		.recv_input = recv_input,
		.unfocus = unfocus,
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
