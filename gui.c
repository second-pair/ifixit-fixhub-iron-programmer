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
//  Serial
//  Core Readouts
static GtkWidget* label_state = NULL;
static GtkWidget* label_liveTemp = NULL;
static GtkWidget* label_livePower = NULL;
static GtkWidget* label_liveDutyCycle = NULL;
//  Setpoints
static GtkWidget* label_spTemp_curr = NULL;
static GtkWidget* text_spTemp_new = NULL;
static GtkWidget* label_maxTemp_curr = NULL;
static GtkWidget* text_maxTemp_new = NULL;
//  Auxiliary Readouts
static GtkWidget* label_uptime = NULL;
static GtkWidget* label_faults = NULL;
static GtkWidget* label_version = NULL;
static GtkWidget* label_sn_device = NULL;
static GtkWidget* label_sn_mcu = NULL;
//  Configuration
static GtkWidget* sw_idleEnable = NULL;
static GtkWidget* text_idleTimer = NULL;
static GtkWidget* text_idleTemp = NULL;
static GtkWidget* sw_sleepEnable = NULL;
static GtkWidget* text_sleepTimer = NULL;
static GtkWidget* sw_units = NULL;
static GtkWidget* text_calTemp = NULL;
static GtkWidget* label_idleEnable = NULL;
static GtkWidget* label_idleTimer = NULL;
static GtkWidget* label_idleTemp = NULL;
static GtkWidget* label_sleepEnable = NULL;
static GtkWidget* label_sleepTimer = NULL;
static GtkWidget* label_units = NULL;
static GtkWidget* label_calTemp = NULL;

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
static void cb_btn_reset_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_reboot_clicked (GtkButton* theButton, gpointer data);

static void cb_btn_spTemp_set_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_maxTemp_set_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_idle_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_sleep_clicked (GtkButton* theButton, gpointer data);
static void cb_btn_calTemp_clicked (GtkButton* theButton, gpointer data);

