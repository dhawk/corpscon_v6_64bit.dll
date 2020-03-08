/*
  File:   grids83.c
  Author: Jeffrey G. Ruby
  Agency: U.S. Army Topographic Engineering Center
          ATTN:  CEERD-TR-S
          7701 Telegraph Road
          Alexandria, Virginia  22315-3864
          (703) 428-6766
          jeff.ruby@us.army.mil

	 This file contains all the routines for converting to/from
	 state plane, utms, and geographic coordinates.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <math.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>
#include "gridcons.h"
#include "grids83.h"
#include "ccerror.h"

/*
	 For information on any manual listed below,
	contact the NGS Information Branch:

	 NOAA, NGS, N/CG 174
	 SSMC3 Station 9202
	 1315 East West Highway
	 Silver Spring, Maryland  20910-3282
	 (301) 713-3242
*/

DLLEXPORT int _stdcall geo83_to_sp83(int zone, int unit,
													double *lat_in, double *lon_in,
													double *north, double *east,
                              			double *conv, double *kp)
{
static int last_zone=0;
static int last_unit=0;
static int (*grids83_function)();
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
int rc;

   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(zone != last_zone)
   {
		rc = get_grids83_function(GEO_TO_SP,zone,&grids83_function);
      if(rc != CC_SUCCESS) return rc;

		rc = init_sp83_constants(zone, &grids83_c);
      if(rc != CC_SUCCESS) return rc;
      last_zone = zone;
   }

   if(unit != last_unit)
   {
      rc = get_cvt_function(METER,unit,&unit_function);
      if(rc != CC_SUCCESS) return rc;
		last_unit = unit;
   }

   if(*grids83_function == NULL) return NULL_GRIDS83_FUNCTION;
   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

	rc = (*grids83_function)(&grids83_c,lat_in,lon_in,north,east,conv,kp);
   *north = (*unit_function)(*north);
   *east = (*unit_function)(*east);

   return rc;
}

DLLEXPORT int _stdcall sp83_to_geo83(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp)
{
static int last_zone=0;
static int last_unit=0;
static int (*grids83_function)();
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
double north,east;
int rc;

   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(zone != last_zone)
   {
		rc = get_grids83_function(SP_TO_GEO,zone,&grids83_function);
      if(rc != CC_SUCCESS) return rc;

		rc = init_sp83_constants(zone, &grids83_c);
      if(rc != CC_SUCCESS) return rc;
      last_zone = zone;
   }
   if(unit != last_unit)
   {
      rc = get_cvt_function(unit,METER,&unit_function);
      if(rc != CC_SUCCESS) return rc;
      last_unit = unit;
   }

   if(*grids83_function == NULL) return NULL_GRIDS83_FUNCTION;
   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   north = (*unit_function)(*north_in);
   east = (*unit_function)(*east_in);
	rc = (*grids83_function)(&grids83_c,lat,lon,&north,&east,conv,kp);

   return rc;
}

DLLEXPORT int _stdcall geo83_to_utm83(int zone, int unit,
													double *lat_in, double *lon_in,
													double *north, double *east,
                              			double *conv, double *kp)
{
static int initialized=0;
static int last_unit=0;
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
int itmp;
int rc;
double lon;

   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(!initialized)
   {
		grids83_c.tm83_c.a = GRS80_a;
		grids83_c.tm83_c.f = GRS80_f;
		grids83_c.tm83_c.fe = 500000.0;
		grids83_c.tm83_c.fn = 0.0;
		grids83_c.tm83_c.sf = 0.9996;
		grids83_c.tm83_c.or = 0.0;
		rc = init_tm_const_83(&grids83_c.tm83_c);
      if(rc != CC_SUCCESS) return rc;
      initialized = 1;
   }

   if(unit != last_unit)
   {
      rc = get_cvt_function(METER,unit,&unit_function);
      if(rc != CC_SUCCESS) return rc;
		last_unit = unit;
   }

   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   lon = *lon_in;
   while(lon >= 180.0) lon -= 360.0;
   while(lon < -180.0) lon += 360.0;
	if(zone == 0) zone = 30 - floor(lon / 6);

	itmp = zone<30 ? 183:543;
	grids83_c.tm83_c.cm = ((double) (itmp - (6 * zone))) / RAD;
   if(lon < 0.0) lon += 360.0;
	rc = geo83_to_tm83(&grids83_c,lat_in,&lon,north,east,conv,kp);
   *north = (*unit_function)(*north);
   *east = (*unit_function)(*east);

   return rc;
}

DLLEXPORT int _stdcall geo27_to_utm27(int zone, int unit,
													double *lat_in, double *lon_in,
													double *north, double *east,
                              			double *conv, double *kp)
{
static int initialized=0;
static int last_unit=0;
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
int itmp;
int rc;
double lon;

   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(!initialized)
   {
		grids83_c.tm83_c.a = CLARKE1866_a;
		grids83_c.tm83_c.f = CLARKE1866_f;
		grids83_c.tm83_c.fe = 500000.0;
		grids83_c.tm83_c.fn = 0.0;
		grids83_c.tm83_c.sf = 0.9996;
		grids83_c.tm83_c.or = 0.0;
		rc = init_tm_const_83(&grids83_c.tm83_c);
      if(rc != CC_SUCCESS) return rc;
      initialized = 1;
   }

   if(unit != last_unit)
   {
      rc = get_cvt_function(METER,unit,&unit_function);
      if(rc != CC_SUCCESS) return rc;
		last_unit = unit;
   }

   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   lon = *lon_in;
   while(lon >= 180.0) lon -= 360.0;
   while(lon < -180.0) lon += 360.0;
	if(zone == 0) zone = 30 - floor(lon / 6);

	itmp = zone<30 ? 183:543;
	grids83_c.tm83_c.cm = ((double) (itmp - (6 * zone))) / RAD;
   if(lon < 0) lon += 360.0;
	rc = geo83_to_tm83(&grids83_c,lat_in,&lon,north,east,conv,kp);
   *north = (*unit_function)(*north);
   *east = (*unit_function)(*east);

   return rc;
}

DLLEXPORT int _stdcall utm83_to_geo83(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp)
{
static int initialized=0;
static int last_unit=0;
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
int itmp;
double north,east;
int rc;

   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(!initialized)
   {
		grids83_c.tm83_c.a = GRS80_a;
		grids83_c.tm83_c.f = GRS80_f;
		grids83_c.tm83_c.fe = 500000.0;
		grids83_c.tm83_c.fn = 0.0;
		grids83_c.tm83_c.sf = 0.9996;
		grids83_c.tm83_c.or = 0.0;
		rc = init_tm_const_83(&grids83_c.tm83_c);
      if(rc != CC_SUCCESS) return rc;
      initialized = 1;
   }

   if(unit != last_unit)
   {
      rc = get_cvt_function(unit,METER,&unit_function);
      if(rc != CC_SUCCESS) return rc;
		last_unit = unit;
   }

   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   north = (*unit_function)(*north_in);
   east = (*unit_function)(*east_in);

	itmp = zone<30 ? 183:543;
	grids83_c.tm83_c.cm = ((double) (itmp - (6 * zone))) / RAD;
	rc = tm83_to_geo83(&grids83_c,lat,lon,&north,&east,conv,kp);

   if(*lon > 180.0) *lon -= 360.0;

   return rc;
}

