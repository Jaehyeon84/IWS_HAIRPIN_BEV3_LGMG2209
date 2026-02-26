//-----------------------------------------------------------------------------
//  File: RTC4expl.cpp                          Copyright (c) 2018 SCANLAB GmbH
//-----------------------------------------------------------------------------
//
//
//
//  Abstract
//      DLL handling functions for explicitly linking to the RTC4DLL.DLL
//      or RTC4DLLx64.DLL.
//
//  Authors
//      Gerald Schmid, Bernhard Schrems, Christian Lutz
//
//  Revision History
//      +-----+---------+--------+------------------------------------------+
//      | Mod |  Date   | Author | Description                              |
//      +-----+---------+--------+------------------------------------------+
//      | 0.0 | 15Jan01 | GS, BS | * initial release                        |
//      | 0.1 | 18Jun15 | CLU,BS | * handle nulled on close, LoadLibraryA   |
//      | 0.2 | 15Dec15 | CLU,BS | * 64bit compatibility                    |
//      | 0.3 | 29Sep16 | CLU    | * Company name updated                   |
//      |     |         |        |                                          |
//      +-----+---------+--------+------------------------------------------+
//
//      This file was automatically generated on Mrz 28, 2018.
//
//  Function List
//      RTC4open
//      RTC4close
//-----------------------------------------------------------------------------

#include <windows.h>
#include "RTC4expl.h"

volatile HINSTANCE gLibRTC4 = NULL;

