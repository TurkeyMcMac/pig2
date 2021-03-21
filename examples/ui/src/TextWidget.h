#ifndef TEXT_WIDGET_H_
#define TEXT_WIDGET_H_

// A TextWidget is an Object implementing Widget and Node. It displays multiple
// lines of text on the screen.

struct TextWidget;
typedef struct TextWidget TextWidget;

// Constructs a TextWidget displaying the given text. The text pointer need not
// remain valid after this call. The text is separated into lines by the newline
// characters it contains. A final newline is optional.
TextWidget *TextWidget_alloc(const char *text);

#endif /* TEXT_WIDGET_H_ */
