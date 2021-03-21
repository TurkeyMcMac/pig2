#include "TextWidget.h"
#include "Node.h"
#include "Object.h"
#include "Widget.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct TextWidget {
	Object obj;
	// The maximum line width and the number of lines.
	struct widget_pair dims;
	// The lines stored as a bunch of consecutive NUL-terminated strings.
	char *line_text;
	// The lines stored as dims.y pointers into line_text.
	char *lines[];
};

static const void *getter(const void *iid);

TextWidget *TextWidget_alloc(const char *text)
{
	size_t text_len = strlen(text);
	char *line_text = malloc(text_len + 1);
	assert(line_text);
	struct widget_pair dims = { .x = 0, .y = 0 };
	// tp is a mutable pointer to a part of the text.
	const char *tp = text;
	// Iterate through the newlines.
	for (const char *nl; (nl = strchr(tp, '\n')); tp = nl + 1) {
		// i is the index into text and len is the line length w/o \n.
		size_t i = tp - text, len = nl - tp;
		// Check the maximum line width. This doesn't handle tabs, let
		// alone unicode.
		if ((int)len > dims.x) dims.x = (int)len;
		// Store a copy of the line.
		memcpy(line_text + i, tp, len);
		line_text[i + len] = '\0';
		++dims.y;
	}
	// Check if there is one more line with no terminating \n.
	if ((size_t)(tp - text) < text_len) {
		size_t i = tp - text;
		size_t len = text_len - i;
		if ((int)len > dims.x) dims.x = (int)len;
		memcpy(line_text + i, tp, len);
		++dims.y;
	}
	line_text[text_len] = '\0';
	TextWidget *tw = (TextWidget *)Object_alloc(offsetof(TextWidget, lines)
		+ (size_t)dims.y * sizeof(*tw->lines));
	PIG2_SET_GETTER(tw, getter);
	tw->dims = dims;
	tw->line_text = line_text;
	for (int i = 0; i < dims.y; ++i) {
		// Record the lines in the list of pointers.
		tw->lines[i] = line_text;
		line_text += strlen(line_text) + 1;
	}
	return tw;
}

static void get_requested_dims(void *self_void,
	struct widget_pair *dims, struct widget_pair *min_dims)
{
	const TextWidget *self = self_void;
	*dims = self->dims;
	*min_dims = self->dims;
}

static void draw(void *self_void, WINDOW *win,
	struct widget_pair pos, struct widget_pair dims)
{
	const TextWidget *self = self_void;
	int n_lines = dims.y < self->dims.y ? dims.y : self->dims.y;
	for (int y = 0; y < n_lines; ++y) {
		mvwprintw(win, pos.y + y, pos.x, "%.*s",
			dims.x, self->lines[y]);
	}
}

static bool focus(void *self)
{
	(void)self;
	return false;
}

static void release(Object *self)
{
	free(((TextWidget *)self)->line_text);
	Object_release(self);
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
		.label = "TextWidget",
		.for_each_child = NULL,
	};
	if (iid == Node_iid) return &Node_impl;

	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	return Object_getter(iid);
}