GETMEMORY_FP getmemory;
N_GET_WAVEFORM_FP n_get_waveform;
GET_WAVEFORM_FP get_waveform;
N_MEASUREMENT_STATUS_FP n_measurement_status;
MEASUREMENT_STATUS_FP measurement_status;
N_LOAD_VARPOLYDELAY_FP n_load_varpolydelay;
LOAD_VARPOLYDELAY_FP load_varpolydelay;
N_LOAD_PROGRAM_FILE_FP n_load_program_file;
LOAD_PROGRAM_FILE_FP load_program_file;
N_LOAD_CORRECTION_FILE_FP n_load_correction_file;
LOAD_CORRECTION_FILE_FP load_correction_file;
N_LOAD_Z_TABLE_FP n_load_z_table;
LOAD_Z_TABLE_FP load_z_table;
N_LIST_NOP_FP n_list_nop;
LIST_NOP_FP list_nop;
N_SET_END_OF_LIST_FP n_set_end_of_list;
SET_END_OF_LIST_FP set_end_of_list;
N_JUMP_ABS_3D_FP n_jump_abs_3d;
JUMP_ABS_3D_FP jump_abs_3d;
N_JUMP_ABS_FP n_jump_abs;
JUMP_ABS_FP jump_abs;
N_MARK_ABS_3D_FP n_mark_abs_3d;
MARK_ABS_3D_FP mark_abs_3d;
N_MARK_ABS_FP n_mark_abs;
MARK_ABS_FP mark_abs;
N_JUMP_REL_3D_FP n_jump_rel_3d;
JUMP_REL_3D_FP jump_rel_3d;
N_JUMP_REL_FP n_jump_rel;
JUMP_REL_FP jump_rel;
N_MARK_REL_3D_FP n_mark_rel_3d;
MARK_REL_3D_FP mark_rel_3d;
N_MARK_REL_FP n_mark_rel;
MARK_REL_FP mark_rel;
N_WRITE_8BIT_PORT_LIST_FP n_write_8bit_port_list;
WRITE_8BIT_PORT_LIST_FP write_8bit_port_list;
N_WRITE_DA_1_LIST_FP n_write_da_1_list;
WRITE_DA_1_LIST_FP write_da_1_list;
N_WRITE_DA_2_LIST_FP n_write_da_2_list;
WRITE_DA_2_LIST_FP write_da_2_list;
N_SET_MATRIX_LIST_FP n_set_matrix_list;
SET_MATRIX_LIST_FP set_matrix_list;
N_SET_DEFOCUS_LIST_FP n_set_defocus_list;
SET_DEFOCUS_LIST_FP set_defocus_list;
N_SET_CONTROL_MODE_LIST_FP n_set_control_mode_list;
SET_CONTROL_MODE_LIST_FP set_control_mode_list;
N_SET_OFFSET_LIST_FP n_set_offset_list;
SET_OFFSET_LIST_FP set_offset_list;
N_LONG_DELAY_FP n_long_delay;
LONG_DELAY_FP long_delay;
N_LASER_ON_LIST_FP n_laser_on_list;
LASER_ON_LIST_FP laser_on_list;
N_SET_JUMP_SPEED_FP n_set_jump_speed;
SET_JUMP_SPEED_FP set_jump_speed;
N_SET_MARK_SPEED_FP n_set_mark_speed;
SET_MARK_SPEED_FP set_mark_speed;
N_SET_LASER_DELAYS_FP n_set_laser_delays;
SET_LASER_DELAYS_FP set_laser_delays;
N_SET_SCANNER_DELAYS_FP n_set_scanner_delays;
SET_SCANNER_DELAYS_FP set_scanner_delays;
N_SET_LIST_JUMP_FP n_set_list_jump;
SET_LIST_JUMP_FP set_list_jump;
N_SET_INPUT_POINTER_FP n_set_input_pointer;
SET_INPUT_POINTER_FP set_input_pointer;
N_LIST_CALL_FP n_list_call;
LIST_CALL_FP list_call;
N_LIST_RETURN_FP n_list_return;
LIST_RETURN_FP list_return;
N_Z_OUT_LIST_FP n_z_out_list;
Z_OUT_LIST_FP z_out_list;
N_SET_STANDBY_LIST_FP n_set_standby_list;
SET_STANDBY_LIST_FP set_standby_list;
N_TIMED_JUMP_ABS_FP n_timed_jump_abs;
TIMED_JUMP_ABS_FP timed_jump_abs;
N_TIMED_MARK_ABS_FP n_timed_mark_abs;
TIMED_MARK_ABS_FP timed_mark_abs;
N_TIMED_JUMP_REL_FP n_timed_jump_rel;
TIMED_JUMP_REL_FP timed_jump_rel;
N_TIMED_MARK_REL_FP n_timed_mark_rel;
TIMED_MARK_REL_FP timed_mark_rel;
N_SET_LASER_TIMING_FP n_set_laser_timing;
SET_LASER_TIMING_FP set_laser_timing;
N_SET_WOBBEL_XY_FP n_set_wobbel_xy;
SET_WOBBEL_XY_FP set_wobbel_xy;
N_SET_WOBBEL_FP n_set_wobbel;
SET_WOBBEL_FP set_wobbel;
N_SET_FLY_X_FP n_set_fly_x;
SET_FLY_X_FP set_fly_x;
N_SET_FLY_Y_FP n_set_fly_y;
SET_FLY_Y_FP set_fly_y;
N_SET_FLY_ROT_FP n_set_fly_rot;
SET_FLY_ROT_FP set_fly_rot;
N_FLY_RETURN_FP n_fly_return;
FLY_RETURN_FP fly_return;
N_CALCULATE_FLY_FP n_calculate_fly;
CALCULATE_FLY_FP calculate_fly;
N_WRITE_IO_PORT_LIST_FP n_write_io_port_list;
WRITE_IO_PORT_LIST_FP write_io_port_list;
N_SELECT_COR_TABLE_LIST_FP n_select_cor_table_list;
SELECT_COR_TABLE_LIST_FP select_cor_table_list;
N_SET_WAIT_FP n_set_wait;
SET_WAIT_FP set_wait;
N_SIMULATE_EXT_START_FP n_simulate_ext_start;
SIMULATE_EXT_START_FP simulate_ext_start;
N_WRITE_DA_X_LIST_FP n_write_da_x_list;
WRITE_DA_X_LIST_FP write_da_x_list;
N_SET_PIXEL_LINE_FP n_set_pixel_line;
SET_PIXEL_LINE_FP set_pixel_line;
N_SET_PIXEL_FP n_set_pixel;
SET_PIXEL_FP set_pixel;
N_SET_EXTSTARTPOS_LIST_FP n_set_extstartpos_list;
SET_EXTSTARTPOS_LIST_FP set_extstartpos_list;
N_LASER_SIGNAL_ON_LIST_FP n_laser_signal_on_list;
LASER_SIGNAL_ON_LIST_FP laser_signal_on_list;
N_LASER_SIGNAL_OFF_LIST_FP n_laser_signal_off_list;
LASER_SIGNAL_OFF_LIST_FP laser_signal_off_list;
N_SET_FIRSTPULSE_KILLER_LIST_FP n_set_firstpulse_killer_list;
SET_FIRSTPULSE_KILLER_LIST_FP set_firstpulse_killer_list;
N_SET_IO_COND_LIST_FP n_set_io_cond_list;
SET_IO_COND_LIST_FP set_io_cond_list;
N_CLEAR_IO_COND_LIST_FP n_clear_io_cond_list;
CLEAR_IO_COND_LIST_FP clear_io_cond_list;
N_LIST_JUMP_COND_FP n_list_jump_cond;
LIST_JUMP_COND_FP list_jump_cond;
N_LIST_CALL_COND_FP n_list_call_cond;
LIST_CALL_COND_FP list_call_cond;
N_SAVE_AND_RESTART_TIMER_FP n_save_and_restart_timer;
SAVE_AND_RESTART_TIMER_FP save_and_restart_timer;
N_SET_EXT_START_DELAY_LIST_FP n_set_ext_start_delay_list;
SET_EXT_START_DELAY_LIST_FP set_ext_start_delay_list;
N_SET_TRIGGER_FP n_set_trigger;
SET_TRIGGER_FP set_trigger;
N_ARC_REL_FP n_arc_rel;
ARC_REL_FP arc_rel;
N_ARC_ABS_FP n_arc_abs;
ARC_ABS_FP arc_abs;
DRILLING_FP drilling;
REGULATION_FP regulation;
FLYLINE_FP flyline;
N_GET_INPUT_POINTER_FP n_get_input_pointer;
GET_INPUT_POINTER_FP get_input_pointer;
SELECT_RTC_FP select_rtc;
RTC4_COUNT_CARDS_FP rtc4_count_cards;
N_GET_STATUS_FP n_get_status;
GET_STATUS_FP get_status;
N_READ_STATUS_FP n_read_status;
READ_STATUS_FP read_status;
N_GET_STARTSTOP_INFO_FP n_get_startstop_info;
GET_STARTSTOP_INFO_FP get_startstop_info;
N_GET_MARKING_INFO_FP n_get_marking_info;
GET_MARKING_INFO_FP get_marking_info;
GET_DLL_VERSION_FP get_dll_version;
N_SET_START_LIST_1_FP n_set_start_list_1;
SET_START_LIST_1_FP set_start_list_1;
N_SET_START_LIST_2_FP n_set_start_list_2;
SET_START_LIST_2_FP set_start_list_2;
N_SET_START_LIST_FP n_set_start_list;
SET_START_LIST_FP set_start_list;
N_EXECUTE_LIST_1_FP n_execute_list_1;
EXECUTE_LIST_1_FP execute_list_1;
N_EXECUTE_LIST_2_FP n_execute_list_2;
EXECUTE_LIST_2_FP execute_list_2;
N_EXECUTE_LIST_FP n_execute_list;
EXECUTE_LIST_FP execute_list;
N_WRITE_8BIT_PORT_FP n_write_8bit_port;
WRITE_8BIT_PORT_FP write_8bit_port;
N_WRITE_IO_PORT_FP n_write_io_port;
WRITE_IO_PORT_FP write_io_port;
N_AUTO_CHANGE_FP n_auto_change;
AUTO_CHANGE_FP auto_change;
N_AUTO_CHANGE_POS_FP n_auto_change_pos;
AUTO_CHANGE_POS_FP auto_change_pos;
AUT_CHANGE_FP aut_change;
N_START_LOOP_FP n_start_loop;
START_LOOP_FP start_loop;
N_QUIT_LOOP_FP n_quit_loop;
QUIT_LOOP_FP quit_loop;
N_SET_LIST_MODE_FP n_set_list_mode;
SET_LIST_MODE_FP set_list_mode;
N_STOP_EXECUTION_FP n_stop_execution;
STOP_EXECUTION_FP stop_execution;
N_READ_IO_PORT_FP n_read_io_port;
READ_IO_PORT_FP read_io_port;
N_WRITE_DA_1_FP n_write_da_1;
WRITE_DA_1_FP write_da_1;
N_WRITE_DA_2_FP n_write_da_2;
WRITE_DA_2_FP write_da_2;
N_SET_MAX_COUNTS_FP n_set_max_counts;
SET_MAX_COUNTS_FP set_max_counts;
N_GET_COUNTS_FP n_get_counts;
GET_COUNTS_FP get_counts;
N_SET_MATRIX_FP n_set_matrix;
SET_MATRIX_FP set_matrix;
N_SET_OFFSET_FP n_set_offset;
SET_OFFSET_FP set_offset;
N_GOTO_XYZ_FP n_goto_xyz;
GOTO_XYZ_FP goto_xyz;
N_GOTO_XY_FP n_goto_xy;
GOTO_XY_FP goto_xy;
N_GET_HEX_VERSION_FP n_get_hex_version;
GET_HEX_VERSION_FP get_hex_version;
N_DISABLE_LASER_FP n_disable_laser;
DISABLE_LASER_FP disable_laser;
N_ENABLE_LASER_FP n_enable_laser;
ENABLE_LASER_FP enable_laser;
N_STOP_LIST_FP n_stop_list;
STOP_LIST_FP stop_list;
N_RESTART_LIST_FP n_restart_list;
RESTART_LIST_FP restart_list;
N_GET_XYZ_POS_FP n_get_xyz_pos;
GET_XYZ_POS_FP get_xyz_pos;
N_GET_XY_POS_FP n_get_xy_pos;
GET_XY_POS_FP get_xy_pos;
N_SELECT_LIST_FP n_select_list;
SELECT_LIST_FP select_list;
N_Z_OUT_FP n_z_out;
Z_OUT_FP z_out;
N_SET_FIRSTPULSE_KILLER_FP n_set_firstpulse_killer;
SET_FIRSTPULSE_KILLER_FP set_firstpulse_killer;
N_SET_STANDBY_FP n_set_standby;
SET_STANDBY_FP set_standby;
N_LASER_SIGNAL_ON_FP n_laser_signal_on;
LASER_SIGNAL_ON_FP laser_signal_on;
N_LASER_SIGNAL_OFF_FP n_laser_signal_off;
LASER_SIGNAL_OFF_FP laser_signal_off;
N_SET_DELAY_MODE_FP n_set_delay_mode;
SET_DELAY_MODE_FP set_delay_mode;
N_SET_PISO_CONTROL_FP n_set_piso_control;
SET_PISO_CONTROL_FP set_piso_control;
N_SELECT_STATUS_FP n_select_status;
SELECT_STATUS_FP select_status;
N_GET_ENCODER_FP n_get_encoder;
GET_ENCODER_FP get_encoder;
N_SELECT_COR_TABLE_FP n_select_cor_table;
SELECT_COR_TABLE_FP select_cor_table;
N_EXECUTE_AT_POINTER_FP n_execute_at_pointer;
EXECUTE_AT_POINTER_FP execute_at_pointer;
N_GET_HEAD_STATUS_FP n_get_head_status;
GET_HEAD_STATUS_FP get_head_status;
N_SIMULATE_ENCODER_FP n_simulate_encoder;
SIMULATE_ENCODER_FP simulate_encoder;
N_SET_HI_FP n_set_hi;
SET_HI_FP set_hi;
N_RELEASE_WAIT_FP n_release_wait;
RELEASE_WAIT_FP release_wait;
N_GET_WAIT_STATUS_FP n_get_wait_status;
GET_WAIT_STATUS_FP get_wait_status;
N_SET_CONTROL_MODE_FP n_set_control_mode;
SET_CONTROL_MODE_FP set_control_mode;
N_SET_LASER_MODE_FP n_set_laser_mode;
SET_LASER_MODE_FP set_laser_mode;
N_SET_EXT_START_DELAY_FP n_set_ext_start_delay;
SET_EXT_START_DELAY_FP set_ext_start_delay;
N_HOME_POSITION_FP n_home_position;
HOME_POSITION_FP home_position;
N_SET_ROT_CENTER_FP n_set_rot_center;
SET_ROT_CENTER_FP set_rot_center;
N_DSP_START_FP n_dsp_start;
DSP_START_FP dsp_start;
N_WRITE_DA_X_FP n_write_da_x;
WRITE_DA_X_FP write_da_x;
N_READ_AD_X_FP n_read_ad_x;
READ_AD_X_FP read_ad_x;
N_READ_PIXEL_AD_FP n_read_pixel_ad;
READ_PIXEL_AD_FP read_pixel_ad;
N_GET_Z_DISTANCE_FP n_get_z_distance;
GET_Z_DISTANCE_FP get_z_distance;
N_GET_IO_STATUS_FP n_get_io_status;
GET_IO_STATUS_FP get_io_status;
N_GET_TIME_FP n_get_time;
GET_TIME_FP get_time;
N_SET_DEFOCUS_FP n_set_defocus;
SET_DEFOCUS_FP set_defocus;
N_SET_SOFTSTART_MODE_FP n_set_softstart_mode;
SET_SOFTSTART_MODE_FP set_softstart_mode;
N_SET_SOFTSTART_LEVEL_FP n_set_softstart_level;
SET_SOFTSTART_LEVEL_FP set_softstart_level;
N_CONTROL_COMMAND_FP n_control_command;
CONTROL_COMMAND_FP control_command;
LOAD_COR_FP load_cor;
LOAD_PRO_FP load_pro;
N_GET_SERIAL_NUMBER_FP n_get_serial_number;
GET_SERIAL_NUMBER_FP get_serial_number;
N_GET_SERIAL_NUMBER_32_FP n_get_serial_number_32;
GET_SERIAL_NUMBER_32_FP get_serial_number_32;
N_GET_RTC_VERSION_FP n_get_rtc_version;
GET_RTC_VERSION_FP get_rtc_version;
GET_HI_DATA_FP get_hi_data;
N_AUTO_CAL_FP n_auto_cal;
AUTO_CAL_FP auto_cal;
N_GET_LIST_SPACE_FP n_get_list_space;
GET_LIST_SPACE_FP get_list_space;
TEACHIN_FP teachin;
N_GET_VALUE_FP n_get_value;
GET_VALUE_FP get_value;
N_SET_IO_BIT_FP n_set_io_bit;
SET_IO_BIT_FP set_io_bit;
N_CLEAR_IO_BIT_FP n_clear_io_bit;
CLEAR_IO_BIT_FP clear_io_bit;
SET_DUTY_CYCLE_TABLE_FP set_duty_cycle_table;
N_MOVE_TO_FP n_move_to;
MOVE_TO_FP move_to;