DLLEXPORT int _stdcall utm27_to_geo27(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp)
{
static int initialized=0;
static int last_unit=0;
static double (*unit_function)();
static struct GRIDS83_CONSTANTS grids83_c;
int itmp;
double north,east;
int rc;

   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(!initialized)
   {
		grids83_c.tm83_c.a = CLARKE1866_a;
		grids83_c.tm83_c.f = CLARKE1866_f;
		grids83_c.tm83_c.fe = 500000.0;
		grids83_c.tm83_c.fn = 0.0;
		grids83_c.tm83_c.sf = 0.9996;
		grids83_c.tm83_c.or = 0.0;
		rc = init_tm_const_83(&grids83_c.tm83_c);
      if(rc != CC_SUCCESS) return rc;
      initialized = 1;
   }

   if(unit != last_unit)
   {
      rc = get_cvt_function(unit,METER,&unit_function);
      if(rc != CC_SUCCESS) return rc;
		last_unit = unit;
   }

   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   north = (*unit_function)(*north_in);
   east = (*unit_function)(*east_in);

	itmp = zone<30 ? 183:543;
	grids83_c.tm83_c.cm = ((double) (itmp - (6 * zone))) / RAD;
	rc = tm83_to_geo83(&grids83_c,lat,lon,&north,&east,conv,kp);

   if(*lon > 180.0) *lon -= 360.0;

   return rc;
}


DLLEXPORT int _stdcall describe_sp83_zone_constants(int zone, char *desc)
{
struct LAM83_CONSTANTS lam83_c;
struct TM83_CONSTANTS tm83_c;
int rc_lam;
int rc_tm;
char stmp[128];
char sign[2];
int deg,min;
double sec;

  	sprintf(desc,"SPCS 1983, Zone %04d\n\n",zone);
   
	rc_lam = init_lamb_83(zone, &lam83_c);
	rc_tm = init_tm_83(zone, &tm83_c);

   if(rc_lam != CC_SUCCESS && rc_tm != CC_SUCCESS)
   {
   	if(rc_lam == BAD_SP83_ZONE && rc_tm == BAD_SP83_ZONE) return BAD_SP83_ZONE;
   	if(rc_lam == NULL_LAM83_CONST_STRUCT) return NULL_LAM83_CONST_STRUCT;
   	return NULL_TM83_CONST_STRUCT;
   }

   if(rc_lam == CC_SUCCESS)
   {
   	strcat(desc,"Projection = Lambert\n");
      sprintf(stmp,"Semi-Major Axis of Ellipsoid = %.1lf\n",lam83_c.a);strcat(desc,stmp);
      sprintf(stmp,"Flattening = 1.0 \\ %.11lf\n",1.0 / lam83_c.f);strcat(desc,stmp);
      sprintf(stmp,"False Easting = %.1lf\n",lam83_c.fe);strcat(desc,stmp);
      sprintf(stmp,"False Northing = %.1lf\n",lam83_c.fn);strcat(desc,stmp);
      DD_to_DMS(lam83_c.pn * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Northern Standard Parallel = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);
      DD_to_DMS(lam83_c.ps * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Southern Standard Parallel = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);
      DD_to_DMS(lam83_c.pb * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Latitude of Grid Origin = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);
      DD_to_DMS(lam83_c.l0 * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Central Meridian = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);

   }
   else if(rc_tm == CC_SUCCESS)
   {
   	strcat(desc,"Projection = Transverse Mercator\n");
      sprintf(stmp,"Semi-Major Axis of Ellipsoid = %.1lf\n",tm83_c.a);strcat(desc,stmp);
      sprintf(stmp,"Flattening = 1.0 \\ %.11lf\n",1.0 / tm83_c.f);strcat(desc,stmp);
      sprintf(stmp,"False Easting = %.1lf\n",tm83_c.fe);strcat(desc,stmp);
      sprintf(stmp,"False Northing = %.1lf\n",tm83_c.fn);strcat(desc,stmp);
      DD_to_DMS(tm83_c.or * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Origin = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);
      DD_to_DMS(tm83_c.cm * RAD,sign,&deg,&min,&sec);
      sprintf(stmp,"Central Meridian = %3d %02d %02.0lf\n",deg,min,sec);strcat(desc,stmp);
      sprintf(stmp,"Scale Factor = (1.0 - 1.0 / %.1lf)\n",1.0 / (1.0 - tm83_c.sf));strcat(desc,stmp);
   }

   return CC_SUCCESS;
}


int get_grids83_function(int opt, int zone, int *sp_function)
{
	switch(zone)
	{
		/*AK1*/	case 5001:

						if(opt == GEO_TO_SP) *sp_function = Direct_AK1_83;
						else if(opt == SP_TO_GEO) *sp_function = Inverse_AK1_83;
                  else return BAD_GRIDS83_OPT;
						break;

		/*AK*/	case 5010:
		/*AR*/	case 301:case 302:
		/*CA*/	case 401:case 402:case 403:case 404:case 405:case 406:
		/*CO*/	case 501:case 502:case 503:
		/*CT*/	case 600:
		/*FL*/	case 903:
		/*IA*/	case 1401:case 1402:
		/*KS*/	case 1501:case 1502:
		/*KY*/	case 1600:case 1601:case 1602:
		/*LA*/	case 1701:case 1702:case 1703:
		/*MD*/	case 1900:
		/*MA*/	case 2001:case 2002:
		/*MI*/	case 2111:case 2112:case 2113:
		/*MN*/	case 2201:case 2202:case 2203:
		/*MT*/	case 2500:
		/*NE*/	case 2600:
		/*NY*/	case 3104:
		/*NC*/	case 3200:
		/*ND*/	case 3301:case 3302:
		/*OH*/	case 3401:case 3402:
		/*OK*/	case 3501:case 3502:
		/*OR*/	case 3601:case 3602:
		/*PA*/	case 3701:case 3702:
		/*SC*/	case 3900:
		/*SD*/	case 4001:case 4002:
		/*TN*/	case 4100:
		/*TX*/	case 4201:case 4202:case 4203:case 4204:case 4205:
		/*UT*/	case 4301:case 4302:case 4303:
		/*VA*/	case 4501:case 4502:
		/*WA*/	case 4601:case 4602:
		/*WV*/	case 4701:case 4702:
		/*WI*/	case 4801:case 4802:case 4803:
		/*PRVI*/	case 5200:

						if(opt == GEO_TO_SP) *sp_function = geo83_to_lam83;
						else if (opt == SP_TO_GEO) *sp_function = lam83_to_geo83;
                  else return BAD_GRIDS83_OPT;
						break;

		/*AL*/	case 101:case 102:
		/*AK*/	case 5002:case 5003:case 5004:case 5005:
					case 5006:case 5007:case 5008:case 5009:
		/*AZ*/	case 201:case 202:case 203:
		/*DE*/	case 700:
		/*FL*/	case 901:case 902:
		/*GA*/	case 1001:case 1002:
		/*HI*/	case 5101:case 5102:case 5103:case 5104:case 5105:
		/*ID*/	case 1101:case 1102:case 1103:
		/*IL*/	case 1201:case 1202:
		/*IN*/	case 1301:case 1302:
		/*ME*/	case 1801:case 1802:case 1811: case 1812: case 1813:
		/*MS*/	case 2301:case 2302:
		/*MO*/	case 2401:case 2402:case 2403:
		/*NV*/	case 2701:case 2702:case 2703:
		/*NH*/	case 2800:
		/*NJ*/	case 2900:
		/*NM*/	case 3001:case 3002:case 3003:
		/*NY*/	case 3101:case 3102:case 3103:
		/*RI*/	case 3800:
		/*VT*/	case 4400:
		/*WY*/	case 4901:case 4902:case 4903:case 4904:

						if(opt == GEO_TO_SP) *sp_function = geo83_to_tm83;
   	            else if (opt == SP_TO_GEO) *sp_function =	tm83_to_geo83;
                  else return BAD_GRIDS83_OPT;
						break;

		default:	return BAD_SP83_ZONE;
	}

	return CC_SUCCESS;
}

