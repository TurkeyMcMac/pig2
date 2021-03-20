#include "Object.h"
#include "ButtonWidget.h"
#include "GridWidget.h"
#include "Node.h"
#include "ScrollWidget.h"
#include "SpacerWidget.h"
#include "TextWidget.h"
#include "Widget.h"

void button_fun(void *ctx)
{
	fputs("Pressed\n", ctx);
}

int main(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	GridWidget *grid = GridWidget_alloc(7, 7);
	SpacerWidget *sw = SpacerWidget_alloc((struct widget_pair) { 10, 6 },
		(struct widget_pair) { 2, 1 });
	GridWidget_place(grid, 2, 2, Object_add_ref((Object *)sw));
	GridWidget_place(grid, 4, 4, Object_add_ref((Object *)sw));
	GridWidget_place(grid, 1, 1, (Object *)TextWidget_alloc("One line"));
	GridWidget_place(grid, 3, 3,
		(Object *)TextWidget_alloc("test\nfoo\nbar\nbaz"));
	GridWidget_place(grid, 5, 3,
		(Object *)ButtonWidget_alloc("BUTTON", stderr, button_fun));
	GridWidget_place(grid, 1, 5,
		(Object *)TextWidget_alloc("The quick brown fox"));
	GridWidget *grid2 = GridWidget_alloc(3, 3);
	GridWidget_place(grid2, 0, 0,
		(Object *)TextWidget_alloc("Subgrid foo"));
	GridWidget_place(grid2, 1, 1, Object_add_ref((Object *)sw));
	GridWidget_place(grid2, 2, 2,
		(Object *)TextWidget_alloc("Subgrid bar"));
	GridWidget_place(grid2, 2, 2,
		(Object *)TextWidget_alloc("Subgrid bar"));
	GridWidget_place(grid, 3, 5, (Object *)grid2);
	// A placeholder widget is put in before the widget containing the node
	// tree so that the node tree string still describes the structure after
	// the widget displaying it has been added.
	GridWidget_place(grid, 5, 5,
		(Object *)ScrollWidget_alloc(1,
			(Object *)TextWidget_alloc("")));
	String *node_str = Node_tree_to_str(grid);
	GridWidget_place(grid, 5, 5,
		(Object *)ScrollWidget_alloc(10,
			(Object *)TextWidget_alloc(String_cstr(node_str))));
	Object_remove_ref((Object *)node_str);
	Object_remove_ref((Object *)sw);
	void *root = grid;
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