//  RTC4open
//
//  This function explicitly or dynamically links to the RTC4DLL.DLL
//  or RTC4DLLx64.DLL for Win32 or for Win64 compiling respectively.
//  Call it before using any RTC4 function.
//
//      Return      Meaning
//
//       0          Success. Using of the RTC4 functions is possible.
//      -1          Error: file RTC4DLL.DLL/RTC4DLLx64.DLL not found.
//                  The RTC4 functions must not be used.
//      -2          Error: file RTC4DLL.DLL or the RTC4DLLx64.DLL is 
//                  already loaded.
int RTC4open(void) {
    if(gLibRTC4)
        return(-2);

#if defined(_WIN64)
    gLibRTC4 = LoadLibraryA("RTC4DLLx64.DLL");
#else
    gLibRTC4 = LoadLibraryA("RTC4DLL.DLL");
#endif
    if(!gLibRTC4)
        return(-1);

    // Get the addresses of the DLL-Functions
    getmemory = (GETMEMORY_FP) GetProcAddress(gLibRTC4,"getmemory");
    n_get_waveform = (N_GET_WAVEFORM_FP) GetProcAddress(gLibRTC4,"n_get_waveform");
    get_waveform = (GET_WAVEFORM_FP) GetProcAddress(gLibRTC4,"get_waveform");
    n_measurement_status = (N_MEASUREMENT_STATUS_FP) GetProcAddress(gLibRTC4,"n_measurement_status");
    measurement_status = (MEASUREMENT_STATUS_FP) GetProcAddress(gLibRTC4,"measurement_status");
    n_load_varpolydelay = (N_LOAD_VARPOLYDELAY_FP) GetProcAddress(gLibRTC4,"n_load_varpolydelay");
    load_varpolydelay = (LOAD_VARPOLYDELAY_FP) GetProcAddress(gLibRTC4,"load_varpolydelay");
    n_load_program_file = (N_LOAD_PROGRAM_FILE_FP) GetProcAddress(gLibRTC4,"n_load_program_file");
    load_program_file = (LOAD_PROGRAM_FILE_FP) GetProcAddress(gLibRTC4,"load_program_file");
    n_load_correction_file = (N_LOAD_CORRECTION_FILE_FP) GetProcAddress(gLibRTC4,"n_load_correction_file");
    load_correction_file = (LOAD_CORRECTION_FILE_FP) GetProcAddress(gLibRTC4,"load_correction_file");
    n_load_z_table = (N_LOAD_Z_TABLE_FP) GetProcAddress(gLibRTC4,"n_load_z_table");
    load_z_table = (LOAD_Z_TABLE_FP) GetProcAddress(gLibRTC4,"load_z_table");
    n_list_nop = (N_LIST_NOP_FP) GetProcAddress(gLibRTC4,"n_list_nop");
    list_nop = (LIST_NOP_FP) GetProcAddress(gLibRTC4,"list_nop");
    n_set_end_of_list = (N_SET_END_OF_LIST_FP) GetProcAddress(gLibRTC4,"n_set_end_of_list");
    set_end_of_list = (SET_END_OF_LIST_FP) GetProcAddress(gLibRTC4,"set_end_of_list");
    n_jump_abs_3d = (N_JUMP_ABS_3D_FP) GetProcAddress(gLibRTC4,"n_jump_abs_3d");
    jump_abs_3d = (JUMP_ABS_3D_FP) GetProcAddress(gLibRTC4,"jump_abs_3d");
    n_jump_abs = (N_JUMP_ABS_FP) GetProcAddress(gLibRTC4,"n_jump_abs");
    jump_abs = (JUMP_ABS_FP) GetProcAddress(gLibRTC4,"jump_abs");
    n_mark_abs_3d = (N_MARK_ABS_3D_FP) GetProcAddress(gLibRTC4,"n_mark_abs_3d");
    mark_abs_3d = (MARK_ABS_3D_FP) GetProcAddress(gLibRTC4,"mark_abs_3d");
    n_mark_abs = (N_MARK_ABS_FP) GetProcAddress(gLibRTC4,"n_mark_abs");
    mark_abs = (MARK_ABS_FP) GetProcAddress(gLibRTC4,"mark_abs");
    n_jump_rel_3d = (N_JUMP_REL_3D_FP) GetProcAddress(gLibRTC4,"n_jump_rel_3d");
    jump_rel_3d = (JUMP_REL_3D_FP) GetProcAddress(gLibRTC4,"jump_rel_3d");
    n_jump_rel = (N_JUMP_REL_FP) GetProcAddress(gLibRTC4,"n_jump_rel");
    jump_rel = (JUMP_REL_FP) GetProcAddress(gLibRTC4,"jump_rel");
    n_mark_rel_3d = (N_MARK_REL_3D_FP) GetProcAddress(gLibRTC4,"n_mark_rel_3d");
    mark_rel_3d = (MARK_REL_3D_FP) GetProcAddress(gLibRTC4,"mark_rel_3d");
    n_mark_rel = (N_MARK_REL_FP) GetProcAddress(gLibRTC4,"n_mark_rel");
    mark_rel = (MARK_REL_FP) GetProcAddress(gLibRTC4,"mark_rel");
    n_write_8bit_port_list = (N_WRITE_8BIT_PORT_LIST_FP) GetProcAddress(gLibRTC4,"n_write_8bit_port_list");
    write_8bit_port_list = (WRITE_8BIT_PORT_LIST_FP) GetProcAddress(gLibRTC4,"write_8bit_port_list");
    n_write_da_1_list = (N_WRITE_DA_1_LIST_FP) GetProcAddress(gLibRTC4,"n_write_da_1_list");
    write_da_1_list = (WRITE_DA_1_LIST_FP) GetProcAddress(gLibRTC4,"write_da_1_list");
    n_write_da_2_list = (N_WRITE_DA_2_LIST_FP) GetProcAddress(gLibRTC4,"n_write_da_2_list");
    write_da_2_list = (WRITE_DA_2_LIST_FP) GetProcAddress(gLibRTC4,"write_da_2_list");
    n_set_matrix_list = (N_SET_MATRIX_LIST_FP) GetProcAddress(gLibRTC4,"n_set_matrix_list");
    set_matrix_list = (SET_MATRIX_LIST_FP) GetProcAddress(gLibRTC4,"set_matrix_list");
    n_set_defocus_list = (N_SET_DEFOCUS_LIST_FP) GetProcAddress(gLibRTC4,"n_set_defocus_list");
    set_defocus_list = (SET_DEFOCUS_LIST_FP) GetProcAddress(gLibRTC4,"set_defocus_list");
    n_set_control_mode_list = (N_SET_CONTROL_MODE_LIST_FP) GetProcAddress(gLibRTC4,"n_set_control_mode_list");
    set_control_mode_list = (SET_CONTROL_MODE_LIST_FP) GetProcAddress(gLibRTC4,"set_control_mode_list");
    n_set_offset_list = (N_SET_OFFSET_LIST_FP) GetProcAddress(gLibRTC4,"n_set_offset_list");
    set_offset_list = (SET_OFFSET_LIST_FP) GetProcAddress(gLibRTC4,"set_offset_list");
    n_long_delay = (N_LONG_DELAY_FP) GetProcAddress(gLibRTC4,"n_long_delay");
    long_delay = (LONG_DELAY_FP) GetProcAddress(gLibRTC4,"long_delay");
    n_laser_on_list = (N_LASER_ON_LIST_FP) GetProcAddress(gLibRTC4,"n_laser_on_list");
    laser_on_list = (LASER_ON_LIST_FP) GetProcAddress(gLibRTC4,"laser_on_list");
    n_set_jump_speed = (N_SET_JUMP_SPEED_FP) GetProcAddress(gLibRTC4,"n_set_jump_speed");
    set_jump_speed = (SET_JUMP_SPEED_FP) GetProcAddress(gLibRTC4,"set_jump_speed");
    n_set_mark_speed = (N_SET_MARK_SPEED_FP) GetProcAddress(gLibRTC4,"n_set_mark_speed");
    set_mark_speed = (SET_MARK_SPEED_FP) GetProcAddress(gLibRTC4,"set_mark_speed");
    n_set_laser_delays = (N_SET_LASER_DELAYS_FP) GetProcAddress(gLibRTC4,"n_set_laser_delays");
    set_laser_delays = (SET_LASER_DELAYS_FP) GetProcAddress(gLibRTC4,"set_laser_delays");
    n_set_scanner_delays = (N_SET_SCANNER_DELAYS_FP) GetProcAddress(gLibRTC4,"n_set_scanner_delays");
    set_scanner_delays = (SET_SCANNER_DELAYS_FP) GetProcAddress(gLibRTC4,"set_scanner_delays");
    n_set_list_jump = (N_SET_LIST_JUMP_FP) GetProcAddress(gLibRTC4,"n_set_list_jump");
    set_list_jump = (SET_LIST_JUMP_FP) GetProcAddress(gLibRTC4,"set_list_jump");
    n_set_input_pointer = (N_SET_INPUT_POINTER_FP) GetProcAddress(gLibRTC4,"n_set_input_pointer");
    set_input_pointer = (SET_INPUT_POINTER_FP) GetProcAddress(gLibRTC4,"set_input_pointer");
    n_list_call = (N_LIST_CALL_FP) GetProcAddress(gLibRTC4,"n_list_call");
    list_call = (LIST_CALL_FP) GetProcAddress(gLibRTC4,"list_call");
    n_list_return = (N_LIST_RETURN_FP) GetProcAddress(gLibRTC4,"n_list_return");
    list_return = (LIST_RETURN_FP) GetProcAddress(gLibRTC4,"list_return");
    n_z_out_list = (N_Z_OUT_LIST_FP) GetProcAddress(gLibRTC4,"n_z_out_list");
    z_out_list = (Z_OUT_LIST_FP) GetProcAddress(gLibRTC4,"z_out_list");
    n_set_standby_list = (N_SET_STANDBY_LIST_FP) GetProcAddress(gLibRTC4,"n_set_standby_list");
    set_standby_list = (SET_STANDBY_LIST_FP) GetProcAddress(gLibRTC4,"set_standby_list");
    n_timed_jump_abs = (N_TIMED_JUMP_ABS_FP) GetProcAddress(gLibRTC4,"n_timed_jump_abs");
    timed_jump_abs = (TIMED_JUMP_ABS_FP) GetProcAddress(gLibRTC4,"timed_jump_abs");
    n_timed_mark_abs = (N_TIMED_MARK_ABS_FP) GetProcAddress(gLibRTC4,"n_timed_mark_abs");
    timed_mark_abs = (TIMED_MARK_ABS_FP) GetProcAddress(gLibRTC4,"timed_mark_abs");
    n_timed_jump_rel = (N_TIMED_JUMP_REL_FP) GetProcAddress(gLibRTC4,"n_timed_jump_rel");
    timed_jump_rel = (TIMED_JUMP_REL_FP) GetProcAddress(gLibRTC4,"timed_jump_rel");
    n_timed_mark_rel = (N_TIMED_MARK_REL_FP) GetProcAddress(gLibRTC4,"n_timed_mark_rel");
    timed_mark_rel = (TIMED_MARK_REL_FP) GetProcAddress(gLibRTC4,"timed_mark_rel");
    n_set_laser_timing = (N_SET_LASER_TIMING_FP) GetProcAddress(gLibRTC4,"n_set_laser_timing");
    set_laser_timing = (SET_LASER_TIMING_FP) GetProcAddress(gLibRTC4,"set_laser_timing");
    n_set_wobbel_xy = (N_SET_WOBBEL_XY_FP) GetProcAddress(gLibRTC4,"n_set_wobbel_xy");
    set_wobbel_xy = (SET_WOBBEL_XY_FP) GetProcAddress(gLibRTC4,"set_wobbel_xy");
    n_set_wobbel = (N_SET_WOBBEL_FP) GetProcAddress(gLibRTC4,"n_set_wobbel");
    set_wobbel = (SET_WOBBEL_FP) GetProcAddress(gLibRTC4,"set_wobbel");
    n_set_fly_x = (N_SET_FLY_X_FP) GetProcAddress(gLibRTC4,"n_set_fly_x");
    set_fly_x = (SET_FLY_X_FP) GetProcAddress(gLibRTC4,"set_fly_x");
    n_set_fly_y = (N_SET_FLY_Y_FP) GetProcAddress(gLibRTC4,"n_set_fly_y");
    set_fly_y = (SET_FLY_Y_FP) GetProcAddress(gLibRTC4,"set_fly_y");
    n_set_fly_rot = (N_SET_FLY_ROT_FP) GetProcAddress(gLibRTC4,"n_set_fly_rot");
    set_fly_rot = (SET_FLY_ROT_FP) GetProcAddress(gLibRTC4,"set_fly_rot");
    n_fly_return = (N_FLY_RETURN_FP) GetProcAddress(gLibRTC4,"n_fly_return");
    fly_return = (FLY_RETURN_FP) GetProcAddress(gLibRTC4,"fly_return");
    n_calculate_fly = (N_CALCULATE_FLY_FP) GetProcAddress(gLibRTC4,"n_calculate_fly");
    calculate_fly = (CALCULATE_FLY_FP) GetProcAddress(gLibRTC4,"calculate_fly");
    n_write_io_port_list = (N_WRITE_IO_PORT_LIST_FP) GetProcAddress(gLibRTC4,"n_write_io_port_list");
    write_io_port_list = (WRITE_IO_PORT_LIST_FP) GetProcAddress(gLibRTC4,"write_io_port_list");
    n_select_cor_table_list = (N_SELECT_COR_TABLE_LIST_FP) GetProcAddress(gLibRTC4,"n_select_cor_table_list");
    select_cor_table_list = (SELECT_COR_TABLE_LIST_FP) GetProcAddress(gLibRTC4,"select_cor_table_list");
    n_set_wait = (N_SET_WAIT_FP) GetProcAddress(gLibRTC4,"n_set_wait");
    set_wait = (SET_WAIT_FP) GetProcAddress(gLibRTC4,"set_wait");
    n_simulate_ext_start = (N_SIMULATE_EXT_START_FP) GetProcAddress(gLibRTC4,"n_simulate_ext_start");
    simulate_ext_start = (SIMULATE_EXT_START_FP) GetProcAddress(gLibRTC4,"simulate_ext_start");
    n_write_da_x_list = (N_WRITE_DA_X_LIST_FP) GetProcAddress(gLibRTC4,"n_write_da_x_list");
    write_da_x_list = (WRITE_DA_X_LIST_FP) GetProcAddress(gLibRTC4,"write_da_x_list");
    n_set_pixel_line = (N_SET_PIXEL_LINE_FP) GetProcAddress(gLibRTC4,"n_set_pixel_line");
    set_pixel_line = (SET_PIXEL_LINE_FP) GetProcAddress(gLibRTC4,"set_pixel_line");
    n_set_pixel = (N_SET_PIXEL_FP) GetProcAddress(gLibRTC4,"n_set_pixel");
    set_pixel = (SET_PIXEL_FP) GetProcAddress(gLibRTC4,"set_pixel");
    n_set_extstartpos_list = (N_SET_EXTSTARTPOS_LIST_FP) GetProcAddress(gLibRTC4,"n_set_extstartpos_list");
    set_extstartpos_list = (SET_EXTSTARTPOS_LIST_FP) GetProcAddress(gLibRTC4,"set_extstartpos_list");
    n_laser_signal_on_list = (N_LASER_SIGNAL_ON_LIST_FP) GetProcAddress(gLibRTC4,"n_laser_signal_on_list");
    laser_signal_on_list = (LASER_SIGNAL_ON_LIST_FP) GetProcAddress(gLibRTC4,"laser_signal_on_list");
    n_laser_signal_off_list = (N_LASER_SIGNAL_OFF_LIST_FP) GetProcAddress(gLibRTC4,"n_laser_signal_off_list");
    laser_signal_off_list = (LASER_SIGNAL_OFF_LIST_FP) GetProcAddress(gLibRTC4,"laser_signal_off_list");
    n_set_firstpulse_killer_list = (N_SET_FIRSTPULSE_KILLER_LIST_FP) GetProcAddress(gLibRTC4,"n_set_firstpulse_killer_list");
    set_firstpulse_killer_list = (SET_FIRSTPULSE_KILLER_LIST_FP) GetProcAddress(gLibRTC4,"set_firstpulse_killer_list");
    n_set_io_cond_list = (N_SET_IO_COND_LIST_FP) GetProcAddress(gLibRTC4,"n_set_io_cond_list");
    set_io_cond_list = (SET_IO_COND_LIST_FP) GetProcAddress(gLibRTC4,"set_io_cond_list");
    n_clear_io_cond_list = (N_CLEAR_IO_COND_LIST_FP) GetProcAddress(gLibRTC4,"n_clear_io_cond_list");
    clear_io_cond_list = (CLEAR_IO_COND_LIST_FP) GetProcAddress(gLibRTC4,"clear_io_cond_list");
    n_list_jump_cond = (N_LIST_JUMP_COND_FP) GetProcAddress(gLibRTC4,"n_list_jump_cond");
    list_jump_cond = (LIST_JUMP_COND_FP) GetProcAddress(gLibRTC4,"list_jump_cond");
    n_list_call_cond = (N_LIST_CALL_COND_FP) GetProcAddress(gLibRTC4,"n_list_call_cond");
    list_call_cond = (LIST_CALL_COND_FP) GetProcAddress(gLibRTC4,"list_call_cond");
    n_save_and_restart_timer = (N_SAVE_AND_RESTART_TIMER_FP) GetProcAddress(gLibRTC4,"n_save_and_restart_timer");
    save_and_restart_timer = (SAVE_AND_RESTART_TIMER_FP) GetProcAddress(gLibRTC4,"save_and_restart_timer");
    n_set_ext_start_delay_list = (N_SET_EXT_START_DELAY_LIST_FP) GetProcAddress(gLibRTC4,"n_set_ext_start_delay_list");
    set_ext_start_delay_list = (SET_EXT_START_DELAY_LIST_FP) GetProcAddress(gLibRTC4,"set_ext_start_delay_list");
    n_set_trigger = (N_SET_TRIGGER_FP) GetProcAddress(gLibRTC4,"n_set_trigger");
    set_trigger = (SET_TRIGGER_FP) GetProcAddress(gLibRTC4,"set_trigger");
    n_arc_rel = (N_ARC_REL_FP) GetProcAddress(gLibRTC4,"n_arc_rel");
    arc_rel = (ARC_REL_FP) GetProcAddress(gLibRTC4,"arc_rel");
    n_arc_abs = (N_ARC_ABS_FP) GetProcAddress(gLibRTC4,"n_arc_abs");
    arc_abs = (ARC_ABS_FP) GetProcAddress(gLibRTC4,"arc_abs");
    drilling = (DRILLING_FP) GetProcAddress(gLibRTC4,"drilling");
    regulation = (REGULATION_FP) GetProcAddress(gLibRTC4,"regulation");
    flyline = (FLYLINE_FP) GetProcAddress(gLibRTC4,"flyline");
    n_get_input_pointer = (N_GET_INPUT_POINTER_FP) GetProcAddress(gLibRTC4,"n_get_input_pointer");
    get_input_pointer = (GET_INPUT_POINTER_FP) GetProcAddress(gLibRTC4,"get_input_pointer");
    select_rtc = (SELECT_RTC_FP) GetProcAddress(gLibRTC4,"select_rtc");
    rtc4_count_cards = (RTC4_COUNT_CARDS_FP) GetProcAddress(gLibRTC4,"rtc4_count_cards");
    n_get_status = (N_GET_STATUS_FP) GetProcAddress(gLibRTC4,"n_get_status");
    get_status = (GET_STATUS_FP) GetProcAddress(gLibRTC4,"get_status");
    n_read_status = (N_READ_STATUS_FP) GetProcAddress(gLibRTC4,"n_read_status");
    read_status = (READ_STATUS_FP) GetProcAddress(gLibRTC4,"read_status");
    n_get_startstop_info = (N_GET_STARTSTOP_INFO_FP) GetProcAddress(gLibRTC4,"n_get_startstop_info");
    get_startstop_info = (GET_STARTSTOP_INFO_FP) GetProcAddress(gLibRTC4,"get_startstop_info");
    n_get_marking_info = (N_GET_MARKING_INFO_FP) GetProcAddress(gLibRTC4,"n_get_marking_info");
    get_marking_info = (GET_MARKING_INFO_FP) GetProcAddress(gLibRTC4,"get_marking_info");
    get_dll_version = (GET_DLL_VERSION_FP) GetProcAddress(gLibRTC4,"get_dll_version");
    n_set_start_list_1 = (N_SET_START_LIST_1_FP) GetProcAddress(gLibRTC4,"n_set_start_list_1");
    set_start_list_1 = (SET_START_LIST_1_FP) GetProcAddress(gLibRTC4,"set_start_list_1");
    n_set_start_list_2 = (N_SET_START_LIST_2_FP) GetProcAddress(gLibRTC4,"n_set_start_list_2");
    set_start_list_2 = (SET_START_LIST_2_FP) GetProcAddress(gLibRTC4,"set_start_list_2");
    n_set_start_list = (N_SET_START_LIST_FP) GetProcAddress(gLibRTC4,"n_set_start_list");
    set_start_list = (SET_START_LIST_FP) GetProcAddress(gLibRTC4,"set_start_list");
    n_execute_list_1 = (N_EXECUTE_LIST_1_FP) GetProcAddress(gLibRTC4,"n_execute_list_1");
    execute_list_1 = (EXECUTE_LIST_1_FP) GetProcAddress(gLibRTC4,"execute_list_1");
    n_execute_list_2 = (N_EXECUTE_LIST_2_FP) GetProcAddress(gLibRTC4,"n_execute_list_2");
    execute_list_2 = (EXECUTE_LIST_2_FP) GetProcAddress(gLibRTC4,"execute_list_2");
    n_execute_list = (N_EXECUTE_LIST_FP) GetProcAddress(gLibRTC4,"n_execute_list");
    execute_list = (EXECUTE_LIST_FP) GetProcAddress(gLibRTC4,"execute_list");
    n_write_8bit_port = (N_WRITE_8BIT_PORT_FP) GetProcAddress(gLibRTC4,"n_write_8bit_port");
    write_8bit_port = (WRITE_8BIT_PORT_FP) GetProcAddress(gLibRTC4,"write_8bit_port");
    n_write_io_port = (N_WRITE_IO_PORT_FP) GetProcAddress(gLibRTC4,"n_write_io_port");
    write_io_port = (WRITE_IO_PORT_FP) GetProcAddress(gLibRTC4,"write_io_port");
    n_auto_change = (N_AUTO_CHANGE_FP) GetProcAddress(gLibRTC4,"n_auto_change");
    auto_change = (AUTO_CHANGE_FP) GetProcAddress(gLibRTC4,"auto_change");
    n_auto_change_pos = (N_AUTO_CHANGE_POS_FP) GetProcAddress(gLibRTC4,"n_auto_change_pos");
    auto_change_pos = (AUTO_CHANGE_POS_FP) GetProcAddress(gLibRTC4,"auto_change_pos");
    aut_change = (AUT_CHANGE_FP) GetProcAddress(gLibRTC4,"aut_change");
    n_start_loop = (N_START_LOOP_FP) GetProcAddress(gLibRTC4,"n_start_loop");
    start_loop = (START_LOOP_FP) GetProcAddress(gLibRTC4,"start_loop");
    n_quit_loop = (N_QUIT_LOOP_FP) GetProcAddress(gLibRTC4,"n_quit_loop");
    quit_loop = (QUIT_LOOP_FP) GetProcAddress(gLibRTC4,"quit_loop");
    n_set_list_mode = (N_SET_LIST_MODE_FP) GetProcAddress(gLibRTC4,"n_set_list_mode");
    set_list_mode = (SET_LIST_MODE_FP) GetProcAddress(gLibRTC4,"set_list_mode");
    n_stop_execution = (N_STOP_EXECUTION_FP) GetProcAddress(gLibRTC4,"n_stop_execution");
    stop_execution = (STOP_EXECUTION_FP) GetProcAddress(gLibRTC4,"stop_execution");
    n_read_io_port = (N_READ_IO_PORT_FP) GetProcAddress(gLibRTC4,"n_read_io_port");
    read_io_port = (READ_IO_PORT_FP) GetProcAddress(gLibRTC4,"read_io_port");
    n_write_da_1 = (N_WRITE_DA_1_FP) GetProcAddress(gLibRTC4,"n_write_da_1");
    write_da_1 = (WRITE_DA_1_FP) GetProcAddress(gLibRTC4,"write_da_1");
    n_write_da_2 = (N_WRITE_DA_2_FP) GetProcAddress(gLibRTC4,"n_write_da_2");
    write_da_2 = (WRITE_DA_2_FP) GetProcAddress(gLibRTC4,"write_da_2");
    n_set_max_counts = (N_SET_MAX_COUNTS_FP) GetProcAddress(gLibRTC4,"n_set_max_counts");
    set_max_counts = (SET_MAX_COUNTS_FP) GetProcAddress(gLibRTC4,"set_max_counts");
    n_get_counts = (N_GET_COUNTS_FP) GetProcAddress(gLibRTC4,"n_get_counts");
    get_counts = (GET_COUNTS_FP) GetProcAddress(gLibRTC4,"get_counts");
    n_set_matrix = (N_SET_MATRIX_FP) GetProcAddress(gLibRTC4,"n_set_matrix");
    set_matrix = (SET_MATRIX_FP) GetProcAddress(gLibRTC4,"set_matrix");
    n_set_offset = (N_SET_OFFSET_FP) GetProcAddress(gLibRTC4,"n_set_offset");
    set_offset = (SET_OFFSET_FP) GetProcAddress(gLibRTC4,"set_offset");
    n_goto_xyz = (N_GOTO_XYZ_FP) GetProcAddress(gLibRTC4,"n_goto_xyz");
    goto_xyz = (GOTO_XYZ_FP) GetProcAddress(gLibRTC4,"goto_xyz");
    n_goto_xy = (N_GOTO_XY_FP) GetProcAddress(gLibRTC4,"n_goto_xy");
    goto_xy = (GOTO_XY_FP) GetProcAddress(gLibRTC4,"goto_xy");
    n_get_hex_version = (N_GET_HEX_VERSION_FP) GetProcAddress(gLibRTC4,"n_get_hex_version");
    get_hex_version = (GET_HEX_VERSION_FP) GetProcAddress(gLibRTC4,"get_hex_version");
    n_disable_laser = (N_DISABLE_LASER_FP) GetProcAddress(gLibRTC4,"n_disable_laser");
    disable_laser = (DISABLE_LASER_FP) GetProcAddress(gLibRTC4,"disable_laser");
    n_enable_laser = (N_ENABLE_LASER_FP) GetProcAddress(gLibRTC4,"n_enable_laser");
    enable_laser = (ENABLE_LASER_FP) GetProcAddress(gLibRTC4,"enable_laser");
    n_stop_list = (N_STOP_LIST_FP) GetProcAddress(gLibRTC4,"n_stop_list");
    stop_list = (STOP_LIST_FP) GetProcAddress(gLibRTC4,"stop_list");
    n_restart_list = (N_RESTART_LIST_FP) GetProcAddress(gLibRTC4,"n_restart_list");
    restart_list = (RESTART_LIST_FP) GetProcAddress(gLibRTC4,"restart_list");
    n_get_xyz_pos = (N_GET_XYZ_POS_FP) GetProcAddress(gLibRTC4,"n_get_xyz_pos");
    get_xyz_pos = (GET_XYZ_POS_FP) GetProcAddress(gLibRTC4,"get_xyz_pos");
    n_get_xy_pos = (N_GET_XY_POS_FP) GetProcAddress(gLibRTC4,"n_get_xy_pos");
    get_xy_pos = (GET_XY_POS_FP) GetProcAddress(gLibRTC4,"get_xy_pos");
    n_select_list = (N_SELECT_LIST_FP) GetProcAddress(gLibRTC4,"n_select_list");
    select_list = (SELECT_LIST_FP) GetProcAddress(gLibRTC4,"select_list");
    n_z_out = (N_Z_OUT_FP) GetProcAddress(gLibRTC4,"n_z_out");
    z_out = (Z_OUT_FP) GetProcAddress(gLibRTC4,"z_out");
    n_set_firstpulse_killer = (N_SET_FIRSTPULSE_KILLER_FP) GetProcAddress(gLibRTC4,"n_set_firstpulse_killer");
    set_firstpulse_killer = (SET_FIRSTPULSE_KILLER_FP) GetProcAddress(gLibRTC4,"set_firstpulse_killer");
    n_set_standby = (N_SET_STANDBY_FP) GetProcAddress(gLibRTC4,"n_set_standby");
    set_standby = (SET_STANDBY_FP) GetProcAddress(gLibRTC4,"set_standby");
    n_laser_signal_on = (N_LASER_SIGNAL_ON_FP) GetProcAddress(gLibRTC4,"n_laser_signal_on");
    laser_signal_on = (LASER_SIGNAL_ON_FP) GetProcAddress(gLibRTC4,"laser_signal_on");
    n_laser_signal_off = (N_LASER_SIGNAL_OFF_FP) GetProcAddress(gLibRTC4,"n_laser_signal_off");
    laser_signal_off = (LASER_SIGNAL_OFF_FP) GetProcAddress(gLibRTC4,"laser_signal_off");
    n_set_delay_mode = (N_SET_DELAY_MODE_FP) GetProcAddress(gLibRTC4,"n_set_delay_mode");
    set_delay_mode = (SET_DELAY_MODE_FP) GetProcAddress(gLibRTC4,"set_delay_mode");
    n_set_piso_control = (N_SET_PISO_CONTROL_FP) GetProcAddress(gLibRTC4,"n_set_piso_control");
    set_piso_control = (SET_PISO_CONTROL_FP) GetProcAddress(gLibRTC4,"set_piso_control");
    n_select_status = (N_SELECT_STATUS_FP) GetProcAddress(gLibRTC4,"n_select_status");
    select_status = (SELECT_STATUS_FP) GetProcAddress(gLibRTC4,"select_status");
    n_get_encoder = (N_GET_ENCODER_FP) GetProcAddress(gLibRTC4,"n_get_encoder");
    get_encoder = (GET_ENCODER_FP) GetProcAddress(gLibRTC4,"get_encoder");
    n_select_cor_table = (N_SELECT_COR_TABLE_FP) GetProcAddress(gLibRTC4,"n_select_cor_table");
    select_cor_table = (SELECT_COR_TABLE_FP) GetProcAddress(gLibRTC4,"select_cor_table");
    n_execute_at_pointer = (N_EXECUTE_AT_POINTER_FP) GetProcAddress(gLibRTC4,"n_execute_at_pointer");
    execute_at_pointer = (EXECUTE_AT_POINTER_FP) GetProcAddress(gLibRTC4,"execute_at_pointer");
    n_get_head_status = (N_GET_HEAD_STATUS_FP) GetProcAddress(gLibRTC4,"n_get_head_status");
    get_head_status = (GET_HEAD_STATUS_FP) GetProcAddress(gLibRTC4,"get_head_status");
    n_simulate_encoder = (N_SIMULATE_ENCODER_FP) GetProcAddress(gLibRTC4,"n_simulate_encoder");
    simulate_encoder = (SIMULATE_ENCODER_FP) GetProcAddress(gLibRTC4,"simulate_encoder");
    n_set_hi = (N_SET_HI_FP) GetProcAddress(gLibRTC4,"n_set_hi");
    set_hi = (SET_HI_FP) GetProcAddress(gLibRTC4,"set_hi");
    n_release_wait = (N_RELEASE_WAIT_FP) GetProcAddress(gLibRTC4,"n_release_wait");
    release_wait = (RELEASE_WAIT_FP) GetProcAddress(gLibRTC4,"release_wait");
    n_get_wait_status = (N_GET_WAIT_STATUS_FP) GetProcAddress(gLibRTC4,"n_get_wait_status");
    get_wait_status = (GET_WAIT_STATUS_FP) GetProcAddress(gLibRTC4,"get_wait_status");
    n_set_control_mode = (N_SET_CONTROL_MODE_FP) GetProcAddress(gLibRTC4,"n_set_control_mode");
    set_control_mode = (SET_CONTROL_MODE_FP) GetProcAddress(gLibRTC4,"set_control_mode");
    n_set_laser_mode = (N_SET_LASER_MODE_FP) GetProcAddress(gLibRTC4,"n_set_laser_mode");
    set_laser_mode = (SET_LASER_MODE_FP) GetProcAddress(gLibRTC4,"set_laser_mode");
    n_set_ext_start_delay = (N_SET_EXT_START_DELAY_FP) GetProcAddress(gLibRTC4,"n_set_ext_start_delay");
    set_ext_start_delay = (SET_EXT_START_DELAY_FP) GetProcAddress(gLibRTC4,"set_ext_start_delay");
    n_home_position = (N_HOME_POSITION_FP) GetProcAddress(gLibRTC4,"n_home_position");
    home_position = (HOME_POSITION_FP) GetProcAddress(gLibRTC4,"home_position");
    n_set_rot_center = (N_SET_ROT_CENTER_FP) GetProcAddress(gLibRTC4,"n_set_rot_center");
    set_rot_center = (SET_ROT_CENTER_FP) GetProcAddress(gLibRTC4,"set_rot_center");
    n_dsp_start = (N_DSP_START_FP) GetProcAddress(gLibRTC4,"n_dsp_start");
    dsp_start = (DSP_START_FP) GetProcAddress(gLibRTC4,"dsp_start");
    n_write_da_x = (N_WRITE_DA_X_FP) GetProcAddress(gLibRTC4,"n_write_da_x");
    write_da_x = (WRITE_DA_X_FP) GetProcAddress(gLibRTC4,"write_da_x");
    n_read_ad_x = (N_READ_AD_X_FP) GetProcAddress(gLibRTC4,"n_read_ad_x");
    read_ad_x = (READ_AD_X_FP) GetProcAddress(gLibRTC4,"read_ad_x");
    n_read_pixel_ad = (N_READ_PIXEL_AD_FP) GetProcAddress(gLibRTC4,"n_read_pixel_ad");
    read_pixel_ad = (READ_PIXEL_AD_FP) GetProcAddress(gLibRTC4,"read_pixel_ad");
    n_get_z_distance = (N_GET_Z_DISTANCE_FP) GetProcAddress(gLibRTC4,"n_get_z_distance");
    get_z_distance = (GET_Z_DISTANCE_FP) GetProcAddress(gLibRTC4,"get_z_distance");
    n_get_io_status = (N_GET_IO_STATUS_FP) GetProcAddress(gLibRTC4,"n_get_io_status");
    get_io_status = (GET_IO_STATUS_FP) GetProcAddress(gLibRTC4,"get_io_status");
    n_get_time = (N_GET_TIME_FP) GetProcAddress(gLibRTC4,"n_get_time");
    get_time = (GET_TIME_FP) GetProcAddress(gLibRTC4,"get_time");
    n_set_defocus = (N_SET_DEFOCUS_FP) GetProcAddress(gLibRTC4,"n_set_defocus");
    set_defocus = (SET_DEFOCUS_FP) GetProcAddress(gLibRTC4,"set_defocus");
    n_set_softstart_mode = (N_SET_SOFTSTART_MODE_FP) GetProcAddress(gLibRTC4,"n_set_softstart_mode");
    set_softstart_mode = (SET_SOFTSTART_MODE_FP) GetProcAddress(gLibRTC4,"set_softstart_mode");
    n_set_softstart_level = (N_SET_SOFTSTART_LEVEL_FP) GetProcAddress(gLibRTC4,"n_set_softstart_level");
    set_softstart_level = (SET_SOFTSTART_LEVEL_FP) GetProcAddress(gLibRTC4,"set_softstart_level");
    n_control_command = (N_CONTROL_COMMAND_FP) GetProcAddress(gLibRTC4,"n_control_command");
    control_command = (CONTROL_COMMAND_FP) GetProcAddress(gLibRTC4,"control_command");
    load_cor = (LOAD_COR_FP) GetProcAddress(gLibRTC4,"load_cor");
    load_pro = (LOAD_PRO_FP) GetProcAddress(gLibRTC4,"load_pro");
    n_get_serial_number = (N_GET_SERIAL_NUMBER_FP) GetProcAddress(gLibRTC4,"n_get_serial_number");
    get_serial_number = (GET_SERIAL_NUMBER_FP) GetProcAddress(gLibRTC4,"get_serial_number");
    n_get_serial_number_32 = (N_GET_SERIAL_NUMBER_32_FP) GetProcAddress(gLibRTC4,"n_get_serial_number_32");
    get_serial_number_32 = (GET_SERIAL_NUMBER_32_FP) GetProcAddress(gLibRTC4,"get_serial_number_32");
    n_get_rtc_version = (N_GET_RTC_VERSION_FP) GetProcAddress(gLibRTC4,"n_get_rtc_version");
    get_rtc_version = (GET_RTC_VERSION_FP) GetProcAddress(gLibRTC4,"get_rtc_version");
    get_hi_data = (GET_HI_DATA_FP) GetProcAddress(gLibRTC4,"get_hi_data");
    n_auto_cal = (N_AUTO_CAL_FP) GetProcAddress(gLibRTC4,"n_auto_cal");
    auto_cal = (AUTO_CAL_FP) GetProcAddress(gLibRTC4,"auto_cal");
    n_get_list_space = (N_GET_LIST_SPACE_FP) GetProcAddress(gLibRTC4,"n_get_list_space");
    get_list_space = (GET_LIST_SPACE_FP) GetProcAddress(gLibRTC4,"get_list_space");
    teachin = (TEACHIN_FP) GetProcAddress(gLibRTC4,"teachin");
    n_get_value = (N_GET_VALUE_FP) GetProcAddress(gLibRTC4,"n_get_value");
    get_value = (GET_VALUE_FP) GetProcAddress(gLibRTC4,"get_value");
    n_set_io_bit = (N_SET_IO_BIT_FP) GetProcAddress(gLibRTC4,"n_set_io_bit");
    set_io_bit = (SET_IO_BIT_FP) GetProcAddress(gLibRTC4,"set_io_bit");
    n_clear_io_bit = (N_CLEAR_IO_BIT_FP) GetProcAddress(gLibRTC4,"n_clear_io_bit");
    clear_io_bit = (CLEAR_IO_BIT_FP) GetProcAddress(gLibRTC4,"clear_io_bit");
    set_duty_cycle_table = (SET_DUTY_CYCLE_TABLE_FP) GetProcAddress(gLibRTC4,"set_duty_cycle_table");
    n_move_to = (N_MOVE_TO_FP) GetProcAddress(gLibRTC4,"n_move_to");
    move_to = (MOVE_TO_FP) GetProcAddress(gLibRTC4,"move_to");
    return(0);
}


//  RTC4close
//
//  This function terminates the explicit linking to the RTC4DLL.DLL or RTC4DLLx64.DLL.
//  Call it when the use of the RTC4 functions is finished.
void RTC4close(void) {
    if (gLibRTC4) {
        (void)FreeLibrary(gLibRTC4);
        gLibRTC4 = NULL;
    }
}
