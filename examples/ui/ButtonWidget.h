#ifndef BUTTON_WIDGET_H_
#define BUTTON_WIDGET_H_

#include "Object.h"

struct ButtonWidget;
typedef struct ButtonWidget ButtonWidget;

ButtonWidget *ButtonWidget_alloc(const char *label,
	Object *ctx, void (*fun)(Object *ctx));

#endif /* BUTTON_WIDGET_H_ */