int init_sp83_constants(int zone, pGRIDS83_CONSTANTS c)
{
struct LAM83_CONSTANTS lam83_c;
struct TM83_CONSTANTS tm83_c;
int rc_lam;
int rc_tm;

	if(zone == 5001) return CC_SUCCESS; // AK 1 is initialize in it's own sub
	if(c == NULL) return NULL_GRIDS83_CONST_STRUCT;

	rc_lam = init_lamb_83(zone, &lam83_c);
	rc_tm = init_tm_83(zone, &tm83_c);

   if(rc_lam != CC_SUCCESS && rc_tm != CC_SUCCESS)
   {
   	if(rc_lam == BAD_SP83_ZONE && rc_tm == BAD_SP83_ZONE) return BAD_SP83_ZONE;
   	if(rc_lam == NULL_LAM83_CONST_STRUCT) return NULL_LAM83_CONST_STRUCT;
   	return NULL_TM83_CONST_STRUCT;
   }

   c->lam83_c = lam83_c;
   c->tm83_c = tm83_c;

   return CC_SUCCESS;
}

/*
	This routine converts geographic coordinates in lat/lon
	on NAD 83 to transverse merctor coordinates on NAD 83.
	Lat & Lon should be input as decimal degrees, positive north & west.
	North and east are returned in meters.  Convergence is
	returned in decimal degrees.  The TM83_CONSTANTS structure
	should be initialized with init_tm_83().

	 Formulas for the conversion come from:

	 NOAA Manual NOS NGS 5
	 State Plane Coordinate System of 1983
	 James E. Stem
	 National Geodetic Survey
	 Rockville, MD
	 January, 1989
*/
int geo83_to_tm83(pGRIDS83_CONSTANTS grids83_c,
						double *lat_in, double *lon_in,
                  double *north, double *east,
                  double *conv, double *kp)
{
double phi,lam,cosphi,sinphi,tn,tnsq,l,lsq,om,s,r,nsq;
double a1,a2,a3,a4,a5,a6,a7,c1,c3,c5,f2,f4;
double lat,lon;
struct TM83_CONSTANTS *c;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	c = &grids83_c->tm83_c;

	phi = lat / RAD;
	lam = lon / RAD;

	cosphi = cos(phi);
	sinphi = sin(phi);
	tn = sinphi / cosphi;
	tnsq = tn * tn;

	l = (lam - c->cm) * cosphi;
	lsq = l * l;

/* Below is what is in the book */
	om = phi + sinphi * cosphi * (c->U0 + c->U2 * cosphi * cosphi
				+ c->U4 * pow(cosphi,4.0) + c->U6 * pow(cosphi,6.0));

/********************************************************************
   This is what is in the NGS program UTM_S.  Above and below
   give slightly different answers in the Northing value sometimes.
  	om = phi + c->A * sin(2.0 * phi)
  				+ c->B * sin(4.0 * phi)
 				+ c->C * sin(6.0 * phi);
*********************************************************************/

	s = c->sf * om * c->r;

	r = c->sf * c->a / sqrt(1.0 - c->esq * sinphi * sinphi);

	a2 = r * tn / 2.0;

	nsq = c->esq * cosphi * cosphi / (1.0 - c->esq);

	a4 = (5.0 - tnsq + nsq * (9.0 + 4.0 * nsq)) / 12.0;

	a6 = (61.0 + tnsq * (tnsq - 58.0)
			+ nsq * (270.0 - 330.0 * tnsq)) / 360.0;

	*north = s - c->so + c->fn + a2 * lsq * (1.0 + lsq * (a4 + a6 * lsq));

	a1 = -r;

	a3 = (1.0 - tnsq + nsq) / 6.0;

	a5 = (5.0 - 18.0 * tnsq + pow(tn,4.0)
			+ nsq * (14.0 - 58.0 * tnsq)) / 120.0;

	a7 = (61.0 - 479.0 * tnsq + 179.0 * pow(tn,4.0) - pow(tn,6.0)) / 5040.0;

	*east = c->fe + a1 * l * (1.0 + lsq * (a3 + lsq * (a5 + a7 * lsq)));

	/* Convergence */

	c1 = -tn;

	c3 = (1.0 + 3.0 * nsq + 2.0 * nsq * nsq) / 3.0;

	c5 = (2.0 - tnsq) / 15.0;

	*conv = c1 * l * (1.0 + lsq * (c3 + c5 * lsq));
	*conv *= RAD;

	/* Scale Factor */

	f2 = (1.0 + nsq) / 2.0;

	f4 = (5.0 - 4.0 * tnsq + nsq * (9.0 - 24.0 * tnsq)) / 12.0;

	*kp = c->sf * (1.0 + f2 * lsq * (1.0 + f4 * lsq));

	return CC_SUCCESS;
}

/*
	This routine converts transverse mercator coordinates on
	 NAD 83 to geographic coordinates in lat/lon on NAD 83.
	 North and east should be input in meters.  Lat and Lon
	 are returned in decimal degrees, positive north and west.
	Convergence is returned in decimal degrees.  The
	TM83_CONSTANTS structure should be initialized with init_tm_83().

	 Formulas for the conversion come from:

	 NOAA Manual NOS NGS 5
	 State Plane Coordinate System of 1983
	 James E. Stem
	 National Geodetic Survey
    Rockville, MD
    January, 1989
*/
int tm83_to_geo83(pGRIDS83_CONSTANTS grids83_c,
						double *lat, double *lon,
                  double *north_in, double *east_in,
                  double *conv, double *kp)
{
double om,phif,rf,cosom,q,qsq,tf,tfsq,nfsq,l;
double b2,b3,b4,b5,b6,b7,d1,d3,d5,g2,g4;
double north,east;
struct TM83_CONSTANTS *c;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

	c = &grids83_c->tm83_c;

	om = (north - c->fn + c->so) / (c->sf * c->r);

	cosom = cos(om);

	phif = om + sin(om) * cosom * (c->V0 + c->V2 * cosom * cosom
				+ c->V4 * pow(cosom,4.0) + c->V6 * pow(cosom,6.0));

	rf = c->sf * c->a / sqrt(1.0 - c->esq * sin(phif) * sin(phif));

	q = (east - c->fe) / rf;
	qsq = q * q;

	tf = sin(phif) / cos(phif);
	tfsq = tf * tf;
	nfsq = c->esq * cos(phif) * cos(phif) / (1.0 - c->esq);

	b2 = -tf * (1.0 + nfsq) / 2.0;

	b4 = - (5.0 + 3.0 * tfsq + nfsq * (1.0 - 9.0 * tfsq)
			- 4.0 * nfsq * nfsq) / 12.0;

	b6 = (61.0 + 90.0 * tfsq + 45.0 * tfsq * tfsq
			+ nfsq * (46.0 - 252.0 * tfsq - 90.0 * tfsq * tfsq)) / 360.0;

	*lat = phif + b2 * qsq * (1.0 + qsq * (b4 + b6 * qsq));
	*lat *= RAD;

	b3 = -(1.0 + 2.0 * tfsq + nfsq) / 6.0;

	b5 = (5.0 + 28.0 * tfsq + 24.0 * tfsq * tfsq
			+ nfsq * (6.0 + 8.0 * tfsq)) / 120.0;

	b7 = -(61.0 + 662.0 * tfsq + 1320.0 * pow(tf,4.0)
			+ 720.0 * pow(tf,6.0)) / 5040.0;

	l = q * (1.0 + qsq * (b3 + qsq * (b5 + b7 * qsq)));

	*lon = c->cm - l / cos(phif);
	*lon *= RAD;

	d1 = tf;

	d3 = -(1.0 + tfsq - nfsq - 2.0 * nfsq * nfsq) / 3.0;

	d5 = (2.0 + 5.0 * tfsq + 3.0 * pow(tf,4.0)) / 15.0;

	*conv = d1 * q * (1.0 + qsq * (d3 + d5 * qsq));
	*conv *= RAD;

	g2 = (1.0 + nfsq) / 2.0;

	g4 = (1.0 + 5.0 * nfsq) / 12.0;

	*kp = c->sf * (1.0 + g2 * qsq * (1.0 + g4 * qsq));

	return CC_SUCCESS;
}

