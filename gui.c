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
static GtkWidget* text_ser_port = NULL;
static GtkWidget* label_version_curr = NULL;
static GtkWidget* label_spTemp_curr = NULL;
static GtkWidget* text_spTemp_new = NULL;
static GtkWidget* label_maxTemp_curr = NULL;
static GtkWidget* text_maxTemp_new = NULL;

//  Truly Global Variables

//  Local Prototype Functions
static GtkWidget* priv_layout_create (void);
static GtkWidget* priv_sep_create (GtkOrientation orientation);
static GtkWidget* priv_ser_create (void);
static GtkWidget* priv_core_create (void);
static GtkWidget* priv_setpoints_create (void);
static GtkWidget* priv_aux_create (void);
static GtkWidget* priv_config_create (void);


//  Local Prototype Callbacks
static void cb_btn_ser_connect_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_maxTemp_set_clicked (GtkButton* theButton, gpointer data);
static gboolean cb_spTemp_update_to (gpointer data);
static gboolean cb_maxTemp_update_to (gpointer data);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //


static GtkWidget* priv_layout_create (void)
{
	//  Create the top-level container widget & a title.
	GtkWidget* box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign (box_main, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (box_main, GTK_ALIGN_CENTER);
	gtk_box_append (GTK_BOX (box_main), gtk_label_new (TEXT_TL_TITLE));
	gtk_box_append (GTK_BOX (box_main), gtk_label_new (NULL));

	//  Append the section widgets.
	gtk_box_append (GTK_BOX (box_main), priv_ser_create ());
	gtk_box_append (GTK_BOX (box_main), priv_sep_create (GTK_ORIENTATION_HORIZONTAL));
	gtk_box_append (GTK_BOX (box_main), priv_core_create ());
	gtk_box_append (GTK_BOX (box_main), priv_sep_create (GTK_ORIENTATION_HORIZONTAL));
	gtk_box_append (GTK_BOX (box_main), priv_setpoints_create ());
	gtk_box_append (GTK_BOX (box_main), priv_sep_create (GTK_ORIENTATION_HORIZONTAL));
	gtk_box_append (GTK_BOX (box_main), priv_aux_create ());
	gtk_box_append (GTK_BOX (box_main), priv_sep_create (GTK_ORIENTATION_HORIZONTAL));
	gtk_box_append (GTK_BOX (box_main), priv_config_create ());

	/*  Advanced
	PID Kp, Ki, Kd, Max i
	Control loop period uS
	*/

	return box_main;
}

static GtkWidget* priv_sep_create (GtkOrientation orientation)
{
	GtkWidget* separator = gtk_separator_new (orientation);
	gtk_widget_set_margin_top (separator, 10);
	gtk_widget_set_margin_bottom (separator, 10);
	return separator;
}

//  Section functions for ^.
static GtkWidget* priv_ser_create (void)
{
	//  Create Serial connection widgets.
	//#  This should end up being some kind of popup.
	GtkWidget* box_serial = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_halign (box_serial, GTK_ALIGN_CENTER);
	GtkEntryBuffer* buffer_ser_port = gtk_entry_buffer_new (GUI_PORT_DEFAULT, -1);
	text_ser_port = gtk_text_new_with_buffer (buffer_ser_port);
	gtk_box_append (GTK_BOX (box_serial), text_ser_port);
	GtkWidget* btn_ser_connect = gtk_button_new_with_label (TEXT_SER_OPEN);
	g_signal_connect (btn_ser_connect, "clicked", G_CALLBACK (cb_btn_ser_connect_clicked), NULL);
	gtk_box_append (GTK_BOX (box_serial), btn_ser_connect);
	return box_serial;
}
static GtkWidget* priv_core_create (void)
{
	/*  Core Readouts
	Switch ON/OFF   State IDLE
	Switch Status [heater details]
	Curr Temp, Curr Power / Duty Cycle % [heater details / heater controlloop]
	Status/State [heater details]
	*/
	GtkWidget* grid_core = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_core), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_core), 1);
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new (TEXT_CORE_TITLE), 0, 0, 4, 1);

	/*  (Intended) States:
	Switch Off
	Sleep
	Idle
	Heating
	Cooling
	At Temperature
	No Tip
	*/
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new (TEXT_CORE_STATE), 1, 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new ("Switch Off"), 2, 1, 1, 1);

	//  Live Readouts
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new (TEXT_CORE_READOUT), 0, 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new ("<live temp>"), 1, 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new ("<live power>"), 2, 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new ("<live DC>"), 3, 2, 1, 1);

	return grid_core;
}
static GtkWidget* priv_setpoints_create (void)
{
	/*  Setpoints
	SP Temp [settings activetemp]
	SP Min Temp [seems to be fixed at 200], SP Max Temp [settings maxtemp]
	*/
	GtkWidget* grid_setpoints = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_setpoints), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_setpoints), 1);
	gtk_grid_attach (GTK_GRID (grid_setpoints), gtk_label_new (TEXT_SETPOINTS_TITLE), 0, 0, 4, 1);

	//  Setpoint Temp
	gtk_grid_attach (GTK_GRID (grid_setpoints), gtk_label_new (TEXT_SETPOINTS_TEMP), 0, 1, 1, 1);
	label_spTemp_curr = gtk_label_new ("<spTemp>");
	gtk_grid_attach (GTK_GRID (grid_setpoints), label_spTemp_curr, 1, 1, 1, 1);
	GtkEntryBuffer* buffer_spTemp_new = gtk_entry_buffer_new (GUI_SP_TEMP_DEFAULT, -1);
	text_spTemp_new = gtk_text_new_with_buffer (buffer_spTemp_new);
	gtk_grid_attach (GTK_GRID (grid_setpoints), text_spTemp_new, 2, 1, 1, 1);
	GtkWidget* btn_spTemp_set = gtk_button_new_with_label ("Set");
	g_signal_connect (btn_spTemp_set, "clicked", G_CALLBACK (cb_btn_spTemp_set_clicked), NULL);
	gtk_grid_attach (GTK_GRID (grid_setpoints), btn_spTemp_set, 3, 1, 1, 1);

	//  Max Temp
	gtk_grid_attach (GTK_GRID (grid_setpoints), gtk_label_new (TEXT_SETPOINTS_TEMP_MAX), 0, 2, 1, 1);
	label_maxTemp_curr = gtk_label_new ("<maxTemp>");
	gtk_grid_attach (GTK_GRID (grid_setpoints), label_maxTemp_curr, 1, 2, 1, 1);
	GtkEntryBuffer* buffer_maxTemp_new = gtk_entry_buffer_new (GUI_MAX_TEMP_DEFAULT, -1);
	text_maxTemp_new = gtk_text_new_with_buffer (buffer_maxTemp_new);
	gtk_grid_attach (GTK_GRID (grid_setpoints), text_maxTemp_new, 2, 2, 1, 1);
	GtkWidget* btn_maxTemp_set = gtk_button_new_with_label ("Set");
	g_signal_connect (btn_maxTemp_set, "clicked", G_CALLBACK (cb_btn_maxTemp_set_clicked), NULL);
	gtk_grid_attach (GTK_GRID (grid_setpoints), btn_maxTemp_set, 3, 2, 1, 1);

	return grid_setpoints;
}
static GtkWidget* priv_aux_create (void)
{
	/*  Secondary Readouts
	Tip Installed [heater details]
	Serial Number [otp read], MCU S/N [mcu_sn], Version [otp read, version]
	Uptime [uptime]
	Fault Mask [heater faultmask]
	Accelerometer [mxc4005 magnitude]
	*/
	GtkWidget* grid_aux = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_aux), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_aux), 1);
	gtk_grid_attach (GTK_GRID (grid_aux), gtk_label_new (TEXT_AUX_TITLE), 0, 0, 3, 1);
	GtkWidget* label_version_title = gtk_label_new (TEXT_AUX_VERSION);
	gtk_grid_attach (GTK_GRID (grid_aux), label_version_title, 0, 1, 1, 1);
	label_version_curr = gtk_label_new ("<version>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_version_curr, 2, 1, 1, 1);
	return grid_aux;
}
static GtkWidget* priv_config_create (void)
{
	/*  Configuration
	Celsius/Farenheit [settings units]
	Idle Enable [settings idletimerenable], Idle Timer [settings idletimer], Idle Temp [settings idletemp]
	Sleep Enable [settings sleeptimerenable], Sleep Timer [settings sleeptimer]
	Temperature Correction [settings tempcorrection]
	Reboot [bootloader reboot / reset]
	Reset
	*/
	GtkWidget* grid_config = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_config), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_config), 1);
	gtk_grid_attach (GTK_GRID (grid_config), gtk_label_new (TEXT_CONFIG_TITLE), 0, 0, 3, 1);
	return grid_config;
}


