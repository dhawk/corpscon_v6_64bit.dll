#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "usng.h"
#include "gridcons.h"
#include "grids83.h"
#include "ccerror.h"


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall usng83_to_geo83(double *lat_out, double *lon_out,
													double *conv_out, double *kp_out,
                                       char *usng_in)
{
double north;
double east;
int zone;
int rc;
double lat,lon;
double conv,kp;

	rc = usng_to_utm(&north,&east,&zone,usng_in);
   if(rc != CC_SUCCESS) return rc;

	rc = utm83_to_geo83(zone,METER,&lat,&lon,&north,&east,&conv,&kp);
   if(rc != CC_SUCCESS) return USNG_IN_CANNOT_CONVERT_USNG83_TO_GEO83;

   *lat_out = lat;
   *lon_out = lon;
   *conv_out = conv;
   *kp_out = kp;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall usng27_to_geo27(double *lat_out, double *lon_out,
													double *conv_out, double *kp_out,
                                       char *usng_in)
{
double north;
double east;
int zone;
int rc;
double lat,lon;
double conv,kp;

	rc = usng_to_utm(&north,&east,&zone,usng_in);
   if(rc != CC_SUCCESS) return rc;

	rc = utm27_to_geo27(zone,METER,&lat,&lon,&north,&east,&conv,&kp);
   if(rc != CC_SUCCESS) return USNG_IN_CANNOT_CONVERT_USNG27_TO_GEO27;

   *lat_out = lat;
   *lon_out = lon;
   *conv_out = conv;
   *kp_out = kp;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall usng_to_utm(double *north_out, double *east_out, int *zone_out, char *usng_in)
{
char usng[64];
char stmp[16];
double north, east;
double north_min,east_min;
double lat_min,lon_min;
double conv,kp;
int gzd_row,gzd_col;
int sid_row,sid_col;
int n;
int digits;
int rc;
char *p;

	if(usng_in == NULL) return USNG_IN_STRING_NULL;
	strcpy(usng,strupr(usng_in));
   sscanf(usng,"%s",usng);
   n = strlen(usng);
   if(n < 8 || n > 15) return USNG_IN_STRING_BAD_LENGTH;

   p = usng;

   if(n % 2){ strncpy(stmp,p,2); stmp[2] = '\0'; p+=2; }
   else { strncpy(stmp,p,1); stmp[1] = '\0'; p++; }
	gzd_col = atoi(stmp);
   if(gzd_col < 1 || gzd_col > 60) return USNG_IN_STRING_BAD_GZD_COL;

   gzd_row = letter_index(p++) - 2;
   if(gzd_row < 11 || gzd_row > 20) return USNG_IN_STRING_BAD_GZD_ROW;

   sid_col = letter_index(p++);
   if(sid_col < 1 || sid_col > 24) return USNG_IN_STRING_BAD_SID_COL;

   sid_row = letter_index(p++);
   if(sid_row < 1 || sid_row > 24) return USNG_IN_STRING_BAD_SID_ROW;

   switch(n)
   {
   	case 8: case 9: digits = 2; break;
      case 10: case 11: digits = 3; break;
      case 12: case 13: digits = 4; break;
      case 14: case 15: digits = 5; break;
      default: return USNG_IN_STRING_BAD_LENGTH;
   }
   strncpy(stmp,p,digits); stmp[digits] = '\0';
   rc = sscanf(stmp,"%lf",&east);
   if(rc != 1) return USNG_IN_STRING_BAD_EAST;
   p += digits;
   strncpy(stmp,p,digits); stmp[digits] = '\0';
   rc = sscanf(stmp,"%lf",&north);
   if(rc != 1) return USNG_IN_STRING_BAD_NORTH;

   switch(((gzd_col - 1) % 3) + 1)
   {
   	case 1: break;
      case 2: sid_col -= 9; break;
      case 3: sid_col -= 16; break;
      default: return USNG_IN_STRING_BAD_SID_COL;
   }
   if(sid_col < 1 || sid_col > 8) return USNG_IN_STRING_BAD_SID_COL;

   if(!(gzd_col % 2)) sid_row -= 5;
   if(sid_row < 1 || sid_row > 20) return USNG_IN_STRING_BAD_SID_ROW;

   lat_min = (gzd_row - 11) * 8.0;
   lon_min = 180.0 - (gzd_col - 1) * 6.0;
   rc = geo83_to_utm83(gzd_col,METER,&lat_min,&lon_min,&north_min,&east_min,&conv,&kp);
   if(rc != CC_SUCCESS) return USNG_IN_STRING_CANNOT_CALCULATE_NORTH_MIN;

   north += (sid_row - 1) * 100000.0;
   east += sid_col * 100000.0;

   while(north < north_min) north += 2000000.0;

/*
   printf("%s\n",usng);
   printf("north_min = %lf\n",north_min);
   printf("lat_min = %lf\n",lat_min);
   printf("lon_min = %lf\n",lon_min);
   printf("gzd_col = %d\n",gzd_col);
   printf("gzd_row = %d\n",gzd_row);
   printf("sid_col = %d\n",sid_col);
   printf("sid_row = %d\n",sid_row);
   printf("east = %lf\n",east);
   printf("north = %lf\n",north);
*/

   *north_out = north;
   *east_out = east;
   *zone_out = gzd_col;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int letter_index(char *p)
{
int val;

	if(*p < 'A' || *p == 'I' || *p == 'O' || *p > 'Z') return 0;
   val = (int) *p;
   val -= 64;
   if(val > 15) val--;
   if(val > 9) val--;

   return val;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall geo83_to_usng83(double lat_in, double lon_in, int digits,
													char *usng_str, double *conv, double *kp)
{
double north;
double east;
int rc;

	rc = geo83_to_utm83(0,METER,&lat_in,&lon_in,&north,&east,conv,kp);
   if(rc != CC_SUCCESS) return USNG_OUT_CANNOT_CONVERT_GEO83_TO_USNG83;

   rc = geo_to_usng(lat_in,lon_in,north,east,digits,usng_str);
   if(rc != CC_SUCCESS) return rc;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall geo27_to_usng27(double lat_in, double lon_in, int digits,
													char *usng_str, double *conv, double *kp)
{
double north;
double east;
int rc;

	rc = geo27_to_utm27(0,METER,&lat_in,&lon_in,&north,&east,conv,kp);
   if(rc != CC_SUCCESS) return USNG_OUT_CANNOT_CONVERT_GEO27_TO_USNG27;

   rc = geo_to_usng(lat_in,lon_in,north,east,digits,usng_str);
   if(rc != CC_SUCCESS) return rc;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int geo_to_usng(double lat_in, double lon_in, double north, double east, int digits, char *usng_str)
{
char abc[] = "ABCDEFGHJKLMNPQRSTUVWXYZ";
char stmp[64];
double factor;
int gzd_row,gzd_col;
int sid_row,sid_col;
int rc;

	strcpy(usng_str,"");

   if(digits < 2 || digits > 5) return USNG_OUT_BAD_DIGITS_SPECIFIED;

   rc = usng_lon_to_gzd_col(lon_in,&gzd_col);
   if(rc != CC_SUCCESS) return rc;

   rc = usng_lat_to_gzd_row(lat_in,&gzd_row);
   if(rc != CC_SUCCESS) return rc;

	rc = usng_north_to_sid_row(north,&sid_row);
   if(rc != CC_SUCCESS) return rc;
   if(!(gzd_col % 2)) sid_row += 5;
//   while(sid_row > 24) sid_row -= 24;
   while(sid_row > 20) sid_row -= 20;

	rc = usng_east_to_sid_col(east,&sid_col);
   if(rc != CC_SUCCESS) return rc;
   switch(((gzd_col - 1) % 3) + 1)
   {
   	case 1: break;
      case 2: sid_col += 8; break;
      case 3: sid_col += 16; break;
      default: return 0;
   }
   while(sid_col > 24) sid_col -= 24;

	factor = pow10(5 - digits);

   north = fmod(north,100000.0);
   east = fmod(east,100000.0);

   north = floor(north / factor);
   east = floor(east / factor);

	switch(digits)
   {
   	case 2: sprintf(stmp,"%02ld%02ld",(long)east,(long)north); break;
   	case 3: sprintf(stmp,"%03ld%03ld",(long)east,(long)north); break;
   	case 4: sprintf(stmp,"%04ld%04ld",(long)east,(long)north); break;
   	case 5: sprintf(stmp,"%05ld%05ld",(long)east,(long)north); break;
      default: return 0;
   }

   sprintf(usng_str,"%02d%c%c%c%s",gzd_col,abc[gzd_row+1],abc[sid_col-1],abc[sid_row-1],stmp);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
/* positive west */
int usng_lon_to_gzd_col(double lon, int *col)
{
   while(lon >= 180.0) lon -= 360.0;
   while(lon < -180.0) lon += 360.0;

	*col = 30 - (int) floor(lon / 6);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
/* positive north */
int usng_lat_to_gzd_row(double lat, int *row)
{
	if(lat < -90) return USNG_OUT_BAD_LAT;
	if(lat > 90) return USNG_OUT_BAD_LAT;

//   lat += 90.0;
   lat += 80.0;

   *row = ((int) floor(lat / 8.0)) + 1;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int usng_north_to_sid_row(double north, int *row)
{
double a;

	a = fmod(north,2000000.0);

   *row = ((int) floor(a / 100000.0)) + 1;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int usng_east_to_sid_col(double east, int *col)
{
double a;

	a = fmod(east,1000000.0);

   *col = ((int) floor(a / 100000.0));

   return CC_SUCCESS;
}
