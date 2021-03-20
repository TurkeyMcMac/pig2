#include "SpacerWidget.h"
#include "Node.h"
#include "Object.h"
#include "Widget.h"
#include <assert.h>

struct SpacerWidget {
	Object obj;
	struct widget_pair dims, min_dims;
};

static const void *getter(const void *iid);

SpacerWidget *SpacerWidget_alloc(
	struct widget_pair dims, struct widget_pair min_dims)
{
	assert(min_dims.x >= 0 && min_dims.y >= 0);
	assert(dims.x >= min_dims.x && dims.y >= min_dims.y);
	SpacerWidget *sw = (SpacerWidget *)Object_alloc(sizeof(SpacerWidget));
	PIG2_SET_GETTER(sw, getter);
	sw->dims = dims;
	sw->min_dims = min_dims;
	return sw;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	const SpacerWidget *self = self_void;
	*dims = self->dims;
	*min_dims = self->min_dims;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	(void)self_void, (void)win, (void)pos, (void)dims;
}

static bool focus(void *self)
{
	(void)self;
	return false;
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
		.label = "SpacerWidget",
		.for_each_child = NULL,
	};
	if (iid == Node_iid) return &Node_impl;

	return Object_getter(iid);
}
