#include <dir.h>
#include <stdio.h>
#include "ccfileutil.h"

#ifndef CCCONVERT_H
#define CCCONVERT_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define CCNAD27 1927
#define CCNAD83 1983
#define CCNAD91 1991

#define CCNGVD29 1929
#define CCNAVD88 1988
#define CCGRS80 1980

#define CCGEOID90 1990
#define CCGEOID93 1993
#define CCGEOID96 1996
#define CCGEOID99 1999
#define CCGEOID03 2003

#define CCGEOGRAPHIC 1
#define CCSTATEPLANE 2
#define CCUTM 3
#define CCUSNG 4

#define AVERAGE_R 6372160.0

#define X_OUT_SEARCH_CODE 1
#define Y_OUT_SEARCH_CODE 2
#define Z_OUT_SEARCH_CODE 3
#define USNG_OUT_SEARCH_CODE 4
#define KP_OUT_SEARCH_CODE 5
#define CF_OUT_SEARCH_CODE 6
#define CONV_OUT_SEARCH_CODE 7
#define DLAS_OUT_SEARCH_CODE 8
#define DLOS_OUT_SEARCH_CODE 9
#define DLAM_OUT_SEARCH_CODE 10
#define DLOM_OUT_SEARCH_CODE 11

#define X_IN_SEARCH_CODE 101
#define Y_IN_SEARCH_CODE 102
#define Z_IN_SEARCH_CODE 103
#define USNG_IN_SEARCH_CODE 104
#define KP_IN_SEARCH_CODE 105
#define CF_IN_SEARCH_CODE 106
#define CONV_IN_SEARCH_CODE 107
#define DLAS_IN_SEARCH_CODE 108
#define DLOS_IN_SEARCH_CODE 109
#define DLAM_IN_SEARCH_CODE 110
#define DLOM_IN_SEARCH_CODE 111
#define X_INPUT_FIELD_SEARCH_CODE 151
#define Y_INPUT_FIELD_SEARCH_CODE 152
#define Z_INPUT_FIELD_SEARCH_CODE 153

#define POINT_NAME_SEARCH_CODE 201
#define GHT_SEARCH_CODE 202
#define VHT_SEARCH_CODE 203
#define DLAM_TOTAL_SEARCH_CODE 204
#define DLOM_TOTAL_SEARCH_CODE 205
#define X_GRID_SHIFT_SEARCH_CODE 206
#define Y_GRID_SHIFT_SEARCH_CODE 207

DLLEXPORT int _stdcall (*g_usng2geo_function)();
DLLEXPORT int _stdcall (*g_grid2geo_function)();
DLLEXPORT int _stdcall (*g_togeo83_function)();
DLLEXPORT int _stdcall (*g_fromgeo83_function)();
DLLEXPORT int _stdcall (*g_geo2grid_function)();
DLLEXPORT int _stdcall (*g_geo2usng_function)();
DLLEXPORT int _stdcall (*g_geoid_function)();
DLLEXPORT int _stdcall (*g_vertcon_function)();
DLLEXPORT int _stdcall (*g_z_function1)();
DLLEXPORT int _stdcall (*g_z_function2)();
DLLEXPORT int _stdcall (*g_cf_function_in)();
DLLEXPORT int _stdcall (*g_cf_function_out)();
double (*g_vunit_function_in)();
double (*g_vunit_function_out)();
double (*g_grid_shift_unit_function)();

/* CONVERSION */
DLLEXPORT int _stdcall corpscon_initialize_convert(void);
DLLEXPORT int _stdcall corpscon_convert(void);
int corpscon_convert_internal(void);
DLLEXPORT int _stdcall corpscon_convert_line(char *line);
int corpscon_convert_line_internal(char *line);
DLLEXPORT int _stdcall corpscon_clean_up(void);
int xy_line_parse(double *x, double *y);

/* CONFIGURATION */
DLLEXPORT int _stdcall corpscon_default_config(void);
DLLEXPORT int _stdcall corpscon_config_from_script_file(char *script_file);
DLLEXPORT int _stdcall corpscon_set_config_from_script_line(char *line);
DLLEXPORT int _stdcall corpscon_save_config_to_script_file(char *script_file);
int delimiter_from_config(char *delimiter, char *delimiter_out);
int delimiter_to_config(char *delimiter_in, char *delimiter_out);