/*
	This routine converts geographic coordinates in lat/lon
	on NAD 83 to lambert conformal conic coordinates on NAD 83.
	Lat & Lon should be input as decimal degrees, positive north & west.
	North and east are returned in meters.  Convergence is
	returned in decimal degrees.  The LAM83_CONSTANTS structure
	should be initialized with init_lamb_83().

	 Formulas for the conversion come from:

	 NOAA Manual NOS NGS 5
	 State Plane Coordinate System of 1983
	 James E. Stem
	 National Geodetic Survey
	 Rockville, MD
	 January, 1989
*/
int geo83_to_lam83(pGRIDS83_CONSTANTS grids83_c,
							double *lat_in, double *lon_in,
                     double *north, double *east,
                     double *conv, double *kp)
{
double phi,lam,sinphi,q,r;
double lat,lon;
struct LAM83_CONSTANTS *c;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	c = &grids83_c->lam83_c;

	phi = lat / RAD;
	lam = lon / RAD;

	sinphi = sin(phi);

	q = (log((1.0 + sinphi) / (1.0 - sinphi)) - c->e
		* log((1.0 + c->e * sinphi) / (1.0 - c->e * sinphi))) / 2.0;

	r = c->k / exp(q * c->sinphi0);

	*conv = (c->l0 - lam) * c->sinphi0;

	*north = c->rb + c->fn - r * cos(*conv);

	*east = c->fe + r * sin(*conv);

	*kp = sqrt(1.0 - c->e * c->e * sinphi * sinphi)
			* (r * c->sinphi0) / (c->a * cos(phi));

	*conv *= RAD;

	return CC_SUCCESS;
}

/*
	This routine converts lambert conformal conic coordinates on
    NAD 83 to geographic coordinates in lat/lon on NAD 83.
    North and east should be input in meters.  Lat and Lon
    are returned in decimal degrees, positive north and west.
	Convergence is returned in decimal degrees.  The
	LAM83_CONSTANTS structure should be initialized with init_lamb_83().

    Formulas for the conversion come from:

    NOAA Manual NOS NGS 5
    State Plane Coordinate System of 1983
    James E. Stem
    National Geodetic Survey
    Rockville, MD
	 January, 1989
*/
int lam83_to_geo83(pGRIDS83_CONSTANTS grids83_c,
							double *lat, double *lon,
                     double *north_in, double *east_in,
                     double *conv, double *kp)
{
double r1,e1,r,q,sinphi,f1,f2;
int i;
double north, east;
struct LAM83_CONSTANTS *c;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

   c = &grids83_c->lam83_c;

	r1 = c->rb - north + c->fn;

	e1 = east - c->fe;

	*conv = atan(e1/r1);

	*lon = c->l0 - *conv / c->sinphi0;
	*lon *= RAD;

	r = sqrt(r1 * r1  + e1 * e1);

	q = log(c->k / r) / c->sinphi0;

	sinphi = (exp(2.0 * q) - 1.0) / (exp(2.0 * q) + 1.0);

	for(i=0;i<3;i++)
	{
		f1 = (log((1.0 + sinphi) / (1.0 - sinphi)) - c->e
				* log((1.0 + c->e * sinphi) / (1.0 - c->e * sinphi)))
				/ 2.0 - q;

		f2 = 1.0 / (1.0 - sinphi * sinphi)
				- c->e * c->e / (1.0 - c->e * c->e * sinphi * sinphi);

		sinphi = sinphi - f1 / f2;
	}

	*lat = asin(sinphi);

	*kp = sqrt(1.0 - c->e * c->e * sinphi * sinphi)
			* r * c->sinphi0 / (c->a * cos(*lat));

	*lat *= RAD;
	*conv *= RAD;

	return CC_SUCCESS;
}

