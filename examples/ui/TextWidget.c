#include "TextWidget.h"
#include "Node.h"
#include "Object.h"

struct TextWidget {
	Object obj;
	const char *text;
};

static const void *getter(const void *iid);

TextWidget *TextWidget_alloc(const char *text)
{
	TextWidget *tw = (TextWidget *)Object_alloc(sizeof(TextWidget));
	PIG2_SET_GETTER(tw, getter);
	tw->text = text;
	return tw;
}

static const void *getter(const void *iid)
{
	static const struct Node_impl Node_impl = {
		.label = "TextWidget",
		.for_each_child = NULL,
	};
	if (iid == Node_iid) return &Node_impl;

	return Object_getter(iid);
}
