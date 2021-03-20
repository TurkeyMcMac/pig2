#include "ButtonWidget.h"
#include "Node.h"
#include "Object.h"
#include "Widget.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct ButtonWidget {
	Object obj;
	const char *label;
	void *ctx;
	void (*fun)(void *ctx);
	bool focused;
};

static const void *getter(const void *iid);

ButtonWidget *ButtonWidget_alloc(const char *label,
	void *ctx, void (*fun)(void *ctx))
{
	ButtonWidget *bw = (ButtonWidget *)Object_alloc(sizeof(ButtonWidget));
	PIG2_SET_GETTER(bw, getter);
	bw->label = label;
	bw->ctx = ctx;
	bw->fun = fun;
	bw->focused = false;
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
		.for_each_child = NULL,
	};
	if (iid == Node_iid) return &Node_impl;

	return Object_getter(iid);
}
