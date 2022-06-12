#include <stdio.h>
#include <stdlib.h>
#include "requests.h"
#include <gtk/gtk.h>
static int counter = 0;

GtkWidget *window;
GtkWidget *vbox;
GtkWidget *refreshButton, *GetPriceButton;
GtkWidget *tokenEntry, *tokenLabel;
GtkWidget *showToken;
GtkWidget *priceOfToken;
gchar *tokenData;

double tokenPrice;
char tokenPriceChar[15];

void * memmem(const void * haystack, size_t hlen,
    const void * needle, size_t nlen) {
    int needle_first;
    const void * p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = * (unsigned char * ) needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1))) {
        if (!memcmp(p, needle, nlen))
            return (void * ) p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}

double get_price(char* token){
  char url[512] = "https://api.coingecko.com/api/v3/simple/price?ids="; //solana&vs_currencies=usd";
  char ending[20] = "&vs_currencies=usd";
  strcat(url, token);
  strcat(url, ending);
  // printf("%s", url);
  double result;
  req_t req;                     /* declare struct used to store data */
  int ret = requests_init(&req); /* setup */
  if (ret) {
    return 1;
  }
  
  requests_get(&req, url); /* submit GET request */
  // printf("Request URL: %s\n", req.url);
  // printf("Response Code: %lu\n", req.code);
  // printf("Response Body:\n%s\n", req.text);

  char price[4];
  // price[4] = 0;
  void *pos = memmem(req.text, strlen(req.text), "usd", 3);
  // printf("pos = %s", (char*)pos+5);
  memcpy(price, (char*)pos+5, 4);
  sscanf(price, "%lf", &result);
  // printf("%lf", result);
  requests_close(&req); /* clean up */
  return result;
}

void destroy(GtkWidget *widget, gpointer data){
  gtk_main_quit();
}

void setToken_clicked(GtkWidget *wid,gpointer data){
  tokenData = gtk_entry_get_text(GTK_ENTRY(tokenEntry));
  printf("Got token name: %s\n", tokenData);
  updateTokenPrice(); 
  // priceOfToken = gtk_label_new("123"); 

  gtk_label_set_text(GTK_LABEL(data),tokenData); 
  gtk_entry_set_text(GTK_ENTRY(tokenEntry),""); 
} 
void greet_button(GtkWidget *widget, gpointer data){
  g_print("Hi! This is my first GUI program in linux ever!\n");
  g_print("I'm using GTK and C to write this\n");
  g_print("fuck mispi btw\n");
  g_print("You clicked on this button %d times\n", ++counter);
}

void updateTokenPrice(){
  double tmpPrice = get_price(tokenData);
  char usdt[6] = " USDT";
  
  printf("tmpPrice = %lf\n", tmpPrice);
  snprintf(tokenPriceChar, 5, "%f", tmpPrice);
  strcat(tokenPriceChar, usdt);
  gtk_label_set_text(priceOfToken, tokenPriceChar);
}

void refresh_button(GtkWidget *widget, gpointer data){
  printf("entered refresh_button()\n");
  updateTokenPrice();
}


int main(int argc, char* argv[]){
    // printf("%lf", solPrice);
  gtk_init(&argc, &argv);

  showToken = gtk_label_new("");
  priceOfToken= gtk_label_new("");

  GetPriceButton = gtk_button_new_with_label("Get Price");

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  refreshButton = gtk_button_new_with_label("Referesh");
//make token input
  tokenLabel = gtk_label_new("Enter token name:");
  tokenEntry = gtk_entry_new();



  gtk_entry_set_placeholder_text(GTK_ENTRY(tokenEntry), "e.g. solana");
  gtk_entry_set_visibility(GTK_ENTRY(tokenEntry), TRUE);
  // const gchar* tokenName = gtk_entry_get_text(GTK_ENTRY(tokenEntry));

  g_signal_connect(GetPriceButton,"clicked",G_CALLBACK(setToken_clicked), showToken);
// Create a verical box 
  vbox = gtk_box_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  
  // Adding a title to the main window:
  gtk_window_set_title(GTK_WINDOW(window), "CoinGecko Price Checker");

  // Let's change the border for the main window container 
  gtk_container_set_border_width(GTK_CONTAINER(window), 50);
  
  g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
  g_signal_connect(G_OBJECT(refreshButton), "clicked", G_CALLBACK(refresh_button), "button");

  // gtk_container_add(GTK_CONTAINER(window), refreshButton);
    // Adding buttons to vertical box
  gtk_box_pack_start(GTK_BOX(vbox), refreshButton, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), tokenLabel, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), tokenEntry, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), showToken, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), GetPriceButton, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), priceOfToken, FALSE, FALSE, 0);

  // gtk_container_add(GTK_CONTAINER(window), fileSB);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
