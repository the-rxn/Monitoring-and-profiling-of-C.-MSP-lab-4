#include<gtk/gtk.h>

GtkWidget *emailLabel, *emailEntry, *passwordLabel, *passwordEntry, *signupBtn, *grid;

void signup_button_clicked(GtkWidget *wid,gpointer data)
 {
      const gchar *emailData = gtk_entry_get_text(GTK_ENTRY(emailEntry)); 
      gtk_label_set_text(GTK_LABEL(data),emailData); 
      gtk_entry_set_text(GTK_ENTRY(emailEntry),""); 
      gtk_entry_set_text(GTK_ENTRY(PasswordEntry),"");
 } 

static void activate (GtkApplication* app, gpointer user_data)
 {
     GtkWidget *window;
     window = gtk_application_window_new (app);
     gtk_window_set_title (GTK_WINDOW (window), "User Input");
     gtk_window_set_default_size (GTK_WINDOW (window), 500, 400);

     GtkWidget *showEmail; 
     emailLabel = gtk_label_new("Email:"); 
     emailEntry = gtk_entry_new(); 
     gtk_entry_set_placeholder_text(GTK_ENTRY(emailEntry),"Email");
     GIcon *icon; 
     GFile *path; 
     path = g_file_new_for_path("D:/path/emailicon.png"); 
     icon = g_file_icon_new(path); 
     gtk_entry_set_icon_from_gicon(GTK_ENTRY(emailEntry),GTK_ENTRY_ICON_PRIMARY,icon); 

     PasswordLabel = gtk_label_new("Password:");
     PasswordEntry = gtk_entry_new();
     gtk_entry_set_placeholder_text(GTK_ENTRY(PasswordEntry),"Password");
     gtk_entry_set_visibility(GTK_ENTRY(PasswordEntry),FALSE);
     signupBtn = gtk_button_new_with_label("Sign Up");
     showEmail = gtk_label_new("");

     g_signal_connect(signupBtn,"clicked",G_CALLBACK(signup_button_clicked),showEmail);

     GtkWidget *box; box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
     gtk_box_pack_start(GTK_BOX(box),emailLabel,FALSE,FALSE,0);
     gtk_box_pack_start(GTK_BOX(box),emailEntry,FALSE,FALSE,0);
     gtk_box_pack_start(GTK_BOX(box),PasswordLabel,FALSE,FALSE,0);
     gtk_box_pack_start(GTK_BOX(box),PasswordEntry,FALSE,FALSE,0); 
     gtk_box_pack_start(GTK_BOX(box),signupBtn,FALSE,FALSE,0); 
     gtk_box_pack_start(GTK_BOX(box),showEmail,FALSE,FALSE,0); 

     gtk_container_add(GTK_CONTAINER(window),box); 
     gtk_widget_show_all (window);
 } 


 int main(int argc,char **argv)
 {
     GtkApplication *app;
     int status;
     app = gtk_application_new ("com.hackthedeveloper", G_APPLICATION_FLAGS_NONE);
     g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
     status = g_application_run(G_APPLICATION(app), argc, argv);
     g_object_unref (app);
     return status;
 }
