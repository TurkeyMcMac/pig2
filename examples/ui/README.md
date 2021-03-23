# UI Example

This example draws a simple ncurses UI on the terminal. Some of the code is
complex without serving to exemplify pig2, as it turns out that UIs are
complicated, even when very simplified. The program defines a somewhat flexible
framework that allows for the UI to adapt to the screen size dynamically.

## Building

To build this on Unix, you can most likely just run `make` in this directory.
You can also look in the Makefile and follow the directions manually.

## Concepts in This Example

### General Implementation and Usage

You can find interface implementation code in most of the src/\*.c files.
[src/main.c](src/main.c) has a good example of method usage.

### Interfaces

This program contains a few interfaces, whose headers are as follows:

* [Node](src/Node.h)
* [Object](src/Object.h) (Containing the abstract methods of the Object class)
* [Widget](src/Widget.h)

### Classes

Several classes implement the interfaces. Their headers are below. You can look
at the Object class to see one way to create extendable super classes.

* [Object](src/Object.h)
  * [ButtonWidget](src/ButtonWidget.h) (Implementing Widget and Node)
  * [GridWidget](src/GridWidget.h) (Implementing Widget and Node)
  * [ScrollWidget](src/ScrollWidget.h) (Implementing Widget and Node)
  * [SpacerWidget](src/SpacerWidget.h) (Implementing Widget and Node)
  * [String](src/String.h)
  * [TextWidget](src/TextWidget.h) (Implementing Widget and Node)

(All of these classes also implement the Object "interface" in that they have
implementations of the abstract methods of the Object class.)