/*
	This routine initializes the constants for lambert
	conformal conic grid projection conversion.  The
	 LAM83_CONSTANTS structure holds all the constants
	that have to be initialized.  Refer to gridcons.h
	for more info on LAM83_CONSTANTS.  Zone refers to
	 the state plane zone id.
*/
int init_lamb_83(int zone, pLAM83_CONSTANTS c)
{
double dtmp,qs,ws,qn,wn,qb;

	if(c == NULL) return NULL_LAM83_CONST_STRUCT;

	c->a = GRS80_a;
	c->f = GRS80_f;
	c->e = sqrt(c->f + c->f - c->f * c->f);

	 switch(zone)
	{
		case 5010:	/* AK 10 */
					c->fe = 1000000.0;
					c->fn = 0.0;
					c->ps = torad(51,50);
					c->pn = torad(53,50);
					c->pb = torad(51,0);
					c->l0 = torad(176,0);
					break;
		case 301:	/* AR North */
					c->fe = 400000.0;
					c->fn = 0.0;
					c->ps = torad(34,56);
					c->pn = torad(36,14);
					c->pb = torad(34,20);
					c->l0 = torad(92,0);
					break;
		case 302:	/* AR South */
					c->fe = 400000.0;
					c->fn = 400000.0;
					c->ps = torad(33,18);
					c->pn = torad(34,46);
					c->pb = torad(32,40);
					c->l0 = torad(92,0);
					break;
		case 401:	/* CA 1 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(40,0);
					c->pn = torad(41,40);
					c->pb = torad(39,20);
					c->l0 = torad(122,0);
					break;
		case 402:	/* CA 2 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(38,20);
					c->pn = torad(39,50);
					c->pb = torad(37,40);
					c->l0 = torad(122,0);
					break;
		case 403:	/* CA 3 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(37,4);
					c->pn = torad(38,26);
					c->pb = torad(36,30);
					c->l0 = torad(120,30);
					break;
		case 404:	/* CA 4 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(36,0);
					c->pn = torad(37,15);
					c->pb = torad(35,20);
					c->l0 = torad(119,0);
					break;
		case 405:	/* CA 5 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(34,2);
					c->pn = torad(35,28);
					c->pb = torad(33,30);
					c->l0 = torad(118,0);
					break;
		case 406:	/* CA 6 */
					c->fe = 2000000.0;
					c->fn = 500000.0;
					c->ps = torad(32,47);
					c->pn = torad(33,53);
					c->pb = torad(32,10);
					c->l0 = torad(116,15);
					break;
		case 501:	/* CO North */
					c->fe = 914401.8289;
					c->fn = 304800.6096;
					c->ps = torad(39,43);
					c->pn = torad(40,47);
					c->pb = torad(39,20);
					c->l0 = torad(105,30);
					break;
		case 502:	/* CO Central */
					c->fe = 914401.8289;
					c->fn = 304800.6096;
					c->ps = torad(38,27);
					c->pn = torad(39,45);
					c->pb = torad(37,50);
					c->l0 = torad(105,30);
					break;
		case 503:	/* CO South */
					c->fe = 914401.8289;
					c->fn = 304800.6096;
					c->ps = torad(37,14);
					c->pn = torad(38,26);
					c->pb = torad(36,40);
					c->l0 = torad(105,30);
					break;
		case 600:	/* CT */
					c->fe = 304800.6096;
					c->fn = 152400.3048;
					c->ps = torad(41,12);
					c->pn = torad(41,52);
					c->pb = torad(40,50);
					c->l0 = torad(72,45);
					break;
		case 903:	/* FL North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(29,35);
					c->pn = torad(30,45);
					c->pb = torad(29,0);
					c->l0 = torad(84,30);
					break;
		case 1401:	/* IA North */
					c->fe = 1500000.0;
					c->fn = 1000000.0;
					c->ps = torad(42,4);
					c->pn = torad(43,16);
					c->pb = torad(41,30);
					c->l0 = torad(93,30);
					break;
		case 1402:	/* IA South */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(40,37);
					c->pn = torad(41,47);
					c->pb = torad(40,0);
					c->l0 = torad(93,30);
					break;
		case 1501:	/* KS North */
					c->fe = 400000.0;
					c->fn = 0.0;
					c->ps = torad(38,43);
					c->pn = torad(39,47);
					c->pb = torad(38,20);
					c->l0 = torad(98,0);
					break;
		case 1502:	/* KS South */
					c->fe = 400000.0;
					c->fn = 400000.0;
					c->ps = torad(37,16);
					c->pn = torad(38,34);
					c->pb = torad(36,40);
					c->l0 = torad(98,30);
					break;
      case 1600:  /* KY Single Zone */
					c->fe = 1500000.0;
					c->fn = 1000000.0;
					c->ps = torad(37,5);
					c->pn = torad(38,40);
					c->pb = torad(36,20);
					c->l0 = torad(85,45);
      			break;
		case 1601:	/* KY North */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(37,58);
					c->pn = torad(38,58);
					c->pb = torad(37,30);
					c->l0 = torad(84,15);
					break;
		case 1602:	/* KY South */
					c->fe = 500000.0;
					c->fn = 500000.0;
					c->ps = torad(36,44);
					c->pn = torad(37,56);
					c->pb = torad(36,20);
					c->l0 = torad(85,45);
					break;
		case 1701:	/* LA North */
					c->fe = 1000000.0;
					c->fn = 0.0;
					c->ps = torad(31,10);
					c->pn = torad(32,40);
					c->pb = torad(30,30);
					c->l0 = torad(92,30);
					break;
		case 1702:	/* LA South */
					c->fe = 1000000.0;
					c->fn = 0.0;
					c->ps = torad(29,18);
					c->pn = torad(30,42);
					c->pb = torad(28,30);
					c->l0 = torad(91,20);
					break;
		case 1703:	/* LA Offshore */
					c->fe = 1000000.0;
					c->fn = 0.0;
					c->ps = torad(26,10);
					c->pn = torad(27,50);
					c->pb = torad(25,30);
					c->l0 = torad(91,20);
					break;
		case 1900:	/* MD */
					c->fe = 400000.0;
					c->fn = 0.0;
					c->ps = torad(38,18);
					c->pn = torad(39,27);
					c->pb = torad(37,40);
					c->l0 = torad(77,0);
					break;
		case 2001:	/* MA Mainland */
					c->fe = 200000.0;
					c->fn = 750000.0;
					c->ps = torad(41,43);
					c->pn = torad(42,41);
					c->pb = torad(41,0);
					c->l0 = torad(71,30);
					break;
		case 2002:	/* MA Island */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(41,17);
					c->pn = torad(41,29);
					c->pb = torad(41,0);
					c->l0 = torad(70,30);
					break;
		case 2111:	/* MI North */
					c->fe = 8000000.0;
					c->fn = 0.0;
					c->ps = torad(45,29);
					c->pn = torad(47,5);
					c->pb = torad(44,47);
					c->l0 = torad(87,0);
					break;
		case 2112:	/* MI Central */
					c->fe = 6000000.0;
					c->fn = 0.0;
					c->ps = torad(44,11);
					c->pn = torad(45,42);
					c->pb = torad(43,19);
					c->l0 = torad(84,22);
					break;
		case 2113:	/* MI South */
					c->fe = 4000000.0;
					c->fn = 0.0;
					c->ps = torad(42,6);
					c->pn = torad(43,40);
					c->pb = torad(41,30);
					c->l0 = torad(84,22);
					break;
		case 2201:	/* MN North */
					c->fe = 800000.0;
					c->fn = 100000.0;
					c->ps = torad(47,2);
					c->pn = torad(48,38);
					c->pb = torad(46,30);
					c->l0 = torad(93,6);
					break;
		case 2202:	/* MN Central */
					c->fe = 800000.0;
					c->fn = 100000.0;
					c->ps = torad(45,37);
					c->pn = torad(47,3);
					c->pb = torad(45,0);
					c->l0 = torad(94,15);
					break;
		case 2203:	/* MN South */
					c->fe = 800000.0;
					c->fn = 100000.0;
					c->ps = torad(43,47);
					c->pn = torad(45,13);
					c->pb = torad(43,0);
					c->l0 = torad(94,0);
					break;
		case 2500:	/* MT */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(45,0);
					c->pn = torad(49,0);
					c->pb = torad(44,15);
					c->l0 = torad(109,30);
					break;
		case 2600:	/* NE */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(40,0);
					c->pn = torad(43,0);
					c->pb = torad(39,50);
					c->l0 = torad(100,0);
					break;
		case 3104:	/* NY Long Island */
					c->fe = 300000.0;
					c->fn = 0.0;
					c->ps = torad(40,40);
					c->pn = torad(41,2);
					c->pb = torad(40,10);
					c->l0 = torad(74,0);
					break;
		case 3200:	/* NC */
					c->fe = 609601.22;
					c->fn = 0.0;
					c->ps = torad(34,20);
					c->pn = torad(36,10);
					c->pb = torad(33,45);
					c->l0 = torad(79,0);
					break;
		case 3301:	/* ND North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(47,26);
					c->pn = torad(48,44);
					c->pb = torad(47,0);
					c->l0 = torad(100,30);
					break;
		case 3302:	/* ND South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(46,11);
					c->pn = torad(47,29);
					c->pb = torad(45,40);
					c->l0 = torad(100,30);
					break;
		case 3401:	/* OH North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(40,26);
					c->pn = torad(41,42);
					c->pb = torad(39,40);
					c->l0 = torad(82,30);
					break;
		case 3402:	/* OH South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(38,44);
					c->pn = torad(40,2);
					c->pb = torad(38,0);
					c->l0 = torad(82,30);
					break;
		case 3501:	/* OK North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(35,34);
					c->pn = torad(36,46);
					c->pb = torad(35,0);
					c->l0 = torad(98,0);
					break;
		case 3502:	/* OK South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(33,56);
					c->pn = torad(35,14);
					c->pb = torad(33,20);
					c->l0 = torad(98,0);
					break;
		case 3601:	/* OR North */
					c->fe = 2500000.0;
					c->fn = 0.0;
					c->ps = torad(44,20);
					c->pn = torad(46,0);
					c->pb = torad(43,40);
					c->l0 = torad(120,30);
					break;
		case 3602:	/* OR South */
					c->fe = 1500000.0;
					c->fn = 0.0;
					c->ps = torad(42,20);
					c->pn = torad(44,0);
					c->pb = torad(41,40);
					c->l0 = torad(120,30);
					break;
		case 3701:	/* PA North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(40,53);
					c->pn = torad(41,57);
					c->pb = torad(40,10);
					c->l0 = torad(77,45);
					break;
		case 3702:	/* PA South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(39,56);
					c->pn = torad(40,58);
					c->pb = torad(39,20);
					c->l0 = torad(77,45);
					break;
		case 3900:	/* SC */
					c->fe = 609600.0;
					c->fn = 0.0;
					c->ps = torad(32,30);
					c->pn = torad(34,50);
					c->pb = torad(31,50);
					c->l0 = torad(81,0);
					break;
		case 4001:	/* SD North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(44,25);
					c->pn = torad(45,41);
					c->pb = torad(43,50);
					c->l0 = torad(100,0);
					break;
		case 4002:	/* SD South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(42,50);
					c->pn = torad(44,24);
					c->pb = torad(42,20);
					c->l0 = torad(100,20);
					break;
		case 4100:	/* TN */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(35,15);
					c->pn = torad(36,25);
					c->pb = torad(34,20);
					c->l0 = torad(86,0);
					break;
		case 4201:	/* TX North */
					c->fe = 200000.0;
					c->fn = 1000000.0;
					c->ps = torad(34,39);
					c->pn = torad(36,11);
					c->pb = torad(34,0);
					c->l0 = torad(101,30);
					break;
		case 4202:	/* TX North Central */
					c->fe = 600000.0;
					c->fn = 2000000.0;
					c->ps = torad(32,8);
					c->pn = torad(33,58);
					c->pb = torad(31,40);
					c->l0 = torad(98,30);
					break;
		case 4203:	/* TX Central */
					c->fe = 700000.0;
					c->fn = 3000000.0;
					c->ps = torad(30,7);
					c->pn = torad(31,53);
					c->pb = torad(29,40);
					c->l0 = torad(100,20);
					break;
		case 4204:	/* TX South Central */
					c->fe = 600000.0;
					c->fn = 4000000.0;
					c->ps = torad(28,23);
					c->pn = torad(30,17);
					c->pb = torad(27,50);
					c->l0 = torad(99,0);
					break;
		case 4205:	/* TX South */
					c->fe = 300000.0;
					c->fn = 5000000.0;
					c->ps = torad(26,10);
					c->pn = torad(27,50);
					c->pb = torad(25,40);
					c->l0 = torad(98,30);
					break;
		case 4301:	/* UT North */
					c->fe = 500000.0;
					c->fn = 1000000.0;
					c->ps = torad(40,43);
					c->pn = torad(41,47);
					c->pb = torad(40,20);
					c->l0 = torad(111,30);
					break;
		case 4302:	/* UT Central */
					c->fe = 500000.0;
					c->fn = 2000000.0;
					c->ps = torad(39,1);
					c->pn = torad(40,39);
					c->pb = torad(38,20);
					c->l0 = torad(111,30);
					break;
		case 4303:	/* UT South */
					c->fe = 500000.0;
					c->fn = 3000000.0;
					c->ps = torad(37,13);
					c->pn = torad(38,21);
					c->pb = torad(36,40);
					c->l0 = torad(111,30);
					break;
		case 4501:	/* VA North */
					c->fe = 3500000.0;
					c->fn = 2000000.0;
					c->ps = torad(38,2);
					c->pn = torad(39,12);
					c->pb = torad(37,40);
					c->l0 = torad(78,30);
					break;
		case 4502:	/* VA South */
					c->fe = 3500000.0;
					c->fn = 1000000.0;
					c->ps = torad(36,46);
					c->pn = torad(37,58);
					c->pb = torad(36,20);
					c->l0 = torad(78,30);
					break;
		case 4601:	/* WA North */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(47,30);
					c->pn = torad(48,44);
					c->pb = torad(47,0);
					c->l0 = torad(120,50);
					break;
		case 4602:	/* WA South */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->ps = torad(45,50);
					c->pn = torad(47,20);
					c->pb = torad(45,20);
					c->l0 = torad(120,30);
					break;
		case 4701:	/* WV North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(39,0);
					c->pn = torad(40,15);
					c->pb = torad(38,30);
					c->l0 = torad(79,30);
					break;
		case 4702:	/* WV South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(37,29);
					c->pn = torad(38,53);
					c->pb = torad(37,0);
					c->l0 = torad(81,0);
					break;
		case 4801:	/* WI North */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(45,34);
					c->pn = torad(46,46);
					c->pb = torad(45,10);
					c->l0 = torad(90,0);
					break;
		case 4802:	/* WI Central */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(44,15);
					c->pn = torad(45,30);
					c->pb = torad(43,50);
					c->l0 = torad(90,0);
					break;
		case 4803:	/* WI South */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->ps = torad(42,44);
					c->pn = torad(44,4);
					c->pb = torad(42,0);
					c->l0 = torad(90,0);
					break;
		case 5200:	/* PR & VI */
					c->fe = 200000.0;
					c->fn = 200000.0;
					c->ps = torad(18,2);
					c->pn = torad(18,26);
					c->pb = torad(17,50);
					c->l0 = torad(66,26);
					break;
		default:	return BAD_SP83_ZONE;
	}

	dtmp = sin(c->ps);
	qs = (log((1.0 + dtmp) / (1.0 - dtmp))
			- c->e * log((1.0 + c->e * dtmp) / (1.0 - c->e * dtmp))) / 2.0;
	ws = sqrt(1.0 - c->e * c->e * dtmp * dtmp);

    dtmp = sin(c->pn);
	qn = (log((1.0 + dtmp) / (1.0 - dtmp))
			- c->e * log((1.0 + c->e * dtmp) / (1.0 - c->e * dtmp))) / 2.0;
	 wn = sqrt(1.0 - c->e * c->e * dtmp * dtmp);

    dtmp = sin(c->pb);
    qb = (log((1.0 + dtmp) / (1.0 - dtmp))
			- c->e * log((1.0 + c->e * dtmp) / (1.0 - c->e * dtmp))) / 2.0;

    c->sinphi0 = log(wn * cos(c->ps) / (ws * cos(c->pn))) / (qn - qs);

	 c->k = c->a * cos(c->ps) * exp(qs * c->sinphi0) / (ws * c->sinphi0);

    c->rb = c->k / exp(qb * c->sinphi0);

	return CC_SUCCESS;
}