//  Update API
void gui_spTemp_update (uint16_t newValue)
{
	_LOG (4, "SP Temp:  %u\n", newValue);
	g_idle_add_full (G_PRIORITY_LOW, cb_spTemp_update_to, (gpointer)(uintptr_t)newValue, NULL);
}
void gui_maxTemp_update (uint16_t newValue)
{
	_LOG (4, "Max Temp:  %u\n", newValue);
	g_idle_add_full (G_PRIORITY_LOW, cb_maxTemp_update_to, (gpointer)(uintptr_t)newValue, NULL);
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
	gtk_window_set_child (GTK_WINDOW (window_main), priv_layout_create ());
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
		GtkEntryBuffer* buffer = gtk_text_get_buffer (GTK_TEXT (text_ser_port));
		serial_init (gtk_entry_buffer_get_text (buffer));
		//  Only update thee button's label if we opened successfully.
		if (serial_isOpen ())
			gtk_button_set_label (theButton, TEXT_SER_CLOSE);
	}
}

//  Helper functions for the Setter Buttons
#define text_cmdSubmit_number(cmdType, theText, numType, numMin, numMax) \
({ \
	GtkEntryBuffer* buffer = gtk_text_get_buffer (GTK_TEXT (theText)); \
	const char* text = gtk_entry_buffer_get_text (buffer); \
	int decode = strtol (text, NULL, 10); \
	if (decode < numMin) decode = numMin; \
	if (decode > numMax) decode = numMax; \
	numType newSp = (numType)decode; \
	ironCommand* ironCmd = malloc (sizeof (ironCommand)); \
	_NULL_EXIT (ironCmd); \
	ironCmd -> type = cmdType; \
	snprintf (ironCmd -> params, SERIAL_PARAM_SIZE, "%u", newSp); \
	serial_cmd_submit (ironCmd); \
})

//  Setter Buttons
static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data)
{
	text_cmdSubmit_number (ironCmdType_spTemp_set, text_spTemp_new,
		uint16_t, INT16_MIN, INT16_MAX);
}
static void cb_btn_maxTemp_set_clicked (GtkButton* theButton, gpointer data)
{
	text_cmdSubmit_number (ironCmdType_maxTemp_set, text_maxTemp_new,
		uint16_t, INT16_MIN, INT16_MAX);
}


//  Helper functions for the Update Timeouts
#define label_update(theLabel, theNumber, numType) \
({ \
	char updateText [LABEL_LEN_NUMBER]; \
	snprintf (updateText, LABEL_LEN_NUMBER, "%u", (numType)(uintptr_t)theNumber); \
	gtk_label_set_text (GTK_LABEL (theLabel), updateText); \
	return 0; \
})

//  Update Timeouts
static gboolean cb_spTemp_update_to (gpointer data)
	{  label_update (label_spTemp_curr, data, uint16_t);  }
static gboolean cb_maxTemp_update_to (gpointer data)
	{  label_update (label_maxTemp_curr, data, uint16_t);  }

//  *--</Callbacks>--*  //
