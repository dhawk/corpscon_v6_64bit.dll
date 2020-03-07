#ifndef CCUSNG_H
#define CCUSNG_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

DLLEXPORT int _stdcall usng83_to_geo83(double *lat_out, double *lon_out,
													double *conv_out, double *kp_out,
                                       char *usng_in);
DLLEXPORT int _stdcall usng27_to_geo27(double *lat_out, double *lon_out,
													double *conv_out, double *kp_out,
                                       char *usng_in);
DLLEXPORT int _stdcall usng_to_utm(double *north_out, double *east_out, int *zone_out, char *usng_in);
DLLEXPORT int _stdcall geo83_to_usng83(double lat_in, double lon_in, int digits,
													char *usng_str, double *conv, double *kp);
DLLEXPORT int _stdcall geo27_to_usng27(double lat_in, double lon_in, int digits,
													char *usng_str, double *conv, double *kp);
int geo_to_usng(double lat_in, double lon_in, double north, double east, int digits, char *usng_str);
int usng_lon_to_gzd_col(double lon, int *col);
int usng_lat_to_gzd_row(double lat, int *row);
int usng_north_to_sid_row(double north, int *row);
int usng_east_to_sid_col(double east, int *col);
int letter_index(char *p);


#endif