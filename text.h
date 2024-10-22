/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Description  -=-
 User-Facing Text Header
 This file should contain all user-facing text and all defines relevant to contstructing these strings.
 This file would typically be visible to all other files within the programme, so it should be easy to author this file in such a way that renaming a component cascades the rename to all relevant parts of user-facing text.

//  *--</Preface>--*  */

#ifndef IFIXIT_FIXHUB_IRON_PROGRAMMER_TEXT_H
#define IFIXIT_FIXHUB_IRON_PROGRAMMER_TEXT_H



//  *--<Text>--*  //

//  Fundamentals
#define TEXT_UNIT_TEMP_C "°C"
#define TEXT_UNIT_TEMP_C_LONG "Celsius"
#define TEXT_UNIT_TEMP_F "°F"
#define TEXT_UNIT_TEMP_F_LONG "Farenheit"
#define TEMP_UNIT_POWER "W"
#define TEXT_UNIT_PCT "%"
#define TEXT_UNIT_PCTF "%%"
#define TEXT_SET "Set"
#define TEXT_ENABLE "Enable"
#define TEXT_ENABLED TEXT_ENABLE"d"
#define TEXT_DISABLE "Disable"
#define TEXT_DISABLED TEXT_DISABLE"d"
//  Top-Level
#define TEXT_TL_TITLE "iFixit FixHub Iron Programmer"
#define TEXT_SER_OPEN "Open Port"
#define TEXT_SER_CLOSE "Close Port"
//  Core Readouts
#define TEXT_CORE_TITLE "Core Readouts"
#define TEXT_CORE_READOUT "Readout"
#define TEXT_CORE_STATE "Status"
#define TEXT_CORE_STATE_UNKNOWN "Unknown State!"
//  Setpoints
#define TEXT_SETPOINTS_TITLE "Setpoints"
#define TEXT_SETPOINTS_TEMP "Target Temp"
#define TEXT_SETPOINTS_TEMP_MAX "Max Temp"
//  Secondary Readouts
#define TEXT_AUX_TITLE "Secondary Readouts"
#define TEXT_AUX_UPTIME "Uptime"
#define TEXT_AUX_VERSION "Version"
//  Configuration
#define TEXT_CONFIG_TITLE "Configuration"
#define TEXT_CONFIG_IDLE "Idle"
#define TEXT_CONFIG_SLEEP "Sleep"
#define TEXT_COFIG_UNITS "Units"
#define TEXT_CONFIG_CAL_TEMP "Cal Temp"
#define TEXT_CONFIG_RESET "Reset"
#define TEXT_CONFIG_REBOOT "Reboot"

//  *--</Text>--*  //



#endif
