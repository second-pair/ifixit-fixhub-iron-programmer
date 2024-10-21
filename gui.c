/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */



//  *--<Preparations>--*  //

//  Includes
//#include <pthread.h>  //  Must be first - https://www.ibm.com/docs/en/aix/7.2?topic=p-pthread-mutex-initializer-macro
#include "gui.h"

//  Defines
#define LABEL_LEN_NUMBER 16

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
static GtkWidget* label_ser_port = NULL;
static GtkWidget* label_version_curr = NULL;
static GtkWidget* label_spTemp_curr = NULL;
static GtkWidget* text_spTemp_new = NULL;

//  Truly Global Variables

//  Local Prototype Functions
static GtkWidget* gui_layout_create (void);

//  Local Prototype Callbacks
static void cb_btn_ser_connect_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_version_get_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_spTemp_get_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data);
static gboolean cb_spTemp_update_to (gpointer data);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //


static GtkWidget* gui_layout_create (void)
{
	//  Create the top-level container widget.
	GtkWidget* box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign (box_main, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (box_main, GTK_ALIGN_CENTER);

	//  Populate with a title and a button to connect to Serial.
	gtk_box_append (GTK_BOX (box_main), gtk_label_new (TEXT_TL_TITLE));
	gtk_box_append (GTK_BOX (box_main), gtk_label_new (NULL));
	label_ser_port = gtk_label_new ("/dev/ttyACM0");
	gtk_box_append (GTK_BOX (box_main), label_ser_port);
	GtkWidget* btn_ser_connect = gtk_button_new_with_label (TEXT_SER_OPEN);
	g_signal_connect (btn_ser_connect, "clicked", G_CALLBACK (cb_btn_ser_connect_clicked), NULL);
	gtk_box_append (GTK_BOX (box_main), btn_ser_connect);
	gtk_box_append (GTK_BOX (box_main), gtk_label_new (NULL));

	//  Create some initial core parameters.
	GtkWidget* label_version_title = gtk_label_new (TEXT_PARAM_VERSION);
	gtk_box_append (GTK_BOX (box_main), label_version_title);
	label_version_curr = gtk_label_new ("<version>");
	gtk_box_append (GTK_BOX (box_main), label_version_curr);
	GtkWidget* btn_version_get = gtk_button_new_with_label ("Get");
	g_signal_connect (btn_version_get, "clicked", G_CALLBACK (cb_btn_version_get_clicked), NULL);
	gtk_box_append (GTK_BOX (box_main), btn_version_get);
	GtkWidget* label_spTemp_title = gtk_label_new (TEXT_PARAM_SP_TEMP);
	gtk_box_append (GTK_BOX (box_main), label_spTemp_title);
	label_spTemp_curr = gtk_label_new ("<spTemp>");
	gtk_box_append (GTK_BOX (box_main), label_spTemp_curr);
	GtkWidget* btn_spTemp_get = gtk_button_new_with_label ("Get");
	g_signal_connect (btn_spTemp_get, "clicked", G_CALLBACK (cb_btn_spTemp_get_clicked), NULL);
	gtk_box_append (GTK_BOX (box_main), btn_spTemp_get);
	text_spTemp_new = gtk_text_new ();
	gtk_box_append (GTK_BOX (box_main), text_spTemp_new);
	GtkWidget* btn_spTemp_set = gtk_button_new_with_label ("Set");
	g_signal_connect (btn_spTemp_set, "clicked", G_CALLBACK (cb_btn_spTemp_set_clicked), NULL);
	gtk_box_append (GTK_BOX (box_main), btn_spTemp_set);

	return box_main;
}


//  Update API

void gui_spTemp_update (uint16_t newSp)
{
	_LOG (4, "SP Temp:  %u\n", newSp);
	g_idle_add_full (G_PRIORITY_LOW, cb_spTemp_update_to, (gpointer)(uintptr_t)newSp, NULL);
}


//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

void cb_app_main_activate (GtkApplication* theApp, gpointer data)
{
	//  Create the window.
	GtkWidget* window_main = gtk_application_window_new (theApp);
	gtk_window_set_title (GTK_WINDOW (window_main), GUI_TITLE);

	//  Determine how we want to size the window.
	#if (GUI_WINDOW_MODE == 1)
		gtk_window_set_default_size (GTK_WINDOW (window_main), GUI_WINDOW_SIZE_W, GUI_WINDOW_SIZE_H);
	#elif(GUI_WINDOW_MODE == 2)
		gtk_window_fullscreen (GTK_WINDOW (window_main));
	#endif

	//  Create its child and present it.
	gtk_window_set_child (GTK_WINDOW (window_main), gui_layout_create ());
	gtk_window_present (GTK_WINDOW (window_main));
}

static void cb_btn_ser_connect_clicked (GtkButton* theButton, gpointer data)
{
	//  Toggle open-ness of the Serial port.
	if (serial_isOpen ())
	{
		serial_close ();
		gtk_button_set_label (theButton, TEXT_SER_OPEN);
	}
	else
	{
		serial_init (gtk_label_get_text (GTK_LABEL (label_ser_port)));
		//  Only update thee button's label if we opened successfully.
		if (serial_isOpen ())
			gtk_button_set_label (theButton, TEXT_SER_CLOSE);
	}
}

//  Getter buttons.
static void cb_btn_version_get_clicked (GtkButton* theButton, gpointer data)
{
	serial_cmd_noParams_submit (ironCmdType_version_get);
}
static void cb_btn_spTemp_get_clicked (GtkButton* theButton, gpointer data)
{
	serial_cmd_noParams_submit (ironCmdType_spTemp_get);
}
static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data)
{
	GtkEntryBuffer* buffer = gtk_text_get_buffer (GTK_TEXT (text_spTemp_new));
	const char* text = gtk_entry_buffer_get_text (buffer);
	int decode = strtol (text, NULL, 10);
	if (decode > INT16_MAX) decode = INT16_MAX;
	if (decode < INT16_MIN) decode = INT16_MIN;
	uint16_t newSp = (uint16_t)decode;
	ironCommand* ironCmd = malloc (sizeof (ironCommand));
	_NULL_EXIT (ironCmd);
	ironCmd -> type = ironCmdType_spTemp_set;
	snprintf (ironCmd -> params, SERIAL_PARAM_SIZE, "%u", newSp);
	serial_cmd_submit (ironCmd);
}

//  Setter buttons.
//static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data)


//  Update Timeouts
static gboolean cb_spTemp_update_to (gpointer data)
{
	char updateText [LABEL_LEN_NUMBER];
	snprintf (updateText, LABEL_LEN_NUMBER, "%u", (uint16_t)(uintptr_t)data);
	gtk_label_set_text (GTK_LABEL (label_spTemp_curr), updateText);
	return 0;
}


//  *--</Callbacks>--*  //