/*
	This routine initializes the constants for transverse
	mercator grid projection conversion.  The
	 TM83_CONSTANTS structure holds all the constants
	 that have to be initialized.  Refer to gridcons.h
	 for more info on TM83_CONSTANTS.  Zone refers to
	 the state plane zone id.
*/
int init_tm_83(int zone, pTM83_CONSTANTS c)
{
	if(c == NULL) return NULL_TM83_CONST_STRUCT;

	c->a = GRS80_a;
	c->f = GRS80_f;

	switch(zone)
	{
		case 101:   /* AL East */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(25000.0);
					c->cm = torad(85,50);
					c->or = torad(30,30);
					break;
		case 102:   /* AL West */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->sf = compute_sf(15000.0);
					c->cm = torad(87,30);
					c->or = torad(30,0);
					break;
		case 5002:  /* AK 2 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(142,0);
					c->or = torad(54,0);
					break;
		case 5003:  /* AK 3 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(146,0);
					c->or = torad(54,0);
					break;
		  case 5004:  /* AK 4 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(150,0);
					c->or = torad(54,0);
					break;
		  case 5005:  /* AK 5 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(154,0);
					c->or = torad(54,0);
					break;
		  case 5006:  /* AK 6 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(158,0);
					c->or = torad(54,0);
					break;
		  case 5007:  /* AK 7 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(162,0);
					c->or = torad(54,0);
					break;
		  case 5008:	/* AK 8 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(166,0);
					c->or = torad(54,0);
					break;
		case 5009:	/* AK 9 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(170,0);
					c->or = torad(54,0);
					break;
		case 201:   /* AZ East */
					c->fe = 213360.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(110,10);
					c->or = torad(31,0);
					break;
		case 202:   /* AZ Central */
					c->fe = 213360.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(111,55);
					c->or = torad(31,0);
					break;
		case 203:   /* AZ West */
					c->fe = 213360.0;
					c->fn = 0.0;
					c->sf = compute_sf(15000.0);
					c->cm = torad(113,45);
					c->or = torad(31,0);
					break;
		case 700:   /* DE */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(200000.0);
					c->cm = torad(75,25);
					c->or = torad(38,0);
					break;
		case 901:   /* FL East */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(17000.0);
					c->cm = torad(81,0);
					c->or = torad(24,20);
					break;
		case 902:   /* FL West */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(17000.0);
					c->cm = torad(82,0);
					c->or = torad(24,20);
					break;
		case 1001:  /* GA East */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(82,10);
					c->or = torad(30,0);
					break;
		case 1002:  /* GA West */
					c->fe = 700000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(84,10);
					c->or = torad(30,0);
					break;
		case 5101:  /* HI 1 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(155,30);
					c->or = torad(18,50);
					break;
		case 5102:  /* HI 2 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(156,40);
					c->or = torad(20,20);
					break;
		case 5103:  /* HI 3 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(100000.0);
					c->cm = torad(158,0);
					c->or = torad(21,10);
					break;
		case 5104:  /* HI 4 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(100000.0);
					c->cm = torad(159,30);
					c->or = torad(21,50);
					break;
		case 5105:  /* HI 5 */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = 1.0;
					c->cm = torad(160,10);
					c->or = torad(21,40);
					break;
		case 1101:  /* ID East */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(19000.0);
					c->cm = torad(112,10);
					c->or = torad(41,40);
					break;
		case 1102:  /* ID Central */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(19000.0);
					c->cm = torad(114,0);
					c->or = torad(41,40);
					break;
		case 1103:  /* ID West */
					c->fe = 800000.0;
					c->fn = 0.0;
					c->sf = compute_sf(15000.0);
					c->cm = torad(115,45);
					c->or = torad(41,40);
					break;
		case 1201:  /* IL East */
					c->fe = 300000.0;
					c->fn = 0.0;
					c->sf = compute_sf(40000.0);
					c->cm = torad(88,20);
					c->or = torad(36,40);
					break;
		case 1202:  /* IL West */
					c->fe = 700000.0;
					c->fn = 0.0;
					c->sf = compute_sf(17000.0);
					c->cm = torad(90,10);
					c->or = torad(36,40);
					break;
		case 1301:  /* IN East */
					c->fe = 100000.0;
					c->fn = 250000.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(85,40);
					c->or = torad(37,30);
					break;
		case 1302:  /* IN West */
					c->fe = 900000.0;
					c->fn = 250000.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(87,5);
					c->or = torad(37,30);
					break;
		case 1801:  /* ME East */
					c->fe = 300000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(68,30);
					c->or = torad(43,40);
					break;
		case 1802:  /* ME West */
					c->fe = 900000.0;
					c->fn = 0.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(70,10);
					c->or = torad(42,50);
					break;
		case 1811:  /* ME East 2000*/
					c->fe = 700000.0;
					c->fn = 0.0;
					c->sf = compute_sf(50000.0);
					c->cm = torad(67,52.5);
					c->or = torad(43,50);
					break;
		case 1812:  /* ME Central 2000*/
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(50000.0);
					c->cm = torad(69,7.5);
					c->or = torad(43,30);
					break;
		case 1813:  /* ME West 2000*/
					c->fe = 300000.0;
					c->fn = 0.0;
					c->sf = compute_sf(50000.0);
					c->cm = torad(70,22.5);
					c->or = torad(42,50);
					break;
		case 2301:  /* MS East */
					c->fe = 300000.0;
					c->fn = 0.0;
					c->sf = compute_sf(20000.0);
					c->cm = torad(88,50);
					c->or = torad(29,30);
					break;
		case 2302:  /* MS West */
					c->fe = 700000.0;
					c->fn = 0.0;
					c->sf = compute_sf(20000.0);
					c->cm = torad(90,20);
					c->or = torad(29,30);
					break;
		case 2401:  /* MO East */
					c->fe = 250000.0;
					c->fn = 0.0;
					c->sf = compute_sf(15000.0);
					c->cm = torad(90,30);
					c->or = torad(35,50);
					break;
		case 2402:  /* MO Central */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(15000.0);
					c->cm = torad(92,30);
					c->or = torad(35,50);
					break;
		case 2403:  /* MO West */
					c->fe = 850000.0;
					c->fn = 0.0;
					c->sf = compute_sf(17000.0);
					c->cm = torad(94,30);
					c->or = torad(36,10);
					break;
		case 2701:  /* NV East */
					c->fe = 200000.0;
					c->fn = 8000000.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(115,35);
					c->or = torad(34,45);
					break;
		case 2702:  /* NV Central */
					c->fe = 500000.0;
					c->fn = 6000000.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(116,40);
					c->or = torad(34,45);
					break;
		case 2703:  /* NV West */
					c->fe = 800000.0;
					c->fn = 4000000.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(118,35);
					c->or = torad(34,45);
					break;
		case 2800:  /* NH */
					c->fe = 300000.0;
					c->fn = 0.0;
					c->sf = compute_sf(30000.0);
					c->cm = torad(71,40);
					c->or = torad(42,30);
					break;
		case 2900:  /* NJ */
					c->fe = 150000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(74,30);
					c->or = torad(38,50);
					break;
		case 3001:  /* NM East */
					c->fe = 165000.0;
					c->fn = 0.0;
					c->sf = compute_sf(11000.0);
					c->cm = torad(104,20);
					c->or = torad(31,0);
					break;
		case 3002:  /* NM Central */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(106,15);
					c->or = torad(31,0);
					break;
		case 3003:  /* NM West */
					c->fe = 830000.0;
					c->fn = 0.0;
					c->sf = compute_sf(12000.0);
					c->cm = torad(107,50);
					c->or = torad(31,0);
					break;
		case 3101:  /* NY East */
					c->fe = 150000.0;
					c->fn = 0.0;
					c->sf = compute_sf(10000.0);
					c->cm = torad(74,30);
					c->or = torad(38,50);
					break;
		case 3102:  /* NY Central */
					c->fe = 250000.0;
					c->fn = 0.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(76,35);
					c->or = torad(40,0);
					break;
		case 3103:  /* NY West */
					c->fe = 350000.0;
					c->fn = 0.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(78,35);
					c->or = torad(40,0);
					break;
		case 3800:  /* RI */
					c->fe = 100000.0;
					c->fn = 0.0;
					c->sf = compute_sf(160000.0);
					c->cm = torad(71,30);
					c->or = torad(41,5);
					break;
		case 4400:  /* VT */
					c->fe = 500000.0;
					c->fn = 0.0;
					c->sf = compute_sf(28000.0);
					c->cm = torad(72,30);
					c->or = torad(42,30);
					break;
		case 4901:  /* WY East */
					c->fe = 200000.0;
					c->fn = 0.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(105,10);
					c->or = torad(40,30);
					break;
		case 4902:  /* WY East Central */
					c->fe = 400000.0;
					c->fn = 100000.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(107,20);
					c->or = torad(40,30);
					break;
		case 4903:  /* WY West Central */
					c->fe = 600000.0;
					c->fn = 0.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(108,45);
					c->or = torad(40,30);
					break;
		case 4904:  /* WY West */
					c->fe = 800000.0;
					c->fn = 100000.0;
					c->sf = compute_sf(16000.0);
					c->cm = torad(110,5);
					c->or = torad(40,30);
					break;
		default:	return BAD_SP83_ZONE;
	}

	return init_tm_const_83(c);
}

