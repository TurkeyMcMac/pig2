#ifndef BUTTON_WIDGET_H_
#define BUTTON_WIDGET_H_

#include "Object.h"

// A button is a labelled widget which can be navigated to and which triggers
// an action when pressed using ENTER.

struct ButtonWidget;
typedef struct ButtonWidget ButtonWidget;

// Constructs a ButtonWidget. The label pointer need not remain valid after this
// call. When the button is pressed, it will call fun(ctx). The button takes
// possession of the ctx reference and disposes of it when it is itself
// disposed.
ButtonWidget *ButtonWidget_alloc(const char *label,
	Object *ctx, void (*fun)(Object *ctx));

#endif /* BUTTON_WIDGET_H_ */