DLLEXPORT int _stdcall SetInSystem(int val);
DLLEXPORT int _stdcall SetOutSystem(int val);
DLLEXPORT int _stdcall SetInDatum(int val);
DLLEXPORT int _stdcall SetOutDatum(int val);
DLLEXPORT int _stdcall SetInUnits(int val);
DLLEXPORT int _stdcall SetOutUnits(int val);
DLLEXPORT int _stdcall SetInZone(int val);
DLLEXPORT int _stdcall SetOutZone(int val);
DLLEXPORT int _stdcall SetInVDatum(int val);
DLLEXPORT int _stdcall SetOutVDatum(int val);
DLLEXPORT int _stdcall SetInVUnits(int val);
DLLEXPORT int _stdcall SetOutVUnits(int val);
DLLEXPORT int _stdcall SetOutUSNGDigits(int val);
DLLEXPORT int _stdcall SetNadconPath(char *path);
DLLEXPORT int _stdcall SetInHPGNArea(char *area);
DLLEXPORT int _stdcall SetOutHPGNArea(char *area);
DLLEXPORT int _stdcall SetVertconPath(char *path);
DLLEXPORT int _stdcall SetVertconCustomAreaListFile(char *file);
DLLEXPORT int _stdcall SetUseVertconCustomAreas(int opt);
DLLEXPORT int _stdcall SetGeoidCodeBase(int val);
DLLEXPORT int _stdcall SetGeoidPath(char *path);
DLLEXPORT int _stdcall SetGeoidCustomAreaListFile(char *file);
DLLEXPORT int _stdcall SetUseGeoidCustomAreas(int opt);

/* SINGLE POINT CONVERSION */
DLLEXPORT int _stdcall SetXIn(double val);
DLLEXPORT int _stdcall SetYIn(double val);
DLLEXPORT int _stdcall SetZIn(double val);
DLLEXPORT int _stdcall SetUSNGIn(char *val);
DLLEXPORT int _stdcall GetConvertReturnCode(void);
DLLEXPORT double _stdcall GetXOut(void);
DLLEXPORT double _stdcall GetYOut(void);
DLLEXPORT double _stdcall GetZOut(void);
DLLEXPORT int _stdcall GetUSNGOut(char *val);
DLLEXPORT int _stdcall GetCorpsconValue(int code, double *val);
DLLEXPORT int _stdcall GetCorpsconValueStr(int code, char *str);


/* FILE CONVERSION */
DLLEXPORT int _stdcall SetXInParseField(int val);
DLLEXPORT int _stdcall SetYInParseField(int val);
DLLEXPORT int _stdcall SetZInParseField(int val);
DLLEXPORT int _stdcall SetUSNGInParseField(int val);
DLLEXPORT int _stdcall SetPointNameInField(int val);
DLLEXPORT int _stdcall SetXInFieldWidth(int val);
DLLEXPORT int _stdcall SetYInFieldWidth(int val);
DLLEXPORT int _stdcall SetXOutFieldWidth(int val);
DLLEXPORT int _stdcall SetYOutFieldWidth(int val);
DLLEXPORT int _stdcall SetInFieldDelimiter(char *val);
DLLEXPORT int _stdcall SetOutFieldDelimiter(char *val);
DLLEXPORT int _stdcall SetOutputLineFormat(char *format_str);
DLLEXPORT int _stdcall GetOutputLine(char *line);
DLLEXPORT int _stdcall GetInputField(int field, char *str);
DLLEXPORT int _stdcall WriteOutputStr(double val, int fields, int precision, char *str);
int ParseListForValue(CC_PARSELIST the_list, int start, int count, double *val);