/*
	This routine is not actually used, but was written
	 to test constants.  It showed that the F constant
	 in NGS NOS 5 pg. 40 should have had a negative sign.
	 It was left in for possible later use.

	This routine initializes the constants for transverse
	mercator grid projection conversion.  The
	 OM83_CONSTANTS structure holds all the constants
	 that have to be initialized.  Refer to gridcons.h
	 for more info on OM83_CONSTANTS.  Zone refers to
	 the state plane zone id.
*/
int init_om_83(int zone, pOM83_CONSTANTS c)
{
double epsq,cosphic,sinphic,wc,a,qc,x,alphao;

	if(c == NULL) return NULL_OM83_CONST_STRUCT;

	 c->a = GRS80_a;
	 c->flat = GRS80_f;
	 c->esq = c->flat + c->flat - c->flat * c->flat;
	 c->e = sqrt(c->esq);

	c->f0 = 0.006686920927;
	c->f2 = 0.000052014584;
	c->f4 = 0.000000554430;
	c->f6 = 0.000000006820;

	switch(zone)
	{
   	case 5001:  /* AK 1 */
      			c->fe = 5000000.0;
               c->fn = -5000000.0;
               c->sf = compute_sf(10000.0);
					c->phic = torad(57,0);
               c->lamc = torad(133,40);
               c->alphac = atan(-3.0 / 4.0);
					break;
   	default:	return 0;
    }

    epsq = c->esq / (1.0 - c->esq);

    cosphic = cos(c->phic);
    sinphic = sin(c->phic);

    c->b = sqrt(1.0 + epsq * pow(cosphic,4));
    wc = sqrt(1.0 - c->esq * sinphic * sinphic);

	 a = c->a * c->b * sqrt(1.0 - c->esq) / (wc * wc);

	qc = 0.5 * (log((1.0 + sinphic) / (1.0 - sinphic)) -
		c->e * log((1.0 + c->e * sinphic) / (1.0 - c->e * sinphic)));

	x = c->b * sqrt(1.0 - c->esq) / (wc * cosphic);

    c->c = log(x + sqrt(x * x - 1.0)) - c->b * qc;

	 c->d = c->sf * a / c->b;

    c->f = (c->a * sin(c->alphac) * cos(c->phic)) / (a * wc);

	 alphao = asin(c->f);

	 c->g = cos(alphao);

	 c->lamo = c->lamc + (asin(c->f * sinh(c->b * qc + c->c) / c->g)) / c->b;

	 c->i = c->sf * a / c->a;

	 return 1;
}

