#include "ScrollWidget.h"
#include "Node.h"
#include "Widget.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct ScrollWidget {
	Object obj;
	Object *inner;
	int inner_height;
	int height;
	int scroll_y;
	bool self_focus;
};

static const void *getter(const void *iid);

ScrollWidget *ScrollWidget_alloc(int height, Object *inner)
{
	ScrollWidget *sw = (ScrollWidget *)Object_alloc(sizeof(ScrollWidget));
	PIG2_SET_GETTER(sw, getter);
	sw->inner = inner;
	sw->inner_height = 0; // Not yet calculated.
	sw->height = height;
	sw->scroll_y = 0;
	sw->self_focus = false;
	return sw;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	ScrollWidget *self = self_void;
	const struct Widget_impl *impl = PIG2_GET(self->inner, Widget_iid);
	assert(impl);
	impl->get_requested_dims(self->inner, dims, min_dims);
	self->inner_height = dims->y;
	dims->x += 2;
	dims->y = self->height;
	min_dims->x += 1;
	min_dims->y = self->height;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	const ScrollWidget *self = self_void;
	const struct Widget_impl *inner_impl =
		PIG2_GET(self->inner, Widget_iid);
	assert(inner_impl);
	--dims.x;
	if (dims.x > 0) {
		if (self->inner_height <= self->height) {
			inner_impl->draw(self->inner, win, pos, dims);
		} else if (dims.x > 0) {
			WINDOW *inner_canvas =
				newpad(self->inner_height, dims.x);
			assert(inner_canvas);
			inner_impl->draw(self->inner, inner_canvas,
				(struct widget_pair) { 0, 0 },
				(struct widget_pair) {
					dims.x, self->inner_height });
			for (int y = 0;
			    y < self->height
			 && y < dims.y
			 && y < self->inner_height - self->scroll_y; ++y) {
				for (int x = 0; x < dims.x; ++x) {
					chtype ch = mvwinch(inner_canvas,
						self->scroll_y + y, x);
					if (ch != (chtype)ERR)
						mvwaddch(win, pos.y + y,
							pos.x + x, ch);
				}
			}
			delwin(inner_canvas);
		}
		chtype bar_ch = '|';
		if (self->self_focus) bar_ch |= A_REVERSE;
		double progress = (double)self->scroll_y / self->inner_height;
		double bar_size = (double)self->height / self->inner_height;
		int bar_top = progress * self->height;
		int bar_bot = (progress + bar_size) * self->height;
		for (int y = bar_top; y <= bar_bot && y < dims.y; ++y) {
			mvwaddch(win, pos.y + y, pos.x + dims.x, bar_ch);
		}
	}
}

static bool focus(void *self_void)
{
	ScrollWidget *self = self_void;
	const struct Widget_impl *inner_impl =
		PIG2_GET(self->inner, Widget_iid);
	assert(inner_impl);
	self->self_focus = !inner_impl->focus(self->inner);
	return true;
}

static bool recv_input(void *self_void, int key)
{
	ScrollWidget *self = self_void;
	const struct Widget_impl *inner_impl =
		PIG2_GET(self->inner, Widget_iid);
	assert(inner_impl);
	if (inner_impl->recv_input && inner_impl->recv_input(self->inner, key))
		return true;
	switch (key) {
	case KEY_UP:
		if (self->scroll_y > 0) {
			--self->scroll_y;
			return true;
		}
		break;
	case KEY_DOWN:
		{
			struct widget_pair dims, min_dims;
			inner_impl->get_requested_dims(self->inner,
				&dims, &min_dims);
			if (self->scroll_y < dims.y - self->height) {
				++self->scroll_y;
				return true;
			}
		}
		break;
	}
	return false;
}

static void unfocus(void *self_void)
{
	ScrollWidget *self = self_void;
	const struct Widget_impl *inner_impl =
		PIG2_GET(self->inner, Widget_iid);
	assert(inner_impl);
	if (inner_impl->unfocus) inner_impl->unfocus(self->inner);
	self->self_focus = false;
}

static void for_each_child(const void *self, void *ctx,
		void (*each)(const void *child, void *ctx))
{
	each(((const ScrollWidget *)self)->inner, ctx);
}

static void release(Object *self)
{
	Object_remove_ref(((ScrollWidget *)self)->inner);
	Object_release(self);
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
		.label = "ScrollWidget",
		.for_each_child = for_each_child,
	};
	if (iid == Node_iid) return &Node_impl;

	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	return Object_getter(iid);
}
