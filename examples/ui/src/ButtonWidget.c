#include "ButtonWidget.h"
#include "Node.h"
#include "Widget.h"
#include <stdlib.h>
#include <string.h>

struct ButtonWidget {
	Object obj;
	Object *ctx;
	void (*fun)(Object *ctx);
	bool focused;
	char label[];
};

static const void *getter(const void *iid);

ButtonWidget *ButtonWidget_alloc(const char *label,
	Object *ctx, void (*fun)(Object *ctx))
{
	size_t label_size = strlen(label) + 1;
	ButtonWidget *bw = (ButtonWidget *)Object_alloc(
		offsetof(ButtonWidget, label) + label_size);
	PIG2_SET_GETTER(bw, getter);
	bw->ctx = ctx;
	bw->fun = fun;
	bw->focused = false;
	memcpy(bw->label, label, label_size);
	return bw;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	const ButtonWidget *self = self_void;
	dims->x = (int)strlen(self->label);
	dims->y = 1;
	*min_dims = *dims;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	const ButtonWidget *self = self_void;
	if (dims.y >= 1) {
		if (self->focused) wattron(win, A_REVERSE);
		mvwaddnstr(win, pos.y, pos.x, self->label, dims.x);
		if (self->focused) wattroff(win, A_REVERSE);
	}
}

static bool focus(void *self)
{
	((ButtonWidget *)self)->focused = true;
	return true;
}

static bool recv_input(void *self_void, int key)
{
	ButtonWidget *self = self_void;
	if (key == '\n') {
		self->fun(self->ctx);
		return true;
	}
	return false;
}

static void unfocus(void *self)
{
	((ButtonWidget *)self)->focused = false;
}

static void for_each_child(const void *self, void *ctx,
	void (*each)(const void *child, void *ctx))
{
	each(((const ButtonWidget *)self)->ctx, ctx);
}

static void release(Object *self)
{
	Object_remove_ref(((ButtonWidget *)self)->ctx);
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
		.label = "ButtonWidget",
		.for_each_child = for_each_child,
	};
	if (iid == Node_iid) return &Node_impl;

	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	return Object_getter(iid);
}