/*
	This routine initializes the constants for transverse
	mercator grid projection conversion.  The
	 TM83_CONSTANTS structure holds all the constants
	 that have to be initialized.  Refer to gridcons.h
	 for more info on TM83_CONSTANTS.  Zone refers to
	 the state plane zone id.
*/
int init_tm_const_83(pTM83_CONSTANTS c)
{
double n,u2,u4,u6,u8,v2,v4,v6,v8,cosor,cosor2,cosor4,cosor6;

	if(c == NULL) return NULL_TM83_CONST_STRUCT;

	c->esq = c->f + c->f - c->f * c->f;

	n = c->f / (2.0 - c->f);

	c->r = c->a * (1.0 - n) * (1.0 - n*n)
				* (1.0 + 9.0 * n*n / 4.0 + 225.0 * n*n*n*n / 64.0);

	u2 = -3.0 * n / 2.0 + 9.0 * n*n*n / 16.0;
	u4 = 15.0 * n*n / 16.0 - 15.0 * n*n*n*n / 32.0;
	u6 = -35.0 * n*n*n / 48.0;
	u8 = 315.0 * n*n*n*n / 512.0;

/*
	Used in UTM_S program
	c->A = u2;
	c->B = u4;
	c->C = u6;
*/

	c->U0 = 2.0 * (u2 - 2.0 * u4 + 3.0 * u6 - 4.0 * u8);
	c->U2 = 8.0 * (u4 - 4.0 * u6 + 10.0 * u8);
	c->U4 = 32.0 * (u6 - 6.0 * u8);
	c->U6 = 128.0 * u8;

	v2 = 3.0 * n / 2.0 - 27.0 * n*n*n / 32.0;
	v4 = 21.0 * n*n / 16.0 - 55.0 * n*n*n*n / 32.0;
	v6 = 151.0 * n*n*n / 96.0;
	v8 = 1097.0 * n*n*n*n / 512.0;

	c->V0 = 2.0 * (v2 - 2.0 * v4 + 3.0 * v6 - 4.0 * v8);
	c->V2 = 8.0 * (v4 - 4.0 * v6 + 10.0 * v8);
	c->V4 = 32.0 * (v6 - 6.0 * v8);
	c->V6 = 128.0 * v8;

	if(c->or > 0.0000000001){
		cosor = cos(c->or);
		cosor2 = cosor * cosor;
		cosor4 = cosor2 * cosor2;
		cosor6 = cosor4 * cosor2;
/*		This is what is in the book.*/
		c->omo = c->or + sin(c->or) * cosor * (c->U0 + c->U2
					* cosor2 + c->U4 * cosor4 + c->U6 * cosor6);
/*
		Below is what is in UTMS.
		c->omo = c->or + c->A * sin(2.0 * c->or)
							+ c->B * sin(4.0 * c->or)
							+ c->C * sin(6.0 * c->or);
*/
		c->so = c->sf * c->omo * c->r;
	}
	else{
		c->omo = 0.0;
		c->so =0.0;
	}

   return CC_SUCCESS;
}

int Direct_AK1_83(pGRIDS83_CONSTANTS grids83_c,
							double *lat_in, double *lon_in,
	                  double *north, double *east,
   	               double *conv, double *kp)
{
double esq,e;
double b = 1.000296461404;
double c = 0.004426833926;
double d = 6386186.73253;
double f = -0.327012955438;
double g = 0.945019855334;
double i = 1.0015589176;
double lamo = 101.513839560 / RAD;
double lam,phi,l,q,j,k,u,v,sinphi;
double lat,lon;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	esq = 2.0 * GRS80_f - GRS80_f * GRS80_f;

	e = sqrt(esq);

	phi = lat / RAD;
	lam = lon / RAD;
	sinphi = sin(phi);

	l = (lam - lamo) * b;

	q = 0.5 * (log((1.0 + sinphi) / (1.0 - sinphi)) -
		e * log((1.0 + e * sinphi) / (1.0 - e * sinphi)));

	j = sinh(b * q + c);

	k = cosh(b * q + c);

	u = d * atan((j * g - f * sin(l)) / cos(l));

	v = d / 2.0 * log((k - f * j - g * sin(l))
						/ (k + f * j + g * sin(l)));

	*north = 0.8 * u + 0.6 * v - 5000000.0;

	*east = -0.6 * u + 0.8 * v + 5000000.0;

	*conv = atan((f - j * g * sin(l)) / (k * g * cos(l))) - atan(-3.0/4.0);
	*conv *= RAD;

    *kp = i * sqrt(1.0 - esq * sinphi * sinphi) * cos(u / d) /
    			(cos(phi) * cos(l));

    return CC_SUCCESS;
}

int Inverse_AK1_83(pGRIDS83_CONSTANTS grids83_c,
							double *lat, double *lon,
                     double *north_in, double *east_in,
                     double *conv, double *kp)
{
double f0 = 0.006686920927;
double f2 = 0.000052014584;
double f4 = 0.000000554430;
double f6 = 0.000000006820;
double esq;
double b = 1.000296461404;
double c = 0.004426833926;
double d = 6386186.73253;
double f = -0.327012955438;
double g = 0.945019855334;
double i = 1.0015589176;
double lamo = 101.513839560 / RAD;
double l,j,k,q,r,s,t,u,v,x,cosx,cosx2,cosx4,cosx6,sinphi;
double north,east;

	if(grids83_c == NULL) return NULL_GRIDS83_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

	esq = GRS80_f + GRS80_f - GRS80_f * GRS80_f;

	u = -0.6 * east + 0.8 * north + 7000000.0;
	v = 0.8 * east + 0.6 * north - 1000000.0;

	r = sinh(v / d);
	s = cosh(v / d);
	t = sin(u / d);

	q = (log((s - r * f + g * t) / (s + r * f - g * t)) / 2.0 - c) / b;

	x = 2.0 * atan((exp(q) - 1.0) / (exp(q) + 1.0));

	cosx = cos(x);
	cosx2 = cosx * cosx;
	cosx4 = cosx2 * cosx2;
	cosx6 = cosx4 * cosx2;

	*lat = x + sin(x) * cosx * (f0 + f2 * cosx2 + f4 * cosx4 + f6 * cosx6);

	*lon = lamo - atan((r * g + t * f) / cos(u / d)) / b;

	l = (*lon - lamo) * b;

	sinphi = sin(*lat);

	j = sinh(b * q + c);

	k = cosh(b * q + c);

    *conv = atan((f - j * g * sin(l)) / (k * g * cos(l))) - atan(-3.0/4.0);

    *kp = i * sqrt(1.0 - esq * sinphi * sinphi) * cos(u / d) /
				(cos(*lat) * cos(l));

	*conv *= RAD;
    *lon *= RAD;
	*lat *= RAD;

    return CC_SUCCESS;
}