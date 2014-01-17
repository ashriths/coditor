coditor
=======

A light weight IDE cum text Editor for GNU Linux platform written completely in basic C language using the GTK+ framework.

USAGE
=====

This program is a simple IDE / Text Editor that can be used to create, Edit and run programs. The Compile and Run option works for four languages right now (c,c++,java & python). Any language that can be run and compiled form terminal can be added using simple steps.


Installation Guide
==================

To run this you must have the following libraries present on your linux system.

1) GTK+ Libraries (v3.0)
2) GTK Sourceview Library (v3.0)
3) Gmodule Library(v2.0)

If any of this library is not present please google how to install these libraries or use a Package installer.

There are totally 5 Files which follow the MVC model :

  1) coditor.c (the main initialization file with main)
  2) helper.h (with helper functions)
  3) handler.h (with event handlers)
  4) model.h (with model structure)
  5) layout.xml (an XML file with layout specifications)
  
This program has been compiled into the output file "Coditor" which has to be run from Terminal.

to compile this program again use the following commands:
  
  1) with warning : gcc coditor.c -o ide `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 gtksourceview-3.0` 
  2) without warnign : gcc coditor.c -o ide `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 gtksourceview-3.0` -w
  
the ouptput will be generated in the file "ide", which can be called from the terminal.
