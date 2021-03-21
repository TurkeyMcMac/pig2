#include "Object.h"
#include "ButtonWidget.h"
#include "GridWidget.h"
#include "Node.h"
#include "ScrollWidget.h"
#include "SpacerWidget.h"
#include "TextWidget.h"
#include "Widget.h"
#include <string.h>

struct button_data {
	Object obj;
	GridWidget *text_holder; // Weak pointer.
	char ch;
};

static Object *make_button_data(GridWidget *text_holder, char ch)
{
	struct button_data *data =
		(struct button_data *)Object_alloc(sizeof(struct button_data));
	data->text_holder = text_holder;
	data->ch = ch;
	return (Object *)data;
}

static void button_fun(Object *ctx)
{
	struct button_data *data = (struct button_data *)ctx;
	char str[] = "Pressed button _";
	str[strlen(str) - 1] = data->ch;
	GridWidget_place(data->text_holder, 0, 0,
		(Object *)TextWidget_alloc(str));
}

static Object *make_button_ui(void)
{
	SpacerWidget *space = SpacerWidget_alloc((struct widget_pair) { 2, 1 },
		(struct widget_pair) { 0, 0 });

	GridWidget *grid = GridWidget_alloc(1, 3);
	GridWidget_place(grid, 0, 0,
		(Object *)TextWidget_alloc("Press these buttons"));
	GridWidget_place(grid, 0, 1, Object_add_ref((Object *)space));
	GridWidget *buttons = GridWidget_alloc(5, 5);
	GridWidget_place(grid, 0, 2, (Object *)buttons);

	GridWidget_place(buttons, 1, 1, Object_add_ref((Object *)space));
	GridWidget_place(buttons, 3, 3, Object_add_ref((Object *)space));

	GridWidget_place(buttons, 0, 0,
		(Object *)ButtonWidget_alloc("1", make_button_data(grid, '1'),
			button_fun));
	GridWidget_place(buttons, 2, 0,
		(Object *)ButtonWidget_alloc("2", make_button_data(grid, '2'),
			button_fun));
	GridWidget_place(buttons, 4, 0,
		(Object *)ButtonWidget_alloc("3", make_button_data(grid, '3'),
			button_fun));
	GridWidget_place(buttons, 0, 2,
		(Object *)ButtonWidget_alloc("4", make_button_data(grid, '4'),
			button_fun));
	GridWidget_place(buttons, 2, 2,
		(Object *)ButtonWidget_alloc("5", make_button_data(grid, '5'),
			button_fun));
	GridWidget_place(buttons, 4, 2,
		(Object *)ButtonWidget_alloc("6", make_button_data(grid, '6'),
			button_fun));
	GridWidget_place(buttons, 0, 4,
		(Object *)ButtonWidget_alloc("7", make_button_data(grid, '7'),
			button_fun));
	GridWidget_place(buttons, 2, 4,
		(Object *)ButtonWidget_alloc("8", make_button_data(grid, '8'),
			button_fun));
	GridWidget_place(buttons, 4, 4,
		(Object *)ButtonWidget_alloc("9", make_button_data(grid, '9'),
			button_fun));

	Object_remove_ref((Object *)space);

	return (Object *)grid;
}

static Object *make_ui(void)
{
	GridWidget *grid = GridWidget_alloc(5, 3);

	SpacerWidget *space = SpacerWidget_alloc((struct widget_pair) { 7, 1 },
		(struct widget_pair) { 1, 0 });
	GridWidget_place(grid, 1, 1, Object_add_ref((Object *)space));
	GridWidget_place(grid, 3, 1, Object_add_ref((Object *)space));
	Object_remove_ref((Object *)space);

	GridWidget_place(grid, 0, 0, (Object *)TextWidget_alloc("Explanation"));
	GridWidget_place(grid, 0, 2,
		(Object *)ScrollWidget_alloc(12,
			(Object *)TextWidget_alloc("TODO: explain")));

	GridWidget_place(grid, 4, 0, (Object *)TextWidget_alloc("Buttons"));
	GridWidget_place(grid, 4, 2, make_button_ui());

	GridWidget_place(grid, 2, 0, (Object *)TextWidget_alloc("Node tree"));
	// A placeholder widget is put in before the widget containing the node
	// tree so that the node tree string still describes the structure after
	// the widget displaying it has been added.
	GridWidget_place(grid, 2, 2,
		(Object *)ScrollWidget_alloc(12,
			(Object *)TextWidget_alloc("")));
	String *node_tree_str = Node_tree_to_str(grid);
	GridWidget_place(grid, 2, 2,
		(Object *)ScrollWidget_alloc(12,
			(Object *)TextWidget_alloc(
				String_cstr(node_tree_str))));
	Object_remove_ref((Object *)node_tree_str);

	return (Object *)grid;
}

int main(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	Object *root = make_ui();
	bool do_draw = true;
	const struct Widget_impl *impl = PIG2_GET(root, Widget_iid);
	bool focused = impl->focus(root);
	for (;;) {
		if (do_draw) {
#ifdef PDCURSES
			resize_term(0, 0);
#endif
			struct widget_pair pos = { .x = 0, .y = 0 };
			struct widget_pair dims = { .x = COLS, .y = LINES };
			struct widget_pair req_dims, req_min_dims;
			impl->get_requested_dims(root,
				&req_dims, &req_min_dims);
			erase();
			impl->draw(root, stdscr, pos, dims);
			do_draw = false;
		}
		int key = getch();
		switch (key) {
		case 'q':
			goto end;
#ifdef KEY_RESIZE
		case KEY_RESIZE:
			do_draw = true;
			break;
#endif
		default:
			if (focused) do_draw = impl->recv_input(root, key);
			break;
		}
	}
end:
	if (focused) impl->unfocus(root);
	Object_remove_ref(root);

	endwin();
}
