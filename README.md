coditor
=======

A light weight IDE cum text Editor for GNU Linux platform written completely in basic C language using the GTK+ framework.

USAGE
=====

This program is a simple IDE / Text Editor that can be used to create, Edit and run programs. The Compile and Run option works for four languages right now (c,c++,java & python). Any language that can be run and compiled form terminal can be added using simple steps.


Installation Guide
==================

To run this you must have the following libraries present on your linux system.
<br><br>
1) GTK+ Libraries (v3.0)<br>
2) GTK Sourceview Library (v3.0)<br>
3) Gmodule Library(v2.0)<br>
<br>
If any of this library is not present please google how to install these libraries or use a Package installer.
<br>
There are totally 5 Files which follow the MVC model :
<br><br>
  1) coditor.c (the main initialization file with main)<br>
  2) helper.h (with helper functions)<br>
  3) handler.h (with event handlers)<br>
  4) model.h (with model structure)<br>
  5) layout.xml (an XML file with layout specifications)<br>
  <br><br>
This program has been compiled into the output file "Coditor" which has to be run from Terminal.<br>

to compile this program again use the following commands:<br>
  
  1) with warning :<br> 
  ```gcc coditor.c -o ide `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 gtksourceview-3.0` ``` <br>
  <br>2) without warning : 
  ```gcc coditor.c -o ide `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 gtksourceview-3.0`  -w```
  
the output will be generated in the file "ide", which can be called from the terminal.
