# Converting the CLI into GUI
This is going to be first time writing a GUI application for me

## Used stack:
>
  1. C 
  2. GTK2 

## Steps:
>
  1. Create a window 
  2. Create a subwindow inside it with the list of available libraries.
  3. Create 2 buttons:
    3.1 Choose library folder (should open a file explorer)
    3.2 Choose scanning folder (should open a file explorer)
  4. Add an options button:
    4.1. When clicked, should open a pop up window with the list of available options.
    4.2. Probably will do them as toggles? https://www.cc.gatech.edu/data_files/public/doc/gtk/tutorial/gtk_tut-6.html
      4.2.1. And 
      4.2.2. Or 
      4.2.3. Negative 
  5. Combine all of this shit together.
    5.1. Rewrite main in ./libaotN3246.c
    5.2. Maybe I shouldn't rewrite it but copy it into ./gui.c and refactor it here?

