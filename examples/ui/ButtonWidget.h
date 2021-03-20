#ifndef BUTTON_WIDGET_H_
#define BUTTON_WIDGET_H_

struct ButtonWidget;
typedef struct ButtonWidget ButtonWidget;

ButtonWidget *ButtonWidget_alloc(const char *label,
	void *ctx, void (*fun)(void *ctx));

#endif /* BUTTON_WIDGET_H_ */