/* RETRIEVING CONFIGURATION */
DLLEXPORT int _stdcall GetInSystem(void);
DLLEXPORT int _stdcall GetOutSystem(void);
DLLEXPORT int _stdcall GetInDatum(void);
DLLEXPORT int _stdcall GetOutDatum(void);
DLLEXPORT int _stdcall GetInUnits(void);
DLLEXPORT int _stdcall GetOutUnits(void);
DLLEXPORT int _stdcall GetInZone(void);
DLLEXPORT int _stdcall GetOutZone(void);
DLLEXPORT int _stdcall GetInVDatum(void);
DLLEXPORT int _stdcall GetOutVDatum(void);
DLLEXPORT int _stdcall GetInVUnits(void);
DLLEXPORT int _stdcall GetOutVUnits(void);
DLLEXPORT int _stdcall GetOutUSNGDigits(void);
DLLEXPORT int _stdcall GetNadconPath(char *path);
DLLEXPORT int _stdcall GetInHPGNArea(char *area);
DLLEXPORT int _stdcall GetOutHPGNArea(char *area);
DLLEXPORT int _stdcall GetVertconPath(char *path);
DLLEXPORT int _stdcall GetVertconCustomAreaListFile(char *filename);
DLLEXPORT int _stdcall GetUseVertconCustomAreas(void);
DLLEXPORT int _stdcall GetGeoidCodeBase(void);
DLLEXPORT int _stdcall GetGeoidPath(char *path);
DLLEXPORT int _stdcall GetGeoidCustomAreaListFile(char *filename);
DLLEXPORT int _stdcall GetUseGeoidCustomAreas(void);

/* INTERNAL FUNCTIONS USED BY corpscon_convert() */
int ngvd29_to_navd88(double zin, double *zout, double ght, double vht,
							int g_rc, int v_rc);
int navd88_to_ngvd29(double zin, double *zout, double ght, double vht,
							int g_rc, int v_rc);
int grs80_to_navd88(double zin, double *zout, double ght, double vht,
							int g_rc, int v_rc);
int navd88_to_grs80(double zin, double *zout, double ght, double vht,
							int g_rc, int v_rc);
int nad27_cf(double *cf, double z, double kp, double ght, int g_rc);
int nad83_cf(double *cf, double z, double kp, double ght, int g_rc);

DLLEXPORT int _stdcall dummy_grid2geo(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp);
DLLEXPORT int _stdcall dummy_usng2geo(double *lat, double *lon,
													double *conv, double *kp,
                                       char *usng_in);
DLLEXPORT int _stdcall dummy_geo2grid(int zone, int unit,
													double *lat_in, double *lon_in,
													double *north, double *east,
                              			double *conv, double *kp);
DLLEXPORT int _stdcall dummy_geo2usng(double lat_in, double lon_in,
													int digits, char *usng_str,
                              			double *conv, double *kp);
DLLEXPORT int _stdcall dummy_nad(double *xin, double *yin,
											double *xout, double *yout,
											double *dlas, double *dlos,
                                 double *dlam, double *dlom);
DLLEXPORT int _stdcall no_geoid_function(double x, double y, double *ght);
DLLEXPORT int _stdcall no_vertcon_function(double x, double y, double *vht);
int no_cf_function(double *cf, double z, double kp, double ght, int g_rc);
int dummy_z(double zin, double *zout, double ght, double vht,
							int g_rc, int v_rc);

typedef struct{
	double x;
   double y;
   double z;
   double dlas;
   double dlos;
   double dlam;
   double dlom;
   double kp;
   double conv;
   double cf;
   char usng[16];
}CORPSCON_IO_PT;

typedef struct{
	CORPSCON_IO_PT in;
	CORPSCON_IO_PT out;
   CC_PARSELIST fields;
   int field_count;
   double ght;
   double vht;
   char point_name[CCMAXFIELDWIDTH];
   int convert_return_code;
   double x_grid_shift;
   double y_grid_shift;
}CORPSCON_DATA_PT;

typedef struct{
	int field;
   int width;
   int precision;
}COORD_IO_CONFIG;

typedef struct{
	long system;
   long datum;
   long zone;
   long units;
   long vdatum;
   long vunits;
   char hpgn_area[MAXPATH];
   COORD_IO_CONFIG x;
   COORD_IO_CONFIG y;
   COORD_IO_CONFIG z;
   COORD_IO_CONFIG usng;
   int point_name_field;
   char field_delimiter[64];
}CORPSCON_IO_CONFIG;

typedef struct{
	int custom;
   char path[MAXPATH];
   char custom_file[MAXPATH];
}NGS_FILE_INFO;

typedef struct{
	CORPSCON_IO_CONFIG in;
	CORPSCON_IO_CONFIG out;
   NGS_FILE_INFO geoid;
   NGS_FILE_INFO nadcon;
   NGS_FILE_INFO vertcon;
   int output_format[CCMAXFIELDCOUNT];
   int output_fields;
   int geoid_code_base;
}CORPSCON_CONFIG;

#endif

