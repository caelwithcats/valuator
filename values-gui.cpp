#include <gtk/gtk.h>
#include <string>
// yeah, master and slave will make github very angry.
// Hate you github

GtkBuilder *gladeBuilder;
GtkWidget *masterWindow;
GtkWidget *slaveBtnAddValue;
GtkWidget *slaveTextFieldValueName;
GtkWidget *slaveTextFieldValueType;
GtkWidget *slaveValuesTable;
GtkWidget *slaveMenuItemAbout;
// anyway, compile using: g++ values-gui.cpp  `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

std::string inputedVarValue;
std::string inputedVarName;
std::string inputedVarType;

GtkWidget* dialogueValue;
GtkWidget *dialogueValueWidget;
GtkTreeStore *store;

enum columnData{
  COL_NAME = 0,
  COL_TYPE = 1,
  COL_DATA = 2,
  NUM_COLS
};
void event_insert_text(GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data)
{
    int i;

    for (i = 0; i < length; i++) {
        if (!isdigit(text[i])) {
            g_signal_stop_emission_by_name(G_OBJECT(editable), "insert-text");
            return;
        }
    }
}
void event_get_dialogue_result(GtkWidget *widget, gpointer data) { 
  inputedVarValue = gtk_entry_get_text(GTK_ENTRY(dialogueValueWidget));
  g_print(std::string("printer: name=" + inputedVarName + " type=" + inputedVarType + " value=" +  inputedVarValue + "\n").c_str());
  GtkTreeIter   iter;  
  gtk_tree_store_append (store, &iter, NULL); 
  gtk_tree_store_set (store, &iter,
                    COL_NAME, inputedVarName.c_str(),
                    COL_TYPE, inputedVarType.c_str(),
                    COL_DATA, inputedVarValue.c_str(),
                    -1);
  gtk_widget_destroy(dialogueValue);
};
void event_dialogue_destroy( GtkWidget *widget, gpointer   data )
{
  gtk_widget_destroy(widget);
}
void events_about_box(GtkMenuItem *m) {
  g_print("About pressed");
}
void events_addvalue_pressed(GtkWidget *widget, gpointer data) {
  inputedVarName = gtk_entry_get_text(GTK_ENTRY(slaveTextFieldValueName));
  inputedVarType  = gtk_entry_get_text(GTK_ENTRY(slaveTextFieldValueType));
  g_print(std::string("Add button clicked with the value name being " + inputedVarName  + " and the value type being " + inputedVarType + "\n").c_str());
  if(!inputedVarName.empty() && !inputedVarType.empty()){
  dialogueValue = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(dialogueValue), "Set the value");
  gtk_window_set_default_size(GTK_WINDOW(dialogueValue),200,50);
  gtk_window_set_transient_for(GTK_WINDOW(dialogueValue),GTK_WINDOW(masterWindow));

  GtkWidget *dialogueBox;
  GtkWidget *dialogueOkBtn;
  dialogueBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  gtk_container_add(GTK_CONTAINER(dialogueValue), dialogueBox);

  if(inputedVarType == "words"){
    dialogueValueWidget = gtk_entry_new();
  }else if(inputedVarType == "number"){
    dialogueValueWidget = gtk_entry_new();
    g_signal_connect(G_OBJECT(dialogueValueWidget), "insert-text", G_CALLBACK(event_insert_text), NULL);
  }
  
  dialogueOkBtn = gtk_button_new_with_mnemonic("_OK");
  g_signal_connect(G_OBJECT(dialogueOkBtn),"clicked", G_CALLBACK(event_get_dialogue_result), NULL);
  gtk_box_pack_start(GTK_BOX(dialogueBox),dialogueValueWidget, false,true,20);
  gtk_box_pack_start(GTK_BOX(dialogueBox),dialogueOkBtn, true,true,10);
  g_signal_connect_swapped (dialogueValue,"destroy",G_CALLBACK (event_dialogue_destroy),dialogueValue);
  gtk_entry_set_text(GTK_ENTRY(slaveTextFieldValueName),"");
  gtk_widget_show_all (dialogueValue);
  }else{
    GtkWidget *dialogue;
    dialogue =  gtk_message_dialog_new(GTK_WINDOW(masterWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"The value type and name must be set",g_strerror(errno));
    gtk_window_set_title(GTK_WINDOW(dialogue),"Error!");
    gtk_dialog_run(GTK_DIALOG(dialogue));
    gtk_widget_destroy(dialogue);
  }
}

int main(int argc, char *argv[]) {
  gtk_init (&argc, &argv);

  gladeBuilder = gtk_builder_new();
  gtk_builder_add_from_file(gladeBuilder, "gladefile.glade", NULL);

  masterWindow = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "masterWindow"));
  slaveBtnAddValue = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "slaveBtnAddValue"));
  slaveTextFieldValueName = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "slaveTextFieldValueName"));
  slaveTextFieldValueType = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "slaveTextFieldValueType"));
  slaveValuesTable = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "slaveValuesTable"));
  slaveMenuItemAbout = GTK_WIDGET(gtk_builder_get_object(gladeBuilder, "slaveMenuItemAbout"));
  //tableValues = GTK_TREE_STORE(gtk_builder_get_object(gladeBuilder, "tableValues"));

  gtk_window_set_title(GTK_WINDOW(masterWindow),"Valuator");
  store = gtk_tree_store_new (NUM_COLS,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(slaveValuesTable),GTK_TREE_MODEL (store));
  g_object_unref (G_OBJECT (store));
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new (); 
  column = gtk_tree_view_column_new_with_attributes ("Name",
                                                   renderer,
                                                   "text", COL_NAME,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (slaveValuesTable), column);

  column = gtk_tree_view_column_new_with_attributes ("Type",
                                                   renderer,
                                                   "text", COL_TYPE,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (slaveValuesTable), column);

  column = gtk_tree_view_column_new_with_attributes ("Value",
                                                   renderer,
                                                   "text", COL_DATA,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (slaveValuesTable), column);




  g_object_unref(G_OBJECT(gladeBuilder));
  gtk_widget_show(masterWindow);
  g_signal_connect(G_OBJECT(slaveMenuItemAbout), "activate", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(slaveBtnAddValue), "clicked", G_CALLBACK(events_addvalue_pressed), NULL);
  g_signal_connect (masterWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_main();
  return 0;
}