static gboolean cb_spTemp_update_to (gpointer data);
static gboolean cb_maxTemp_update_to (gpointer data);
//static gboolean cb_idleEnable_update_to (gpointer data);
//static gboolean cb_idleTimer_update_to (gpointer data);
//static gboolean cb_idleTemp_update_to (gpointer data);
//static gboolean cb_sleepEnable_update_to (gpointer data);
//static gboolean cb_sleepTimer_update_to (gpointer data);
//static gboolean cb_units_update_to (gpointer data);
//static gboolean cb_calTemp_update_to (gpointer data);

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
	Status/State [heater details]
	Curr Temp, Curr Power / Duty Cycle % [heater details / heater controlloop]
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
	label_state = gtk_label_new ("<state>");
	gtk_grid_attach (GTK_GRID (grid_core), label_state, 2, 1, 1, 1);

	//  Live Readouts
	gtk_grid_attach (GTK_GRID (grid_core), gtk_label_new (TEXT_CORE_READOUT), 0, 2, 1, 1);
	label_liveTemp = gtk_label_new ("<live temp>");
	gtk_grid_attach (GTK_GRID (grid_core), label_liveTemp, 1, 2, 1, 1);
	label_livePower = gtk_label_new ("<live power>");
	gtk_grid_attach (GTK_GRID (grid_core), label_livePower, 2, 2, 1, 1);
	label_liveDutyCycle = gtk_label_new ("<live DC>");
	gtk_grid_attach (GTK_GRID (grid_core), label_liveDutyCycle, 3, 2, 1, 1);

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
	GtkWidget* btn_spTemp_set = gtk_button_new_with_label (TEXT_SET);
	g_signal_connect (btn_spTemp_set, "clicked", G_CALLBACK (cb_btn_spTemp_set_clicked), NULL);
	gtk_grid_attach (GTK_GRID (grid_setpoints), btn_spTemp_set, 3, 1, 1, 1);

	//  Max Temp
	gtk_grid_attach (GTK_GRID (grid_setpoints), gtk_label_new (TEXT_SETPOINTS_TEMP_MAX), 0, 2, 1, 1);
	label_maxTemp_curr = gtk_label_new ("<maxTemp>");
	gtk_grid_attach (GTK_GRID (grid_setpoints), label_maxTemp_curr, 1, 2, 1, 1);
	GtkEntryBuffer* buffer_maxTemp_new = gtk_entry_buffer_new (GUI_MAX_TEMP_DEFAULT, -1);
	text_maxTemp_new = gtk_text_new_with_buffer (buffer_maxTemp_new);
	gtk_grid_attach (GTK_GRID (grid_setpoints), text_maxTemp_new, 2, 2, 1, 1);
	GtkWidget* btn_maxTemp_set = gtk_button_new_with_label (TEXT_SET);
	g_signal_connect (btn_maxTemp_set, "clicked", G_CALLBACK (cb_btn_maxTemp_set_clicked), NULL);
	gtk_grid_attach (GTK_GRID (grid_setpoints), btn_maxTemp_set, 3, 2, 1, 1);

	return grid_setpoints;
}
static GtkWidget* priv_aux_create (void)
{
	/*  Auxiliary Readouts
	Uptime [uptime]
	Fault Mask [heater faultmask]
	Accelerometer [mxc4005 magnitude]
	Serial Number [otp read], MCU S/N [mcu_sn], Version [otp read, version]
	*/
	GtkWidget* grid_aux = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_aux), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_aux), 1);
	gtk_grid_attach (GTK_GRID (grid_aux), gtk_label_new (TEXT_AUX_TITLE), 0, 0, 3, 1);

	//  Runtime Information
	gtk_grid_attach (GTK_GRID (grid_aux), gtk_label_new (TEXT_AUX_UPTIME), 0, 1, 1, 1);
	label_uptime = gtk_label_new ("<uptime>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_uptime, 1, 1, 1, 1);
	label_faults = gtk_label_new ("<faults>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_faults, 2, 1, 1, 1);

	//  Accelerometer
	gtk_grid_attach (GTK_GRID (grid_aux), gtk_label_new ("<accelerometer>"), 0, 2, 3, 1);

	//  Device Information
	label_version = gtk_label_new ("<version>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_version, 0, 3, 1, 1);
	label_sn_device = gtk_label_new ("<ser-number>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_sn_device, 1, 3, 1, 1);
	label_sn_mcu = gtk_label_new ("<MCU-S/N>");
	gtk_grid_attach (GTK_GRID (grid_aux), label_sn_mcu, 2, 3, 1, 1);

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
	//?  Consider a popup for this.
	GtkWidget* grid_config = gtk_grid_new ();
	gtk_grid_set_column_homogeneous (GTK_GRID (grid_config), 1);
	gtk_grid_set_row_homogeneous (GTK_GRID (grid_config), 1);
	uint8_t rp = 0;
	gtk_grid_attach (GTK_GRID (grid_config), gtk_label_new (TEXT_CONFIG_TITLE), 0, rp++, 3, 1);

	//  Idle Timer
	gtk_grid_attach (GTK_GRID (grid_config), gtk_label_new (TEXT_CONFIG_IDLE), 0, rp, 1, 1);
	label_idleEnable = gtk_label_new ("<idleEnable>");
	gtk_grid_attach (GTK_GRID (grid_config), label_idleEnable, 1, rp, 1, 1);
	sw_idleEnable = gtk_switch_new ();
	gtk_switch_set_state (GTK_SWITCH (sw_idleEnable), GUI_IDLE_ENABLE_DEFAULT);
	gtk_grid_attach (GTK_GRID (grid_config), sw_idleEnable, 2, rp, 1, 1);
	label_idleTimer = gtk_label_new ("<idleTimer>");
	gtk_grid_attach (GTK_GRID (grid_config), label_idleTimer, 3, rp, 1, 1);
	GtkEntryBuffer* buffer_idleTimer = gtk_entry_buffer_new (GUI_IDLE_TIMER_DEFAULT, -1);
	text_idleTimer = gtk_text_new_with_buffer (buffer_idleTimer);
	gtk_grid_attach (GTK_GRID (grid_config), text_idleTimer, 4, rp, 1, 1);
	label_idleTemp = gtk_label_new ("<idleTemp>");
	gtk_grid_attach (GTK_GRID (grid_config), label_idleTemp, 5, rp, 1, 1);
	GtkEntryBuffer* buffer_idleTemp = gtk_entry_buffer_new (GUI_IDLE_TEMP_DEFAULT, -1);
	text_idleTemp = gtk_text_new_with_buffer (buffer_idleTemp);
	gtk_grid_attach (GTK_GRID (grid_config), text_idleTemp, 6, rp, 1, 1);
	GtkWidget* btn_idle = gtk_button_new_with_label (TEXT_SET);
	gtk_grid_attach (GTK_GRID (grid_config), btn_idle, 7, rp++, 1, 1);
	g_signal_connect (btn_idle, "clicked", G_CALLBACK (cb_btn_idle_clicked), NULL);

	//  Sleep Timer
	gtk_grid_attach (GTK_GRID (grid_config), gtk_label_new (TEXT_CONFIG_SLEEP), 0, rp, 1, 1);
	label_sleepEnable = gtk_label_new ("<sleepEnable>");
	gtk_grid_attach (GTK_GRID (grid_config), label_sleepEnable, 1, rp, 1, 1);
	sw_sleepEnable = gtk_switch_new ();
	gtk_switch_set_state (GTK_SWITCH (sw_sleepEnable), GUI_SLEEP_ENABLE_DEFAULT);
	gtk_grid_attach (GTK_GRID (grid_config), sw_sleepEnable, 2, rp, 1, 1);
	label_sleepTimer = gtk_label_new ("<sleepTimer>");
	gtk_grid_attach (GTK_GRID (grid_config), label_sleepTimer, 3, rp, 1, 1);
	GtkEntryBuffer* buffer_sleepTimer = gtk_entry_buffer_new (GUI_SLEEP_ENABLE_TIMER, -1);
	text_sleepTimer = gtk_text_new_with_buffer (buffer_sleepTimer);
	gtk_grid_attach (GTK_GRID (grid_config), text_sleepTimer, 4, rp, 1, 1);
	GtkWidget* btn_sleep = gtk_button_new_with_label (TEXT_SET);
	gtk_grid_attach (GTK_GRID (grid_config), btn_sleep, 5, rp++, 1, 1);
	g_signal_connect (btn_sleep, "clicked", G_CALLBACK (cb_btn_sleep_clicked), NULL);

	//  Units
	GtkWidget* box_units = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous (GTK_BOX (box_units), 1);
	gtk_box_append (GTK_BOX (box_units), gtk_label_new (TEXT_COFIG_UNITS));
	label_units = gtk_label_new ("<celsius>");
	gtk_box_append (GTK_BOX (box_units), label_units);
	gtk_box_append (GTK_BOX (box_units), gtk_label_new (TEXT_UNIT_TEMP_F));
	sw_units = gtk_switch_new ();
	gtk_switch_set_state (GTK_SWITCH (sw_units), GUI_UNIT_CELSIUS_DEFAULT);
	gtk_box_append (GTK_BOX (box_units), sw_units);
	gtk_box_append (GTK_BOX (box_units), gtk_label_new (TEXT_UNIT_TEMP_C));
	gtk_grid_attach (GTK_GRID (grid_config), box_units, 0, rp++, 5, 1);

	//  Calibration Temperature
	label_calTemp = gtk_label_new ("<+0"TEXT_UNIT_TEMP_C">");
	gtk_grid_attach (GTK_GRID (grid_config), label_calTemp, 2, rp, 1, 1);
	GtkEntryBuffer* buffer_calTemp = gtk_entry_buffer_new (GUI_CAL_TEMP_DEFAULT, -1);
	text_calTemp = gtk_text_new_with_buffer (buffer_calTemp);
	gtk_grid_attach (GTK_GRID (grid_config), text_calTemp, 3, rp, 1, 1);
	GtkWidget* btn_calTemp = gtk_button_new_with_label (TEXT_SET);
	gtk_grid_attach (GTK_GRID (grid_config), btn_calTemp, 4, rp++, 1, 1);
	g_signal_connect (btn_calTemp, "clicked", G_CALLBACK (cb_btn_calTemp_clicked), NULL);

	//  Reset & Reboot
	GtkWidget* btn_reset = gtk_button_new_with_label (TEXT_CONFIG_RESET);
	gtk_grid_attach (GTK_GRID (grid_config), btn_reset, 1, rp, 1, 1);
	g_signal_connect (btn_reset, "clicked", G_CALLBACK (cb_btn_reset_clicked), NULL);
	GtkWidget* btn_reboot = gtk_button_new_with_label (TEXT_CONFIG_REBOOT);
	gtk_grid_attach (GTK_GRID (grid_config), btn_reboot, 2, rp++, 1, 1);
	g_signal_connect (btn_reboot, "clicked", G_CALLBACK (cb_btn_reboot_clicked), NULL);

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

static void cb_btn_reset_clicked (GtkButton* theButton, gpointer data)
{
	_LOG (0, "Here.\n");
}
static void cb_btn_reboot_clicked (GtkButton* theButton, gpointer data)
{
	_LOG (0, "Here.\n");
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
static void cb_btn_idle_clicked (GtkButton* theButton, gpointer data)
{
	_LOG (0, "Here.\n");
}
static void cb_btn_sleep_clicked (GtkButton* theButton, gpointer data)
{
	_LOG (0, "Here.\n");
}
static void cb_btn_calTemp_clicked (GtkButton* theButton, gpointer data)
{
	_LOG (0, "Here.\n");
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
