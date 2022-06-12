#include <gtk/gtk.h>

static int counter = 0;


void file_ok_sel (GtkWidget *w, GtkFileSelection *fs){
  g_print ("%s\n", gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
}

void destroy(GtkWidget *widget, gpointer data){
  gtk_main_quit();
}

void greet_button(GtkWidget *widget, gpointer data){
  g_print("Hi! This is my first GUI program in linux ever!\n");
  g_print("I'm using GTK and C to write this\n");
  g_print("fuck mispi btw\n");
  g_print("You clicked on this button %d times\n", ++counter);
}

void store_filename(GtkWidget *widget, gpointer data){
  GtkWidget *file_selector = GTK_WIDGET (data);
  const gchar *selected_filename = NULL;
  selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector));

  g_print ("Selected filename: %s\n", selected_filename);
  // if(selected_filename != NULL) gtk_close_window(GTK_FILE_SELECTION(widget));
}

void create_fileSW(GtkWidget *widget, gpointer data){
  // gtk_button_released(widget);
  GtkWidget *fileSW; //file selection button
  // Let's create a file selection button 
  fileSW = gtk_file_selection_new("Select directory with libraries");

  // Adding functionality for buttons
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileSW)->ok_button), 
      "clicked", 
      (GtkSignalFunc) store_filename, 
      fileSW); 
  g_signal_connect_swapped (GTK_FILE_SELECTION (fileSW)->ok_button,
    "clicked",
    G_CALLBACK (gtk_widget_destroy), 
    fileSW);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(fileSW)->cancel_button), 
    "clicked",
    (GtkSignalFunc) gtk_widget_destroy, 
    GTK_OBJECT(fileSW));

  // Adding a default path
  // gtk_file_selection_set_filename(GTK_FILE_SELECTION(fileSW), "libs");
  
  gtk_widget_show_all(fileSW);

}


int main(int argc, char* argv[]){

  GtkWidget *window;
  GtkWidget *fileSB;
  GtkWidget *vbox;
  GtkWidget *buttonTest;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  buttonTest = gtk_button_new_with_label("Click me!");

  fileSB = gtk_button_new_with_label("LibDir");
  g_signal_connect(GTK_OBJECT(fileSB), "clicked", G_CALLBACK(create_fileSW), "button");

  // Create a verical box 
  vbox = gtk_vbox_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  
  // Adding a title to the main window:
  gtk_window_set_title(GTK_WINDOW(window), "File finder BIT");

  // Let's change the border for the main window container 
  gtk_container_set_border_width(GTK_CONTAINER(window), 50);
  
  g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
  g_signal_connect(GTK_OBJECT(buttonTest), "clicked", G_CALLBACK(greet_button), "button");

  // gtk_container_add(GTK_CONTAINER(window), buttonTest);
 
  // Adding buttons to vertical box
  gtk_box_pack_start(GTK_BOX(vbox), buttonTest, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), fileSB, TRUE, TRUE, 0);
  
  // gtk_container_add(GTK_CONTAINER(window), fileSB);

  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}
