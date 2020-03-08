/*
  File:   grids27.c
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
#include "grids27.h"
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

/*
	Need to document.
*/
DLLEXPORT int _stdcall geo27_to_sp27(int zone, int unit,
													double *lat, double *lon,
													double *north, double *east,
                              			double *conv, double *kp)
{
static int last_zone=0;
static int last_unit=0;
static int (*grids27_function)();
static double (*unit_function)();
static struct SP27_CONSTANTS sp27_c;
int rc;

   if(lat == NULL) return NULL_LAT_IN;
   if(lon == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	if(zone != last_zone)
   {
		rc = get_grids27_function(GEO_TO_SP,zone,&grids27_function);
      if(rc != CC_SUCCESS) return rc;

		rc = init_sp27_constants(zone, &sp27_c);
      if(rc != CC_SUCCESS) return rc;
      last_zone = zone;
   }
   if(unit != last_unit)
   {
      rc = get_cvt_function(US_FOOT,unit,&unit_function);
      if(rc != CC_SUCCESS) return rc;
      last_unit = unit;
   }

   if(*grids27_function == NULL) return NULL_GRIDS27_FUNCTION;
   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

	(*grids27_function)(&sp27_c,lat,lon,north,east,conv,kp);
   *north = (*unit_function)(*north);
   *east = (*unit_function)(*east);

   return CC_SUCCESS;
}

/*
	Need to document.
*/
DLLEXPORT int _stdcall sp27_to_geo27(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp)
{
static int last_zone=0;
static int last_unit=0;
static int (*grids27_function)();
static double (*unit_function)();
static struct SP27_CONSTANTS sp27_c;
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
		rc = get_grids27_function(SP_TO_GEO,zone,&grids27_function);
      if(rc != CC_SUCCESS) return rc;

		rc = init_sp27_constants(zone, &sp27_c);
      if(rc != CC_SUCCESS) return rc;
      last_zone = zone;
   }
   if(unit != last_unit)
   {
      rc = get_cvt_function(unit,US_FOOT,&unit_function);
      if(rc != CC_SUCCESS) return rc;
      last_unit = unit;
   }

   if(*grids27_function == NULL) return NULL_GRIDS27_FUNCTION;
   if(*unit_function == NULL) return NULL_UNIT_FUNCTION;

   north = (*unit_function)(*north_in);
   east = (*unit_function)(*east_in);
	(*grids27_function)(&sp27_c,lat,lon,&north,&east,conv,kp);

   return CC_SUCCESS;
}


DLLEXPORT int _stdcall describe_sp27_zone_constants(int zone, char *desc)
{
struct SP27_CONSTANTS sp27_c;
int rc;
char stmp[128];
char sign[2];
int deg,min;
double sec;

	rc = init_sp27_constants(zone, &sp27_c);

   if(rc != CC_SUCCESS) return rc;

  	sprintf(desc,"SPCS 1927, Zone %04d\n\n",zone);

   if(sp27_c.l1 != 0.0)
   {
   	strcat(desc,"Projection = Lambert\n");
      sprintf(stmp,"L1 = %.1lf\n",sp27_c.l1);strcat(desc,stmp);
      sprintf(stmp,"L2 = %.1lf\n",sp27_c.l2);strcat(desc,stmp);
      sprintf(stmp,"L3 = %.2lf\n",sp27_c.l3);strcat(desc,stmp);
      sprintf(stmp,"L4 = %.2lf\n",sp27_c.l4);strcat(desc,stmp);
      sprintf(stmp,"L5 = %.10lf\n",sp27_c.l5);strcat(desc,stmp);
      sprintf(stmp,"L6 = %.10lf\n",sp27_c.l6);strcat(desc,stmp);
      sprintf(stmp,"L7 = %.1lf\n",sp27_c.l7);strcat(desc,stmp);
      sprintf(stmp,"L8 = %.5lf\n",sp27_c.l8);strcat(desc,stmp);
      sprintf(stmp,"L9 = %.5lf\n",sp27_c.l9);strcat(desc,stmp);
      sprintf(stmp,"L10 = %.5lf\n",sp27_c.l10);strcat(desc,stmp);
      sprintf(stmp,"L11 = %.1lf\n",sp27_c.l11);strcat(desc,stmp);
   }
   else if(sp27_c.t1 != 0.0)
   {
   	strcat(desc,"Projection = Transverse Mercator\n");
      sprintf(stmp,"T1 = %.1lf\n",sp27_c.t1);strcat(desc,stmp);
      sprintf(stmp,"T2 = %.1lf\n",sp27_c.t2);strcat(desc,stmp);
      sprintf(stmp,"T3 = %.1lf\n",sp27_c.t3);strcat(desc,stmp);
      sprintf(stmp,"T4 = %.5lf\n",sp27_c.t4);strcat(desc,stmp);
      sprintf(stmp,"T5 = %.10lf\n",sp27_c.t5);strcat(desc,stmp);
      sprintf(stmp,"T6 = %.7lf\n",sp27_c.t6);strcat(desc,stmp);
   }
   else if(sp27_c.ak_c != 0.0)
   {
   	strcat(desc,"Projection = Alaska 2-9\n");
      sprintf(stmp,"c = %.1lf\n",sp27_c.ak_c);strcat(desc,stmp);
      sprintf(stmp,"cm = %.1lf\n",sp27_c.ak_cm);strcat(desc,stmp);
   }

   return CC_SUCCESS;
}


/*
	Need to document.
*/
int get_grids27_function(int opt, int zone, int *sp_function)
{
		switch(zone)
		{
			/*AK1*/	case 5001:

							if(opt == GEO_TO_SP) *sp_function = Direct_AK1_27;
							else if(opt == SP_TO_GEO) *sp_function = Inverse_AK1_27;
	                  else return BAD_GRIDS27_OPT;
							break;
			/*AK 2-9*/
						case 5002:case 5003:case 5004:case 5005:
						case 5006:case 5007:case 5008:case 5009:

							if(opt == GEO_TO_SP)	*sp_function = Direct_AK2thru9_27;
							else if(opt == SP_TO_GEO) *sp_function = Inverse_AK2thru9_27;
	                  else return BAD_GRIDS27_OPT;
							break;

			/*AL*/	case 101:case 102:
			/*AZ*/	case 201:case 202:case 203:
			/*DE*/	case 700:
			/*FL*/	case 901:case 902:
			/*GA*/	case 1001:case 1002:
			/*HI*/	case 5101:case 5102:case 5103:case 5104:case 5105:
			/*ID*/	case 1101:case 1102:case 1103:
			/*IL*/	case 1201:case 1202:
			/*IN*/	case 1301:case 1302:
			/*ME*/	case 1801:case 1802:
			/*MI*/	case 2101:case 2102:case 2103:
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

	         				if(opt == GEO_TO_SP) *sp_function = geo27_to_tm27;
   	                  else if (opt == SP_TO_GEO) *sp_function = tm27_to_geo27;
		                  else return BAD_GRIDS27_OPT;
								break;

			/*AK*/	case 5010:
			/*AR*/	case 301:case 302:
			/*CA*/	case 401:case 402:case 403:case 404:case 405:
						case 406:case 407:
			/*CO*/	case 501:case 502:case 503:
			/*CT*/	case 600:
			/*FL*/	case 903:
			/*IA*/	case 1401:case 1402:
			/*KS*/	case 1501:case 1502:
			/*KY*/	case 1601:case 1602:
			/*LA*/	case 1701:case 1702:case 1703:
			/*MD*/	case 1900:
			/*MA*/	case 2001:case 2002:
			/*MI*/	case 2111:case 2112:case 2113:
			/*MN*/	case 2201:case 2202:case 2203:
			/*MT*/	case 2501:case 2502:case 2503:
			/*NE*/	case 2601:case 2602:
			/*NY*/	case 3104:
			/*NC*/	case 3200:
			/*ND*/	case 3301:case 3302:
			/*OH*/	case 3401:case 3402:
			/*OK*/	case 3501:case 3502:
			/*OR*/	case 3601:case 3602:
			/*PA*/	case 3701:case 3702:
			/*SC*/	case 3901:case 3902:
			/*SD*/	case 4001:case 4002:
			/*TN*/	case 4100:
			/*TX*/	case 4201:case 4202:case 4203:case 4204:case 4205:
			/*UT*/	case 4301:case 4302:case 4303:
			/*VA*/	case 4501:case 4502:
			/*WA*/	case 4601:case 4602:
			/*WV*/	case 4701:case 4702:
			/*WI*/	case 4801:case 4802:case 4803:
	      /*PRVI*/	case 5201:case 5202:

							if(opt == GEO_TO_SP) *sp_function = geo27_to_lam27;
							else if (opt == SP_TO_GEO) *sp_function = lam27_to_geo27;
	                  else return BAD_GRIDS27_OPT;
							break;

						default:
                  	return BAD_SP27_ZONE;
		}

      return CC_SUCCESS;
}

/*
	This routine converts geographic coordinates in lat/lon
	on NAD 27 to transverse merctor coordinates on NAD 27.
	Lat & Lon should be input as decimal degrees, positive north & west.
	North and east are returned in meters.  Convergence is
	returned in decimal degrees.  The TM27_CONSTANTS structure
	should be initialized with init_tm_27().

    Formulas for the conversion come from:

    State Plane Coordinates By Automatic Data Processing
    Charles N. Claire
    U.S. Department of Commerce
    Environmental Science Services Administration
    Coast and Geodetic Survey
    Publication 62-4
    1968
*/
int geo27_to_tm27(pSP27_CONSTANTS c,
						double *lat_in, double *lon_in,
						double *north, double *east,
                  double *conv, double *kp)
{
double phi,cosphi,sinphi,s1,sm,sinphi2,cosphi2,sinmess,cosmess;
double phi1,phi2,phip,phipp,tmp,tmp2;
double lat,lon;

	if(c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	phi = lat / RAD;

	cosphi = cos(phi);
	sinphi = sin(phi);

	tmp = (c->t2 - lon * 3600.0) / 10000.0;

	s1 = 30.92241724 * cosphi / sqrt(1.0 - 0.0067686580 * sinphi * sinphi)
			* (c->t2 - lon * 3600.0 - 3.9174 * tmp * tmp * tmp);

	tmp = s1 / 100000.0;

	sm = s1 + 4.0831 * tmp * tmp * tmp;

	tmp = 3.28083333 * sm * c->t5 / 100000.0;

	*east = c->t1 + 3.28083333 * sm * c->t5 + tmp * tmp * tmp * c->t6;

	tmp = 1.0 - 0.0067686580 * sinphi * sinphi;

	phi1 = lat * 3600.0 + 25.52381 / pow(10,10) * sm * sm
			* tmp * tmp * tan(phi);

	tmp = 1.0 - 0.0067686580 * sin(phi1 / 3600.0 / RAD)
			* sin(phi1 / 3600.0 / RAD);

	phi2 = lat * 3600.0 + 25.52381 / pow(10,10) * sm * sm
			* tmp * tmp * tan(phi1 / 3600.0 / RAD);

	phip = floor(phi2 / 60.0);
	phipp = phi2 - phip * 60.0;

	cosphi2 = cos(phi2 / 3600.0 / RAD);
	sinphi2 = sin(phi2 / 3600.0 / RAD);

	*north = 101.2794065 * c->t5 * (60.0 * (phip - c->t3)
				+ phipp - c->t4 - (1052.893882 - (4.483344
				- 0.023520 * cosphi2 * cosphi2) * cosphi2 * cosphi2)
				* sinphi2 * cosphi2);

	tmp = c->t2 - lon * 3600.0;

	sinmess = sin((phi + phi2 / 3600.0 / RAD) / 2.0);
	cosmess = cos((phi + phi2 / 3600.0 / RAD) / 2.0);

	*conv = tmp * (sinmess + 1.9587 * tmp * tmp / pow(10,12)
				* sinmess * cosmess * cosmess);

	*conv /= 3600.0;

	tmp = (*east - c->t1) / pow(10,6);
	tmp2 = 1.0 + 0.0068147849 * cosphi * cosphi;

	*kp = c->t5 * (1.0 + tmp2 * tmp2 * tmp * tmp
					/ (881.749162 * c->t5 * c->t5));

	return CC_SUCCESS;
}

/*
	This routine converts transverse mercator coordinates on
	NAD 27 to geographic coordinates in lat/lon on NAD 27.
    North and east should be input in meters.  Lat and Lon
	are returned in decimal degrees, positive north and west.
	Convergence is returned in decimal degrees.  The
	TM27_CONSTANTS structure should be initialized with init_tm_27().

	Formulas for the conversion come from:

	State Plane Coordinates By Automatic Data Processing
    Charles N. Claire
	U.S. Department of Commerce
    Environmental Science Services Administration
    Coast and Geodetic Survey
    Publication 62-4
	1968
*/
int tm27_to_geo27(pSP27_CONSTANTS c,
						double *lat, double *lon,
                  double *north_in, double *east_in,
                  double *conv, double *kp)
{
double tmp,tmp2,sg1,sm,omp,ompp,om,phipp,phippp,cosom,sinom,phip;
double sinphip,s1,sa,sinphi,cosphi,dlam1,phi2p,dlama,phi1,phi2;
double sinmess,cosmess,phi;
double north,east;

	if(c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

	tmp = (east - c->t1) / 100000.0;

	sg1 = east - c->t1 - c->t6 * tmp * tmp * tmp;

	tmp = sg1 / 100000.0;

	sm = 0.3048006099 * (east - c->t1 - c->t6 * tmp * tmp * tmp) / c->t5;

	omp = c->t3;

	ompp = c->t4 + 0.009873675553 * north / c->t5;

    om = (omp / 60.0 + ompp / 3600.0) / RAD;

    cosom = cos(om);
	sinom = sin(om);

	phipp = c->t3;

	phippp = ompp + (1047.546710 + (6.192760 + 0.050912 * cosom * cosom)
    			* cosom * cosom) * sinom * cosom;

    phip = phipp / 60.0 + phippp / 3600.0;

	sinphip = sin(phip / RAD);
    tmp = 1.0 - 0.0067686580 * sinphip * sinphip;
	tmp2 = sm / 100000.0;

    phi2p = phippp - 25.52381 * tmp * tmp * tmp2 * tmp2
				* tan(phip / RAD);

	*lat = phipp / 60.0 + phi2p / 3600.0;

	phi = *lat / RAD;

    sinphi = sin(*lat / RAD);
	cosphi = cos(*lat / RAD);

	tmp = sm / 100000.0;

	sa = sm - 4.0831 * tmp * tmp * tmp;

	tmp = sa / 100000.0;

	s1 = sm - 4.0831 * tmp * tmp * tmp;

	tmp = 1.0 - 0.0067686580 * sinphi * sinphi;

	tmp = sqrt(tmp);

	dlam1 = s1 * tmp / (30.92241724 * cosphi);

	tmp = dlam1 / 10000.0;

	dlama = dlam1 + 3.9174 * tmp * tmp * tmp;
	tmp = dlama / 10000.0;

	*lon = c->t2 - dlam1 - 3.9174 * tmp * tmp * tmp;

	*lon /= 3600.0;

	tmp = (east - c->t1) / pow(10,6);
	tmp2 = 1.0 + 0.0068147849 * cosphi * cosphi;

	*kp = c->t5 * (1.0 + tmp2 * tmp2 * tmp * tmp
					/ (881.749162 * c->t5 * c->t5));

	tmp = 1.0 - 0.0067686580 * sinphi * sinphi;

	phi1 = *lat * 3600.0 + 25.52381 / pow(10,10) * sm * sm
			* tmp * tmp * tan(phi);

	tmp = 1.0 - 0.0067686580 * sin(phi1 / 3600.0 / RAD)
			* sin(phi1 / 3600.0 / RAD);

	phi2 = *lat * 3600.0 + 25.52381 / pow(10,10) * sm * sm
			* tmp * tmp * tan(phi1 / 3600.0 / RAD);

	sinmess = sin((phi + phi2 / 3600.0 / RAD) / 2.0);
	cosmess = cos((phi + phi2 / 3600.0 / RAD) / 2.0);

	tmp = c->t2 - *lon * 3600.0;

	*conv = tmp * (sinmess + 1.9587 * tmp * tmp / pow(10,12)
				* sinmess * cosmess * cosmess);

	*conv /= 3600.0;

	return CC_SUCCESS;
}

/*
	This routine converts geographic coordinates in lat/lon
	on NAD 27 to lambert conformal conic coordinates on NAD 27.
	Lat & Lon should be input as decimal degrees, positive north & west.
	North and east are returned in meters.  Convergence is
	returned in decimal degrees.  The LAM27_CONSTANTS structure
	should be initialized with init_lamb_27().

    Formulas for the conversion come from:

    State Plane Coordinates By Automatic Data Processing
    Charles N. Claire
    U.S. Department of Commerce
    Environmental Science Services Administration
    Coast and Geodetic Survey
    Publication 62-4
    1968
*/
int geo27_to_lam27(pSP27_CONSTANTS c,
							double *lat_in, double *lon_in,
                     double *north, double *east,
                     double *conv, double *kp)
{
double phi,phi1,phi2,s,r,sinphi,cosphi,sintheta,dtmp;
double lat,lon;

	if(c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	phi = lat / RAD;

	sinphi = sin(phi);
	cosphi = cos(phi);

	phi1 = floor(lat * 60.0);
	phi2 = (lat * 60.0 - phi1) * 60.0;

	s = 101.2794065 * (60.0 * (c->l7 - phi1) + c->l8 - phi2
			+ (1052.893882 - (4.483344 - 0.023520 * cosphi * cosphi)
			* cosphi * cosphi) * sinphi * cosphi);

	dtmp = s / pow(10,8);

	r = c->l3 + s * c->l5 * (1.0 + dtmp * dtmp * (c->l9 - dtmp * c->l10
			+ dtmp * dtmp * c->l11));

	*conv = c->l6 * (c->l2 - lon * 3600.0) / 3600.0;
	*conv /= RAD;

	*east = c->l1 + r * sin(*conv);
	sintheta = sin(*conv / 2.0);
	*north = c->l4 - r + 2.0 * r * sintheta * sintheta;

    *conv *= RAD;

	*kp = c->l6 * r * sqrt(1.0 - 0.0067686580 * sinphi * sinphi)
    		/ (20925832.16 * cosphi);

    return CC_SUCCESS;
}

/*
	This routine converts lambert conformal conic coordinates on
    NAD 27 to geographic coordinates in lat/lon on NAD 27.
    North and east should be input in meters.  Lat and Lon
	are returned in decimal degrees, positive north and west.
	Convergence is returned in decimal degrees.  The
	LAM83_CONSTANTS structure should be initialized with init_lam_27().

    Formulas for the conversion come from:

    State Plane Coordinates By Automatic Data Processing
    Charles N. Claire
    U.S. Department of Commerce
    Environmental Science Services Administration
    Coast and Geodetic Survey
    Publication 62-4
    1968
*/
int lam27_to_geo27(pSP27_CONSTANTS c,
							double *lat, double *lon,
                     double *north_in, double *east_in,
                     double *conv, double *kp)
{
double r,s1,s2,s3,s,om1,om2,om,phi1,phi2,cosom,sinphi,cosphi;
double t,t2,t3,t4;
double north,east;

	if(c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

	*conv = atan((east - c->l1) / (c->l4 - north));

    *lon = c->l2 - (*conv * RAD * 3600.0) / c->l6;

    *lon /= 3600.0;

    r = (c->l4 - north) / cos(*conv);

    t = sin(*conv / 2.0);

    s1 = (c->l4 - c->l3 - north + 2.0 * r * t * t) / c->l5;

    *conv *= RAD;

    t = s1 / pow(10,8);
    t2 = t * t;
    t3 = t2 * t;
    t4 = t3 * t;

    s2 = s1 / (1.0 + t2 * c->l9 - t3 * c->l10 + t4 * c->l11);

    t = s2 / pow(10,8);
    t2 = t * t;
    t3 = t2 * t;
    t4 = t3 * t;

    s3 = s1 / (1.0 + t2 * c->l9 - t3 * c->l10 + t4 * c->l11);

    t = s3 / pow(10,8);
    t2 = t * t;
    t3 = t2 * t;
    t4 = t3 * t;

    s = s1 / (1.0 + t2 * c->l9 - t3 * c->l10 + t4 * c->l11);

    om1 = c->l7 - 600.0;

    om2 = (36000.0 + c->l8 - 0.009873675553 * s);

	om = (om1 / 60.0 + om2 / 3600.0) / RAD;
	cosom = cos(om);

	phi1 = c->l7 - 600.0;

	phi2 = om2 + (1047.546710 + (6.192760 + 0.050912 * cosom * cosom)
				* cosom * cosom) * sin(om) * cosom;

	*lat = phi1 / 60.0 + phi2 / 3600.0;

	sinphi = sin(*lat / RAD);
	cosphi = cos(*lat / RAD);

	*kp = c->l6 * r * sqrt(1.0 - 0.0067686580 * sinphi * sinphi)
			/ (20925832.16 * cosphi);

	return CC_SUCCESS;
}

int Direct_AK1_27(pSP27_CONSTANTS sp27_c,double *lat_in, double *lon_in,
						double *north, double *east, double *conv, double *kp)
{
double b = 1.00029977273;
double c = 0.00447599131;
double d = 6386352.67013;
double f = 0.327015517176;
double g = 0.945018968871;
double h = 0.3460412203;
double i = 1.0015773595;
double e = 0.082271854223003;
double esq = 0.006768657997291;
double lamo = 365450.51319 / 3600.0 / RAD;
double l,p,q,u,v,mu,phi,lam,tmp,cosll,sinll,sinphi;
double lat,lon;

   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

	phi = lat / RAD;
	lam = lon / RAD;
	sinphi = sin(phi);

	mu = log(tan(M_PI_4 + phi / 2.0)) -
		e * log((1.0 + e * sinphi) / (1.0 - e * sinphi)) / 2.0;

	tmp = b * mu + c;

	p = (exp(tmp) - exp(-tmp)) / 2.0;

	q = (exp(tmp) + exp(-tmp)) / 2.0;

	l = b * (lam - lamo);
	cosll = cos(l);
	sinll = sin(l);

	u = d * atan((g * p + f * sinll) / cosll);

	v = d / 2.0 * log((q + f * p - g * sinll)
						/ (q - f * p + g * sinll));

	*north = 0.8 * u + 0.6 * v - 5000000.0;

	*north = cvt(METER,US_FOOT,*north);

	*east = -0.6 * u + 0.8 * v + 5000000.0;

	*east = cvt(METER,US_FOOT,*east);

	*conv = atan((3.0 * q * cosll - 4.0 * p * sinll - 4.0 * h)
					/ (4.0 * q * cosll + 3.0 * p * sinll + 3.0 * h)) * RAD;

	*kp = i * sqrt(1.0 - esq * sinphi * sinphi) * cos(u / d) /
				(cos(phi) * cosll);

	return CC_SUCCESS;
}

int Inverse_AK1_27(pSP27_CONSTANTS sp27_c, double *lat, double *lon,
							double *north_in, double *east_in, double *conv, double *kp)
{
double b = 1.00029977273;
double c = 0.00447599131;
double d = 6386352.67013;
double f = 0.327015517176;
double g = 0.945018968871;
double lamo = 365450.51319 / 3600.0;
double vd,r,s,mu,gsinud,chi,coschi,coschi2,coschi4,coschi6,sinchi,phir;
double sinud,cosud,u,v,phipp;
double north,east;

   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

	u = -0.182880365761 * east + 0.243840487681 * north + 7000000.0;
	v = 0.243840487681 * east + 0.182880365761 * north - 1000000.0;

	vd = v / d;

	r = (exp(vd) - exp(-vd)) / 2.0;
	s = (exp(vd) + exp(-vd)) / 2.0;

	gsinud = g * sin(u/d);

	mu = log((s + f * r + gsinud) / (s - f * r - gsinud)) / (2.0 * b)
			- (c / b);

	chi = 2.0 * atan(exp(mu)) - M_PI_2;

	sinchi = sin(chi);
	coschi = cos(chi);
	coschi2 = coschi * coschi;
	coschi4 = coschi2 * coschi2;
	coschi6 = coschi4 * coschi2;

	phir = chi + (0.006761032571 + 0.000053172205 * coschi2
					+ 0.000000573027 * coschi4
					+ 0.000000007128 * coschi6) * sinchi * coschi;

	phipp = 206264.80624 * phir;
	*lat = phipp / 3600.0;

	sinud = sin(u/d);
	cosud = cos(u/d);

	*lon = lamo + atan((f * sinud - g * r) / cosud) / b * RAD;

	*conv = 0.0;
	*kp = 0.0;

	return CC_SUCCESS;
}

int Direct_AK2thru9_27(pSP27_CONSTANTS sp27_c, double *lat_in, double *lon_in,
								double *north, double *east, double *conv, double *kp)
{
double phi,c,cm,lampp,phipp,cosphi,cosphi2,cosphi4,cosphi6,tmp,tmp2,tmp4;
double x1,x2,x3,x4,x5;
double lat,lon;

	if(sp27_c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat_in == NULL) return NULL_LAT_IN;
   if(lon_in == NULL) return NULL_LON_IN;
   if(north == NULL) return NULL_NORTH_OUT;
   if(east == NULL) return NULL_EAST_OUT;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	lat = *lat_in;
   lon = *lon_in;

    c = sp27_c->ak_c;
    cm = sp27_c->ak_cm;

	phi = lat / RAD;
    lampp = lon * 3600.0;
    phipp = lat * 3600.0;

    cosphi = cos(phi);
    cosphi2 = cosphi * cosphi;
    cosphi4 = cosphi2 * cosphi2;
    cosphi6 = cosphi4 * cosphi2;

    tmp = (cm - lampp) / 10000.0;
    tmp2 = tmp * tmp;
    tmp4 = tmp2 * tmp2;

    x1 = 1017862.150 * cosphi / sqrt(1.0 + 0.0068147849 * cosphi2) * tmp;
    x2 = 3.91740509 / 10000.0 * tmp2;
    x3 = 1.0 - 2.0 * cosphi2 - 0.681478 / 100.0 * cosphi4;
    x4 = 4.60382 / pow(10,8) * tmp4;
    x5 = 1.0 - 20.0 * cosphi2 + 23.6047 * cosphi4 + 0.4907 * cosphi6;

    *east = c + x1 * (1.0 - x2 * x3 + x4 * x5);

    x1 = (1052.893943 - 4.483386 * cosphi2 + 2.3559 / 100.0
                * cosphi4) * sqrt(1.0 - cosphi2) * cosphi;
    x2 = 24673.67480 * sqrt(1.0 - cosphi2) * cosphi
                / sqrt(1.0 + 0.0068147849 * cosphi2) * tmp2;
    x3 = -1.0 + 6.0 * cosphi2 + 6.133306 / 100.0 * cosphi4
			+ 1.8577 / 10000.0 * cosphi6;
    x4 = 1.0 - 60.0 * cosphi2 + 117.75 * cosphi4 + 4.089 * cosphi6;

	*north = 101.269278503 * (phipp - 193900.054420 - x1) + x2
                * (1.0 + 1.95870/10000.0 * tmp2 * x3
				+ 1.5346 / pow(10,8) * tmp4 * x4);

    *conv = sqrt(1.0 - cosphi2) * tmp * (10000.0
    			+ 7.83481 * tmp2 * (cosphi2 + 2.044 / 100.0 * cosphi4
                + 0.9 / 10000.0 * cosphi6) + 0.3683 / 100.0 * tmp4
                * (3.0 * cosphi4 - cosphi2));

    *conv /= 3600.0;

    tmp = 1.0 + 0.0068147849 * cosphi2;
    tmp2 = (*east - c) / 1000000.0;

    *kp = 0.9999 * (1.0 + tmp * tmp / 881.57282 * tmp2 * tmp2);

	return CC_SUCCESS;
}

int Inverse_AK2thru9_27(pSP27_CONSTANTS sp27_c, double *lat, double *lon,
								double *north_in, double *east_in, double *conv, double *kp)
{
double phi,c,cm,cosomp,cosomp2,cosomp4;
double tmp,tmp2,tmp4,tmpa,x1,x2,x3;
double cosphip,cosphip2,cosphip4,omppp,omp,phippp,phip;
double cosphi,cosphi2,cosphi4,cosphi6;
double north,east;

	if(sp27_c == NULL) return NULL_SP27_CONST_STRUCT;
   if(lat == NULL) return NULL_LAT_OUT;
   if(lon == NULL) return NULL_LON_OUT;
   if(north_in == NULL) return NULL_NORTH_IN;
   if(east_in == NULL) return NULL_EAST_IN;
   if(conv == NULL) return NULL_CONV;
   if(kp == NULL) return NULL_KP;

	north = *north_in;
   east = *east_in;

    c = sp27_c->ak_c;
    cm = sp27_c->ak_cm;

    omppp = 193900.054420 + 0.00987466302498 * north;

    omp = omppp / 3600.0 / RAD;

    cosomp = cos(omp);
    cosomp2 = cosomp * cosomp;
    cosomp4 = cosomp2 * cosomp2;

    phippp = omppp + (1047.546691 + 6.193011 * cosomp2
    			+ 5.0699 / 100.0 * cosomp4)
                * sqrt(1.0 - cosomp2) * cosomp;

    phip = phippp / 3600.0 / RAD;

    cosphip = cos(phip);
    cosphip2 = cosphip * cosphip;
    cosphip4 = cosphip2 * cosphip2;

    tmp = (east - c) / 1000000.0;
    tmp2 = tmp * tmp;
    tmp4 = tmp2 * tmp2;

    tmpa = 1.0 + 0.0068147849 * cosphip2;

    x1 = 233.9736450 * tmp2 * tmpa * tmpa * sqrt(1.0 / cosphip2 - 1.0);
    x2 = 1.89056040 / 10000.0 * tmp2 * (1.9591113 + 3.0 / cosphip2
			+ 8.1359 / 100.0 * cosphip2 + 2.79 / 10000.0 * cosphip4);
    x3 = 15.5 + 45.0 / cosphip4 - 0.307 / cosphip2 + 1.53 * cosphip2;

    *lat = phippp -  x1 * (1.0 -  x2 + 1.42969 / pow(10,8)
            * tmp4 * tmpa * x3);

    *lat /= 3600.0;

    x1 = 3.78112080 / 10000.0 * tmpa * tmp2;
    x2 = -1.0 + 2.0 / cosphip2 + 0.0068147849 * cosphip2;
    x3 = 1.054 + 24.0 / cosphip4 - 20.0 / cosphip2 - 1.36 / 100.0 * cosphip2;

    *lon = cm - 9824.513072 * sqrt(tmpa) / cosphip * tmp * (1.0 - x1 * x2
            + 4.2890624 / pow(10,8) * tmpa * tmpa * tmp4 * x3);

    *lon /= 3600.0;

    phi = *lat / RAD;
    cosphi = cos(phi);
    cosphi2 = cosphi * cosphi;
    cosphi4 = cosphi2 * cosphi2;
    cosphi6 = cosphi4 * cosphi2;

    tmp = (cm - *lon * 3600.0) / 10000.0;
    tmp2 = tmp * tmp;
    tmp4 = tmp2 * tmp2;

    *conv = sqrt(1.0 - cosphi2) * tmp * (10000.0
    			+ 7.83481 * tmp2 * (cosphi2 + 2.044 / 100.0 * cosphi4
                + 0.9 / 10000.0 * cosphi6) + 0.3683 / 100.0 * tmp4
                * (3.0 * cosphi4 - cosphi2));

    *conv /= 3600.0;

    tmp = 1.0 + 0.0068147849 * cosphi2;
    tmp2 = (east - c) / 1000000.0;

    *kp = 0.9999 * (1.0 + tmp * tmp / 881.57282 * tmp2 * tmp2);


	return CC_SUCCESS;
}

/*
	This routine initializes the constants state plane
   constants.  Zone refers to the state plane zone id.
*/
int init_sp27_constants(int zone, struct SP27_CONSTANTS *c)
{
	if(c == NULL) return NULL_SP27_CONST_STRUCT;

	c->l1 = 0;
	c->l2 = 0;
	c->l3 = 0;
	c->l4 = 0;
	c->l5 = 0;
	c->l6 = 0;
	c->l7 = 0;
	c->l8 = 0;
	c->l9 = 0;
	c->l10 = 0;
	c->l11 = 0;

	c->t1 = 0;
	c->t2 = 0;
	c->t3 = 0;
	c->t4 = 0;
	c->t5 = 0;
	c->t6 = 0;

	c->ak_c = 0;
	c->ak_cm = 0;

    switch(zone)
	{
	     case 5010:     /* AK 10 */
                    c->l1 = 3000000.0;
                    c->l2 = 633600.0;
                    c->l3 = 15893950.36;
                    c->l4 = 16564628.77;
                    c->l5 = 0.9998480641;
                    c->l6 = 0.7969223940;
                    c->l7 = 3161.0;
                    c->l8 = 47.87068;
                    c->l9 = 3.79919;
						  c->l10 = 5.91550;
                    c->l11 = 44.0;
                    break;

    	 case 301:      /* AR North */
                    c->l1 = 2000000.0;
                    c->l2 = 331200.0;
                    c->l3 = 29277593.61;
                    c->l4 = 29732882.87;
                    c->l5 = 0.9999359370;
                    c->l6 = 0.5818991407;
                    c->l7 = 2126.0;
                    c->l8 = 46.35656;
                    c->l9 = 3.81452;
                    c->l10 = 3.26432;
                    c->l11 = 0.0;
                    break;

   		  case 302:      /* AR South */
                    c->l1 = 2000000.0;
                    c->l2 = 331200.0;
                    c->l3 = 31014039.23;
                    c->l4 = 31511724.2;
                    c->l5 = 0.9999184698;
                    c->l6 = 0.5596906871;
                    c->l7 = 2033.0;
                    c->l8 = 56.94711;
                    c->l9 = 3.81550;
                    c->l10 = 3.08256;
                    c->l11 = 0.0;
                    break;

	     case 401:      /* CA 1 */
                    c->l1 = 2000000.0;
                    c->l2 = 439200.0;
						  c->l3 = 24245358.05;
                    c->l4 = 24792436.23;
                    c->l5 = 0.9998946358;
						  c->l6 = 0.6538843192;
                    c->l7 = 2441.0;
                    c->l8 = 26.75847;
                    c->l9 = 3.80992;
                    c->l10 = 3.93575;
                    c->l11 = 0.0;
                    break;

    	 case 402:      /* CA 2 */
                    c->l1 = 2000000.0;
                    c->l2 = 439200.0;
                    c->l3 = 25795850.31;
                    c->l4 = 26312257.65;
                    c->l5 = 0.9999146793;
                    c->l6 = 0.6304679732;
                    c->l7 = 2336.0;
                    c->l8 = 30.81964;
                    c->l9 = 3.81147;
                    c->l10 = 3.70114;
                    c->l11 = 0.0;
                    break;

	     case 403:      /* CA 3 */
                    c->l1 = 2000000.0;
                    c->l2 = 433800.0;
                    c->l3 = 27057475.85;
                    c->l4 = 27512992.04;
                    c->l5 = 0.9999291792;
                    c->l6 = 0.6122320427;
                    c->l7 = 2256.0;
                    c->l8 = 35.52018;
                    c->l9 = 3.81265;
						  c->l10 = 3.52998;
                    c->l11 = 0.0;
                    break;

    	 case 404:      /* CA 4 */
                    c->l1 = 2000000.0;
                    c->l2 = 428400.0;
                    c->l3 = 28182405.33;
                    c->l4 = 28652931.96;
                    c->l5 = 0.9999407628;
                    c->l6 = 0.5965871443;
                    c->l7 = 2189.0;
                    c->l8 = 10.35494;
                    c->l9 = 3.81362;
                    c->l10 = 3.39020;
                    c->l11 = 0.0;
                    break;

	     case 405 :     /* CA 5 */
                    c->l1 = 2000000.0;
                    c->l2 = 424800.0;
                    c->l3 = 30194145.54;
                    c->l4 = 30649424.27;
                    c->l5 = 0.9999221277;
                    c->l6 = 0.5700119219;
                    c->l7 = 2076.0;
                    c->l8 = 52.10305;
                    c->l9 = 3.81523;
                    c->l10 = 3.16593;
                    c->l11 = 0.0;
                    break;

    	 case 406:      /* CA 6 */
                    c->l1 = 2000000.0;
                    c->l2 = 418500.0;
						  c->l3 = 31846570.92;
                    c->l4 = 32271267.72;
                    c->l5 = 0.9999541438;
						  c->l6 = 0.5495175982;
                    c->l7 = 1992.0;
                    c->l8 = 0.16335;
                    c->l9 = 3.81642;
                    c->l10 = 3.00292;
                    c->l11 = 0.0;
                    break;

	     case 407:      /* CA 7 */
                    c->l1 = 4186692.58;
                    c->l2 = 426000.0;
                    c->l3 = 30891382.1;
                    c->l4 = 35055396.31;
                    c->l5 = 0.9999885350;
                    c->l6 = 0.5612432071;
                    c->l7 = 2040.0;
                    c->l8 = 22.88096;
                    c->l9 = 3.81572;
                    c->l10 = 3.09520;
                    c->l11 = 0.0;
                    break;

    	 case 501:      /* CO North */
                    c->l1 = 2000000.0;
                    c->l2 = 379800.0;
                    c->l3 = 24751897.68;
                    c->l4 = 25086068.20;
                    c->l5 = 0.9999568475;
                    c->l6 = 0.6461334829;
                    c->l7 = 2406.0;
                    c->l8 = 24.62308;
                    c->l9 = 3.81044;
						  c->l10 = 3.85610;
                    c->l11 = 0.0;
                    break;

	     case 502:      /* CO Central */
                    c->l1 = 2000000.0;
                    c->l2 = 379800.0;
                    c->l3 = 25781376.91;
                    c->l4 = 26243052.74;
                    c->l5 = 0.9999359117;
                    c->l6 = 0.6306895773;
                    c->l7 = 2337.0;
                    c->l8 = 29.65162;
                    c->l9 = 3.81146;
                    c->l10 = 3.70326;
                    c->l11 = 0.0;
                    break;

    	 case 503:      /* CO South */
                    c->l1 = 2000000.0;
                    c->l2 = 379800.0;
                    c->l3 = 26977133.89;
                    c->l4 = 27402231.82;
                    c->l5 = 0.9999453995;
                    c->l6 = 0.6133780528;
                    c->l7 = 2261.0;
                    c->l8 = 34.26662;
                    c->l9 = 3.81257;
                    c->l10 = 3.54046;
                    c->l11 = 0.0;
                    break;

	     case 600:      /* CT */
                    c->l1 = 600000.0;
                    c->l2 = 261900.0;
						  c->l3 = 23659233.56;
                    c->l4 = 23914389.02;
                    c->l5 = 0.9999831405;
						  c->l6 = 0.6630594147;
                    c->l7 = 2483.0;
                    c->l8 = 19.67980;
                    c->l9 = 3.80929;
                    c->l10 = 4.03278;
                    c->l11 = 0.0;
                    break;

    	 case 903:      /* FL North */
                    c->l1 = 2000000.0;
                    c->l2 = 304200.0;
                    c->l3 = 36030443.05;
                    c->l4 = 36454924.53;
                    c->l5 = 0.9999484343;
                    c->l6 = 0.5025259000;
                    c->l7 = 1802.0;
                    c->l8 = 26.11701;
                    c->l9 = 3.81898;
                    c->l10 = 2.65643;
                    c->l11 = 0.0;
                    break;

	     case 1401:     /* IA North */
                    c->l1 = 2000000.0;
                    c->l2 = 336600.0;
                    c->l3 = 22736950.34;
                    c->l4 = 23162461.59;
                    c->l5 = 0.9999453686;
                    c->l6 = 0.6777445518;
                    c->l7 = 2551.0;
                    c->l8 = 20.02265;
                    c->l9 = 3.80827;
						  c->l10 = 4.19479;
                    c->l11 = 0.0;
                    break;

          case 1402:     /* IA South */
                    c->l1 = 2000000.0;
                    c->l2 = 336600.0;
                    c->l3 = 23936585.11;
                    c->l4 = 24374096.67;
                    c->l5 = 0.9999483705;
                    c->l6 = 0.6587010213;
                    c->l7 = 2463.0;
                    c->l8 = 22.59905;
                    c->l9 = 3.80959;
                    c->l10 = 3.98630;
                    c->l11 = 0.0;
                    break;

    	 case 1501:     /* KS North */
                    c->l1 = 2000000.0;
                    c->l2 = 352800.0;
                    c->l3 = 25644959.12;
                    c->l4 = 25979068.57;
                    c->l5 = 0.9999568556;
                    c->l6 = 0.6327148646;
                    c->l7 = 2346.0;
                    c->l8 = 27.97215;
                    c->l9 = 3.81133;
                    c->l10 = 3.72376;
                    c->l11 = 0.0;
                    break;

	     case 1502:     /* KS South */
                    c->l1 = 2000000.0;
                    c->l2 = 354600.0;
						  c->l3 = 26896024.48;
                    c->l4 = 27351521.50;
                    c->l5 = 0.9999359200;
						  c->l6 = 0.6145281068;
                    c->l7 = 2266.0;
                    c->l8 = 34.41020;
                    c->l9 = 3.81250;
                    c->l10 = 3.55102;
                    c->l11 = 0.0;
                    break;

    	 case 1601:     /* KY North */
                    c->l1 = 2000000.0;
                    c->l2 = 303300.0;
                    c->l3 = 26371820.68;
                    c->l4 = 26724051.82;
                    c->l5 = 0.9999620817;
                    c->l6 = 0.6220672671;
                    c->l7 = 2299.0;
                    c->l8 = 30.63364;
                    c->l9 = 3.81202;
                    c->l10 = 3.62113;
                    c->l11 = 0.0;
                    break;

	     case 1602:     /* KY South */
                    c->l1 = 2000000.0;
                    c->l2 = 308700.0;
                    c->l3 = 27467860.75;
                    c->l4 = 27832235.64;
                    c->l5 = 0.9999453808;
                    c->l6 = 0.6064623718;
                    c->l7 = 2231.0;
                    c->l8 = 36.57874;
                    c->l9 = 3.81301;
						  c->l10 = 3.47771;
                    c->l11 = 0.0;
                    break;

    	 case 1701:     /* LA North */
                    c->l1 = 2000000.0;
                    c->l2 = 333000.0;
                    c->l3 = 33624568.36;
                    c->l4 = 34079629.33;
                    c->l5 = 0.9999147417;
                    c->l6 = 0.5287006734;
                    c->l7 = 1907.0;
                    c->l8 = 12.68515;
                    c->l9 = 3.81758;
                    c->l10 = 2.84511;
                    c->l11 = 0.0;
                    break;

	     case 1702:     /* LA South */
                    c->l1 = 2000000.0;
                    c->l2 = 328800.0;
                    c->l3 = 36271389.35;
                    c->l4 = 36756553.45;
                    c->l5 = 0.9999257458;
                    c->l6 = 0.5000126971;
                    c->l7 = 1792.0;
                    c->l8 = 28.55026;
                    c->l9 = 3.81911;
                    c->l10 = 2.63885;
                    c->l11 = 0.0;
                    break;

    	 case 1703:     /* LA Offshore */
                    c->l1 = 2000000.0;
                    c->l2 = 328800.0;
						  c->l3 = 41091749.54;
                    c->l4 = 41576762.39;
                    c->l5 = 0.9998947956;
						  c->l6 = 0.4540068519;
                    c->l7 = 1612.0;
                    c->l8 = 59.30342;
                    c->l9 = 3.82138;
                    c->l10 = 2.27436;
                    c->l11 = 25.0;
                    break;

	     case 1900:     /* MD */
                    c->l1 = 800000.0;
                    c->l2 = 277200.0;
                    c->l3 = 25989474.99;
                    c->l4 = 26369112.76;
                    c->l5 = 0.9999498485;
                    c->l6 = 0.6276341196;
                    c->l7 = 2323.0;
                    c->l8 = 59.69369;
                    c->l9 = 3.81166;
                    c->l10 = 3.67392;
                    c->l11 = 0.0;
                    break;

    	 case 2001:     /* MA Mainland */
                    c->l1 = 600000.0;
                    c->l2 = 257400.0;
                    c->l3 = 23111975.14;
                    c->l4 = 23549477.32;
                    c->l5 = 0.9999645506;
                    c->l6 = 0.6717286561;
                    c->l7 = 2523.0;
                    c->l8 = 19.53138;
                    c->l9 = 3.80870;
						  c->l10 = 4.12738;
                    c->l11 = 0.0;
                    break;

	     case 2002:     /* MA Island */
                    c->l1 = 200000.0;
                    c->l2 = 253800.0;
                    c->l3 = 23784678.44;
                    c->l4 = 23924398.02;
                    c->l5 = 0.9999984844;
                    c->l6 = 0.6610953994;
                    c->l7 = 2474.0;
                    c->l8 = 19.47463;
                    c->l9 = 3.80943;
                    c->l10 = 4.01174;
                    c->l11 = 0.0;
                    break;

    	 case 2111:     /* MI North */
                    c->l1 = 2000000.0;
                    c->l2 = 313200.0;
                    c->l3 = 20041716.18;
                    c->l4 = 20589420.09;
                    c->l5 = 0.9999410344;
                    c->l6 = 0.7227899381;
                    c->l7 = 2768.0;
                    c->l8 = 22.25085;
                    c->l9 = 3.80501;
                    c->l10 = 4.68430;
                    c->l11 = 36.0;
                    break;

	     case 2112:     /* MI Central */
                    c->l1 = 2000000.0;
                    c->l2 = 303600.0;
						  c->l3 = 21001715.22;
                    c->l4 = 21594768.40;
                    c->l5 = 0.9999509058;
						  c->l6 = 0.7064074100;
                    c->l7 = 2687.0;
                    c->l8 = 50.76661;
                    c->l9 = 3.80622;
                    c->l10 = 4.46875;
                    c->l11 = 35.0;
                    break;

    	 case 2113:     /* MI South */
                    c->l1 = 2000000.0;
                    c->l2 = 303600.0;
                    c->l3 = 22564848.51;
                    c->l4 = 23069597.22;
                    c->l5 = 0.9999450783;
                    c->l6 = 0.6805292633;
                    c->l7 = 2564.0;
                    c->l8 = 22.23938;
                    c->l9 = 3.80808;
                    c->l10 = 4.15706;
                    c->l11 = 33.0;
                    break;

	     case 2201:     /* MN North */
                    c->l1 = 2000000.0;
                    c->l2 = 335160.0;
                    c->l3 = 18984319.62;
                    c->l4 = 19471398.75;
                    c->l5 = 0.9999028166;
                    c->l6 = 0.7412196637;
                    c->l7 = 2861.0;
                    c->l8 = 24.63011;
                    c->l9 = 3.80362;
						  c->l10 = 5.01609;
                    c->l11 = 0.0;
                    break;

    	 case 2202:     /* MN Central */
                    c->l1 = 2000000.0;
                    c->l2 = 339300.0;
                    c->l3 = 20006679.72;
                    c->l4 = 20493457.15;
                    c->l5 = 0.9999220223;
                    c->l6 = 0.7233880702;
                    c->l7 = 2771.0;
                    c->l8 = 20.89747;
                    c->l9 = 3.80497;
                    c->l10 = 4.76197;
                    c->l11 = 0.0;
                    break;

	     case 2203:     /* MN South */
                    c->l1 = 2000000.0;
                    c->l2 = 338400.0;
                    c->l3 = 21327006.06;
                    c->l4 = 21874349.14;
                    c->l5 = 0.9999220448;
                    c->l6 = 0.7009277824;
                    c->l7 = 2661.0;
                    c->l8 = 20.12517;
                    c->l9 = 3.80662;
                    c->l10 = 4.46959;
                    c->l11 = 0.0;
                    break;

    	 case 2501:     /* MT North */
                    c->l1 = 2000000.0;
                    c->l2 = 394200.0;
						  c->l3 = 18689498.40;
                    c->l4 = 19157874.26;
                    c->l5 = 0.9999714855;
						  c->l6 = 0.7464518080;
                    c->l7 = 2888.0;
                    c->l8 = 20.21285;
                    c->l9 = 3.80322;
                    c->l10 = 5.09490;
                    c->l11 = 0.0;
                    break;

	     case 2502:     /* MT Central */
                    c->l1 = 2000000.0;
                    c->l2 = 394200.0;
                    c->l3 = 19432939.76;
                    c->l4 = 19919806.36;
                    c->l5 = 0.9999220151;
                    c->l6 = 0.7333538278;
                    c->l7 = 2821.0;
                    c->l8 = 21.96779;
                    c->l9 = 3.80422;
                    c->l10 = 4.90135;
                    c->l11 = 0.0;
                    break;

    	 case 2503:     /* MT South */
                    c->l1 = 2000000.0;
                    c->l2 = 394200.0;
                    c->l3 = 20500650.51;
                    c->l4 = 21096820.93;
                    c->l5 = 0.9999107701;
                    c->l6 = 0.7149012442;
                    c->l7 = 2729.0;
                    c->l8 = 21.15820;
                    c->l9 = 3.80560;
						  c->l10 = 4.64814;
                    c->l11 = 0.0;
                    break;

	     case 2601:     /* NE North */
                    c->l1 = 2000000.0;
                    c->l2 = 360000.0;
                    c->l3 = 23004346.29;
                    c->l4 = 23368977.46;
                    c->l5 = 0.9999645501;
                    c->l6 = 0.6734507906;
                    c->l7 = 2531.0;
                    c->l8 = 19.30504;
                    c->l9 = 3.80858;
                    c->l10 = 4.14653;
                    c->l11 = 0.0;
                    break;

    	 case 2602:     /* NE South */
                    c->l1 = 2000000.0;
                    c->l2 = 358200.0;
                    c->l3 = 24104561.06;
                    c->l4 = 24590781.86;
                    c->l5 = 0.9999220725;
                    c->l6 = 0.6560764003;
                    c->l7 = 2451.0;
                    c->l8 = 24.68139;
                    c->l9 = 3.80977;
                    c->l10 = 3.95865;
                    c->l11 = 0.0;
                    break;

	     case 3104:     /* NY Long Island */
                    c->l1 = 2000000.0;
                    c->l2 = 266400.0;
						  c->l3 = 24235000.80;
                    c->l4 = 24462545.30;
                    c->l5 = 0.9999949000;
						  c->l6 = 0.6540820950;
                    c->l7 = 2442.0;
                    c->l8 = 20.64240;
                    c->l9 = 3.80990;
                    c->l10 = 3.93780;
                    c->l11 = 0.0;
                    break;

    	 case 3200:     /* NC */
                    c->l1 = 2000000.0;
                    c->l2 = 284400.0;
                    c->l3 = 29637059.47;
                    c->l4 = 30183611.25;
                    c->l5 = 0.9998725510;
                    c->l6 = 0.5771707700;
                    c->l7 = 2106.0;
                    c->l8 = 51.60353;
                    c->l9 = 3.81480;
                    c->l10 = 3.22483;
                    c->l11 = 0.0;
                    break;

	     case 3301:     /* ND North */
                    c->l1 = 2000000.0;
                    c->l2 = 361800.0;
                    c->l3 = 18819849.05;
                    c->l4 = 19215516.01;
                    c->l5 = 0.9999358426;
                    c->l6 = 0.7441333961;
                    c->l7 = 2876.0;
                    c->l8 = 22.57950;
                    c->l9 = 3.80339;
						  c->l10 = 5.05972;
                    c->l11 = 0.0;
                    break;

	   	 case 3302:     /* ND South */
                    c->l1 = 2000000.0;
                    c->l2 = 361800.0;
                    c->l3 = 19661027.79;
                    c->l4 = 20086977.18;
                    c->l5 = 0.9999358523;
                    c->l6 = 0.7293826040;
                    c->l7 = 2801.0;
                    c->l8 = 20.45445;
                    c->l9 = 3.80452;
                    c->l10 = 4.84504;
                    c->l11 = 0.0;
                    break;

    	 case 3401:     /* OH North */
                    c->l1 = 2000000.0;
                    c->l2 = 297000.0;
                    c->l3 = 24048738.51;
                    c->l4 = 24559158.47;
                    c->l5 = 0.9999391411;
                    c->l6 = 0.6569503193;
                    c->l7 = 2455.0;
                    c->l8 = 23.48125;
                    c->l9 = 3.80971;
                    c->l10 = 3.96783;
                    c->l11 = 0.0;
                    break;

	     case 3402:     /* OH South */
                    c->l1 = 2000000.0;
                    c->l2 = 297000.0;
						  c->l3 = 25522875.81;
                    c->l4 = 26027071.12;
                    c->l5 = 0.9999359346;
						  c->l6 = 0.6345195439;
                    c->l7 = 2354.0;
                    c->l8 = 28.63705;
                    c->l9 = 3.81121;
                    c->l10 = 3.74048;
                    c->l11 = 0.0;
                    break;

    	 case 3501:     /* OK North */
                    c->l1 = 2000000.0;
                    c->l2 = 352800.0;
                    c->l3 = 28657871.66;
                    c->l4 = 29082831.70;
                    c->l5 = 0.9999454101;
                    c->l6 = 0.5901470744;
                    c->l7 = 2161.0;
                    c->l8 = 42.56887;
                    c->l9 = 3.81402;
                    c->l10 = 3.33440;
                    c->l11 = 0.0;
                    break;

	     case 3502:     /* OK South */
                    c->l1 = 2000000.0;
                    c->l2 = 352800.0;
                    c->l3 = 30382831.06;
                    c->l4 = 30838032.96;
                    c->l5 = 0.9999359432;
                    c->l6 = 0.5676166827;
                    c->l7 = 2066.0;
                    c->l8 = 52.48935;
                    c->l9 = 3.81537;
						  c->l10 = 3.14645;
                    c->l11 = 0.0;
                    break;

    	 case 3601:     /* OR North */
                    c->l1 = 2000000.0;
                    c->l2 = 433800.0;
                    c->l3 = 20836250.94;
                    c->l4 = 21383852.48;
                    c->l5 = 0.9998945810;
                    c->l6 = 0.7091860222;
                    c->l7 = 2701.0;
                    c->l8 = 22.08858;
                    c->l9 = 3.80602;
                    c->l10 = 4.57382;
                    c->l11 = 0.0;
                    break;

	     case 3602:     /* OR South */
                    c->l1 = 2000000.0;
                    c->l2 = 433800.0;
                    c->l3 = 22341309.43;
                    c->l4 = 22888667.15;
                    c->l5 = 0.9998946058;
                    c->l6 = 0.6841473833;
                    c->l7 = 2581.0;
                    c->l8 = 22.74104;
                    c->l9 = 3.80782;
                    c->l10 = 4.26823;
                    c->l11 = 0.0;
                    break;

    	 case 3701:     /* PA North */
                    c->l1 = 2000000.0;
                    c->l2 = 279900.0;
						  c->l3 = 23755351.27;
                    c->l4 = 24211050.37;
                    c->l5 = 0.9999568410;
						  c->l6 = 0.6615397363;
                    c->l7 = 2476.0;
                    c->l8 = 21.57953;
                    c->l9 = 3.80940;
                    c->l10 = 4.01753;
                    c->l11 = 0.0;
                    break;

	     case 3702:     /* PA South */
                    c->l1 = 2000000.0;
                    c->l2 = 279900.0;
                    c->l3 = 24577800.67;
                    c->l4 = 24984826.43;
                    c->l5 = 0.9999595012;
                    c->l6 = 0.6487931668;
                    c->l7 = 2418.0;
                    c->l8 = 23.87979;
                    c->l9 = 3.81026;
                    c->l10 = 3.88319;
                    c->l11 = 0.0;
                    break;

    	 case 3901:     /* SC North */
                    c->l1 = 2000000.0;
                    c->l2 = 291600.0;
                    c->l3 = 30630125.53;
                    c->l4 = 31127724.75;
                    c->l5 = 0.9999454207;
                    c->l6 = 0.5644973800;
                    c->l7 = 2053.0;
                    c->l8 = 53.44099;
                    c->l9 = 3.81555;
						  c->l10 = 3.12127;
                    c->l11 = 0.0;
                    break;

	     case 3902:     /* SC South */
                    c->l1 = 2000000.0;
                    c->l2 = 291600.0;
                    c->l3 = 32252126.30;
                    c->l4 = 32676887.65;
                    c->l5 = 0.9999326284;
                    c->l6 = 0.5446515700;
                    c->l7 = 1972.0;
                    c->l8 = 3.57839;
                    c->l9 = 3.81669;
                    c->l10 = 2.94381;
                    c->l11 = 0.0;
                    break;

    	 case 4001:     /* SD North */
                    c->l1 = 2000000.0;
                    c->l2 = 360000.0;
                    c->l3 = 20922704.09;
                    c->l4 = 21366697.03;
                    c->l5 = 0.9999391116;
                    c->l6 = 0.7077381841;
                    c->l7 = 2694.0;
                    c->l8 = 18.93392;
                    c->l9 = 3.80612;
                    c->l10 = 4.55529;
                    c->l11 = 0.0;
                    break;

	     case 4002:     /* SD South */
                    c->l1 = 2000000.0;
                    c->l2 = 361200.0;
						  c->l3 = 21993575.61;
                    c->l4 = 22461937.05;
                    c->l5 = 0.9999068931;
						  c->l6 = 0.6898519579;
                    c->l7 = 2608.0;
                    c->l8 = 21.54370;
                    c->l9 = 3.80742;
                    c->l10 = 4.33519;
                    c->l11 = 0.0;
                    break;

    	 case 4100:     /*  TN  */
                    c->l1 = 2000000.0;
                    c->l2 = 309600.0;
                    c->l3 = 29010231.09;
                    c->l4 = 29535149.91;
                    c->l5 = 0.9999484030;
                    c->l6 = 0.5854397296;
                    c->l7 = 2141.0;
                    c->l8 = 44.28313;
                    c->l9 = 3.81431;
                    c->l10 = 3.29422;
                    c->l11 = 0.0;
                    break;

	     case 4201:     /* TX North */
                    c->l1 = 2000000.0;
                    c->l2 = 365400.0;
                    c->l3 = 29456907.29;
                    c->l4 = 29972959.94;
                    c->l5 = 0.9999108771;
                    c->l6 = 0.5795358654;
                    c->l7 = 2116.0;
                    c->l8 = 48.58548;
                    c->l9 = 3.81466;
						  c->l10 = 3.24452;
                    c->l11 = 0.0;
                    break;

    	 case 4202:     /* TX North Central */
                    c->l1 = 2000000.0;
                    c->l2 = 351000.0;
                    c->l3 = 32187809.58;
                    c->l4 = 32691654.54;
                    c->l5 = 0.9998726224;
                    c->l6 = 0.5453944146;
                    c->l7 = 1975.0;
                    c->l8 = 5.95074;
                    c->l9 = 3.81665;
                    c->l10 = 2.97107;
                    c->l11 = 0.0;
                    break;

	     case 4203:     /* TX Central */
                    c->l1 = 2000000.0;
                    c->l2 = 361200.0;
                    c->l3 = 34851703.46;
                    c->l4 = 35337121.23;
                    c->l5 = 0.9998817443;
                    c->l6 = 0.5150588857;
                    c->l7 = 1852.0;
                    c->l8 = 21.62181;
                    c->l9 = 3.81832;
                    c->l10 = 2.74550;
                    c->l11 = 0.0;
                    break;

    	 case 4204:     /* TX South Central */
                    c->l1 = 2000000.0;
                    c->l2 = 356400.0;
						  c->l3 = 37261509.20;
                    c->l4 = 37807440.38;
                    c->l5 = 0.9998632433;
						  c->l6 = 0.4899126408;
                    c->l7 = 1752.0;
                    c->l8 = 37.19059;
                    c->l9 = 3.81962;
                    c->l10 = 2.56899;
                    c->l11 = 0.0;
                    break;

	     case 4205:     /* TX South */
                    c->l1 = 2000000.0;
                    c->l2 = 354600.0;
                    c->l3 = 41091749.54;
                    c->l4 = 41576762.39;
                    c->l5 = 0.9998947956;
                    c->l6 = 0.4540068519;
                    c->l7 = 1612.0;
                    c->l8 = 59.30342;
                    c->l9 = 3.82138;
                    c->l10 = 2.33094;
                    c->l11 = 0.0;
                    break;

    	 case 4301:     /* UT North */
                    c->l1 = 2000000.0;
                    c->l2 = 401400.0;
                    c->l3 = 23894872.45;
                    c->l4 = 24229110.29;
                    c->l5 = 0.9999568422;
                    c->l6 = 0.6593554910;
                    c->l7 = 2466.0;
                    c->l8 = 21.96231;
                    c->l9 = 3.80955;
						  c->l10 = 3.99323;
                    c->l11 = 0.0;
                    break;

	     case 4302:     /* UT Central */
                    c->l1 = 2000000.0;
                    c->l2 = 401400.0;
                    c->l3 = 25117176.75;
                    c->l4 = 25664114.42;
                    c->l5 = 0.9998988207;
                    c->l6 = 0.6405785926;
                    c->l7 = 2381;
                    c->l8 = 29.30066;
                    c->l9 = 3.81081;
                    c->l10 = 3.80024;
                    c->l11 = 0.0;
                    break;


    	 case 4303:     /* UT South */
                    c->l1 = 2000000.0;
                    c->l2 = 401400.0;
                    c->l3 = 27025955.35;
                    c->l4 = 27432812.88;
                    c->l5 = 0.9999512939;
                    c->l6 = 0.6126873424;
                    c->l7 = 2258.0;
                    c->l8 = 34.16878;
                    c->l9 = 3.81262;
                    c->l10 =3.53414;
                    c->l11 = 0.0;
                    break;

		case 4501:     /* VA North */
						  c->l1 = 2000000.0;
						  c->l2 = 282600.0;
						  c->l3 = 26230200.09;
						  c->l4 = 26576444.45;
						  c->l5 = 0.9999483859;
						  c->l6 = 0.6241178597;
						  c->l7 = 2308.0;
						  c->l8 = 30.78682;
						  c->l9 = 3.81189;
						  c->l10 = 3.64047;
						  c->l11 = 0.0;
						  break;

		case 4502:     /* VA South */
						  c->l1 = 2000000.0;
						  c->l2 = 282600.0;
						  c->l3 = 27434800.06;
						  c->l4 = 27811312.71;
						  c->l5 = 0.9999454027;
						  c->l6 = 0.6069248249;
						  c->l7 = 2233.0;
						  c->l8 = 36.41072;
						  c->l9 = 3.81298;
						  c->l10 = 3.48187;
						  c->l11 = 0.0;
						  break;

		case 4601:     /* WA North */
						  c->l1 = 2000000.0;
						  c->l2 = 435000.0;
						  c->l3 = 18798081.67;
						  c->l4 = 19205863.43;
						  c->l5 = 0.9999422551;
						  c->l6 = 0.7445203390;
						  c->l7 = 2878.0;
						  c->l8 = 22.15711;
						  c->l9 = 3.80336;
						  c->l10 = 5.06556;
						  c->l11 = 0.0;
						  break;

		  case 4602:     /* WA South */
						  c->l1 = 2000000.0;
						  c->l2 = 433800.0;
						  c->l3 = 19832653.52;
						  c->l4 = 20289119.60;
						  c->l5 = 0.9999145875;
						  c->l6 = 0.7263957947;
						  c->l7 = 2786.0;
						  c->l8 = 21.72121;
						  c->l9 = 3.80474;
						  c->l10 = 4.80336;
						  c->l11 = 0.0;
						  break;

		case 4701:     /* WV North */
						  c->l1 = 2000000.0;
						  c->l2 = 286200.0;
						  c->l3 = 25305029.12;
						  c->l4 = 25715126.55;
						  c->l5 = 0.9999407460;
						  c->l6 =  0.6377729696;
						  c->l7 = 2368.0;
						  c->l8 = 57.52979;
						  c->l9 = 3.81099;
						  c->l10 = 3.77244;
						  c->l11 = 0.0;
						  break;

		case 4702:     /* WV South */
						  c->l1 = 2000000.0;
						  c->l2 = 291600.0;
						  c->l3 = 26639323.45;
						  c->l4 = 27070620.78;
						  c->l5 = 0.9999256928;
						  c->l6 = 0.6181953936;
						  c->l7 = 2282.0;
						  c->l8 = 33.82207;
						  c->l9 = 3.81227;
						  c->l10 = 3.58491;
						  c->l11 = 0.0;
						  break;

		case 4801:     /* WI  North */
						  c->l1 = 2000000.0;
						  c->l2 = 324000.0;
						  c->l3 = 20124133.05;
						  c->l4 = 20489179.67;
						  c->l5 = 0.9999453461;
						  c->l6 = 0.7213707913;
						  c->l7 = 2761.0;
						  c->l8 = 19.04034;
						  c->l9 = 3.80511;
						  c->l10 = 4.73451;
						  c->l11 = 0.0;
						  break;

		case 4802:     /* WI Central  */
						  c->l1 = 2000000.0;
						  c->l2 = 324000.0;
						  c->l3 = 21050746.99;
						  c->l4 = 21430913.91;
						  c->l5 = 0.9999407059;
						  c->l6 = 0.7055766312;
						  c->l7 = 2683.0;
						  c->l8 = 48.81363;
						  c->l9 = 3.80628;
						  c->l10 = 4.52782;
						  c->l11 = 0.0;
						  break;

		case 4803:     /* WI  South */
						  c->l1 = 2000000.0;
						  c->l2 = 324000.0;
						  c->l3 = 22161432.25;
						  c->l4 = 22672134.66;
						  c->l5 = 0.9999325474;
						  c->l6 = 0.6871032423;
						  c->l7 = 2595.0;
						  c->l8 = 20.01691;
						  c->l9 = 3.80761;
						  c->l10 = 4.30274;
						  c->l11 = 0.0;
						  break;
		case 5201:     /* PR / VI */
						  c->l1 = 500000.0;
						  c->l2 = 239160.0;
						  c->l3 = 63542221.66;
						  c->l4 = 63687479.44;
						  c->l5 = 0.9999939449;
						  c->l6 = 0.3128882281;
						  c->l7 = 1088.0;
						  c->l8 = 48.44933;
						  c->l9 = 3.82699;
						  c->l10 = 1.51030;
						  c->l11 = 0.0;
						  break;
		case 5202:     /* St. Croix */
						  c->l1 = 500000.0;
						  c->l2 = 239160.0;
						  c->l3 = 63542221.66;
						  c->l4 = 63787479.44;
						  c->l5 = 0.9999939449;
						  c->l6 = 0.3128882281;
						  c->l7 = 1088.0;
						  c->l8 = 48.44933;
						  c->l9 = 3.82699;
						  c->l10 = 1.51030;
						  c->l11 = 0.0;
						  break;
		  case 101:	/* AL East */
						  c->t1 = 500000.0;
						  c->t2 = 309000.0;
						  c->t3 = 1822.0;
						  c->t4 = 21.00903;
						  c->t5 = 0.9999600000;
						  c->t6 = 0.3817065;
						  break;

		case 102:	/* AL West */
						  c->t1 = 500000.0;
						  c->t2 = 315000.0;
						  c->t3 = 1792.0;
						  c->t4 = 25.53386;
						  c->t5 = 0.9999333333;
						  c->t6 = 0.3817477;
						  break;

		  case 201:	/* AZ East */
						  c->t1 = 500000.00;
						  c->t2 = 396600.00;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3816485;
						  break;

		  case 202:	/* AZ Central */
						  c->t1 = 500000.0;
						  c->t2 = 402900.0;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3816485;
						  break;

		 case 203:	/* AZ West */
						  c->t1 = 500000.0;
						  c->t2 = 409500.0;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999333333;
						  c->t6 = 0.3815948;
						  break;

		  case 700:	/* DE */
						  c->t1 = 500000.0;
						  c->t2 = 271500.0;
						  c->t3 = 2271.0;
						  c->t4 = 30.53702;
						  c->t5 = 0.9999950281;
						  c->t6 = 0.3811454;
						  break;

		  case 901:	/* FL  East */
						  c->t1 = 500000.0;
						  c->t2 = 291600.0;
						  c->t3 = 1453.0;
						  c->t4 = 26.09287;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3821090;
						  break;

		  case 902:	/* FL West */
						  c->t1 = 500000.0;
						  c->t2 = 295200.0;
						  c->t3 = 1453.0;
						  c->t4 = 26.09287;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3821090;
						  break;

		  case 1001:     /* GA East */
						  c->t1 = 500000.0;
						  c->t2 = 295800.0;
						  c->t3 = 1792.0;
						  c->t4 = 25.53386;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3817593;
						  break;

		 case 1002:     /* GA West */
						  c->t1 = 500000.0;
						  c->t2 = 303000.0;
						  c->t3 = 1792.0;
						  c->t4 = 25.53386;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3817593;
						  break;

		  case 5101:     /* HI 1 */
						  c->t1 = 500000.0;
						  c->t2 = 559800.0;
						  c->t3 = 1124.0;
						  c->t4 = 39.52714;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3826496;
						  break;

		 case 5102:     /* HI 2 */
						  c->t1 = 500000.0;
						  c->t2 = 564000.0;
						  c->t3 = 1214.0;
						  c->t4 = 18.21554;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3825762;
						  break;

		  case 5103:     /* HI 3 */
						  c->t1 = 500000.0;
						  c->t2 = 568800.0;
						  c->t3 = 1264.0;
						  c->t4 = 6.77497;
						  c->t5 = 0.9999900000;
						  c->t6 = 0.3825176;
						  break;

		 case 5104:     /* HI 4 */
						  c->t1 = 500000.0;
						  c->t2 = 574200.0;
						  c->t3 = 1303.0;
						  c->t4 = 57.83623;
						  c->t5 = 0.9999900000;
						  c->t6 = 0.3824812;
						  break;

		  case 5105:     /* HI 5 */
						  c->t1 = 500000.0;
						  c->t2 = 576600.0;
						  c->t3 = 1294.0;
						  c->t4 = 0.05280;
						  c->t5 = 0.9999999999;
						  c->t6 = 0.3824867;
						  break;

		 case 1101:     /* ID East */
						  c->t1 = 500000.0;
						  c->t2 = 403800.0;
						  c->t3 = 2491.0;
						  c->t4 = 18.35156;
						  c->t5 = 0.9999473684;
						  c->t6 = 0.3807624;
						  break;

		  case 1102:     /* ID Central */
						  c->t1 = 500000.0;
						  c->t2 = 410400.0;
						  c->t3 = 2491.0;
						  c->t4 = 18.35156;
						  c->t5 = 0.9999473684;
						  c->t6 = 0.3807624;
						  break;

		 case 1103:     /* ID West */
						  c->t1 = 500000.0;
						  c->t2 = 416700.0;
						  c->t3 = 2491.0;
						  c->t4 = 18.35156;
						  c->t5 = 0.9999333333;
						  c->t6 =   0.3806227;
						  break;

		  case 1201:     /* IL East */
						  c->t1 = 500000.0;
						  c->t2 = 318000.0;
						  c->t3 = 2191.0;
						  c->t4 = 37.04639;
						  c->t5 = 0.9999750000;
						  c->t6 = 0.3811074;
						  break;

		 case 1202:     /* IL West */
						  c->t1 = 500000.0;
						  c->t2 = 324600.0;
						  c->t3 = 2191.0;
						  c->t4 = 37.04639;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3811332;
						  break;

		  case 1301:     /* IN East */
						  c->t1 = 500000.0;
						  c->t2 = 308400.0;
						  c->t3 = 2241.0;
						  c->t4 = 32.84965;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3811064;
						  break;

		 case 1302:     /* IN West */
						  c->t1 = 500000.0;
						  c->t2 = 313500.0;
						  c->t3 = 2241.0;
						  c->t4 = 32.84965;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3811064;
						  break;

		  case 1801:     /* ME East */
						  c->t1 = 500000.0;
						  c->t2 = 246600.0;
						  c->t3 = 2621.0;
						  c->t4 = 15.15187;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3806180;
						  break;

		 case 1802:     /* ME West */
						  c->t1 = 500000.0;
						  c->t2 = 252600.0;
						  c->t3 = 2561.0;
						  c->t4 = 16.25668;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3806575;
						  break;

		  case 2101:     /* MI East */
						  c->t1 = 500000.0;
						  c->t2 = 301200.0;
						  c->t3 = 2481.0;
						  c->t4 = 18.72150;
						  c->t5 = 0.9999428571;
						  c->t6 = 0.3807283;
						  break;

		 case 2102:     /* MI Central */
						  c->t1 = 500000.0;
						  c->t2 = 308700.0;
						  c->t3 = 2481.0;
						  c->t4 = 18.72150;
						  c->t5 = 0.9999090909;
						  c->t6 = 0.3807541;
						  break;

		  case 2103:     /* MI West */
						  c->t1 = 500000.0;
						  c->t2 = 319500.0;
						  c->t3 = 2481.0;
						  c->t4 = 18.72150;
						  c->t5 = 0.9999090909;
						  c->t6 = 0.3805361;
						  break;

		 case 2301:     /* MS East */
						  c->t1 = 500000.0;
						  c->t2 = 319800.0;
						  c->t3 = 1772.0;
						  c->t4 = 28.62716;
						  c->t5 = 0.9999600000;
						  c->t6 = 0.3817257;
						  break;

		  case 2302:     /* MS West */
						  c->t1 = 500000.0;
						  c->t2 = 325200.0;
						  c->t3 = 1822.0;
						  c->t4 = 21.00903;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3816986;
						  break;

		 case 2401:     /* MO East */
						  c->t1 = 500000.0;
						  c->t2 = 325800.0;
						  c->t3 = 2141.0;
						  c->t4 = 41.66790;
						  c->t5 = 0.9999333333;
						  c->t6 = 0.3812643;
						  break;

		  case 2402:     /* MO Central */
						  c->t1 = 500000.0;
						  c->t2 = 333000.0;
						  c->t3 = 2141.0;
						  c->t4 = 41.66790;
						  c->t5 = 0.9999333333;
						  c->t6 = 0.3812422;
						  break;

		 case 2403:     /* MO West */
						  c->t1 = 500000.0;
						  c->t2 = 340200.0;
						  c->t3 = 2161.0;
						  c->t4 = 39.76857;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3812362;
						  break;

		  case 2701:     /* NV East */
						  c->t1 = 500000.0;
						  c->t2 = 416100.0;
						  c->t3 = 2076.0;
						  c->t4 = 48.30429;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3812311;
						  break;

		 case 2702:     /* NV Central */
						  c->t1 = 500000.0;
						  c->t2 = 420000.0;
						  c->t3 = 2076.0;
						  c->t4 = 48.30429;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3812311;
						  break;

		  case 2703:     /* NV West */
						  c->t1 = 500000.0;
						  c->t2 = 426900.0;
						  c->t3 = 2076.0;
						  c->t4 = 48.30429;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3812311;
						  break;

		 case 2800:     /* NH */
						  c->t1 = 500000.0;
						  c->t2 = 258000.0;
						  c->t3 = 2541.0;
						  c->t4 = 16.76677;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3807327;
						  break;

		  case 2900:     /* NJ */
						  c->t1 = 2000000.0;
						  c->t2 = 268800.0;
						  c->t3 = 2321.0;
						  c->t4 = 27.02745;
						  c->t5 = 0.9999750295;
						  c->t6 = 0.3810845;
						  break;

		 case 3001:     /* NM East */
						  c->t1 = 500000.0;
						  c->t2 = 375600.0;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999090909;
						  c->t6 = 0.3816135;
						  break;

		 case 3002:     /* NM Central */
						  c->t1 = 500000.0;
						  c->t2 = 382500.0;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999000000;
						  c->t6 = 0.3816204;
						  break;

		 case 3003:     /* NM West */
						  c->t1 = 500000.0;
						  c->t2 = 388200.0;
						  c->t3 = 1852.0;
						  c->t4 = 16.62358;
						  c->t5 = 0.9999166667;
						  c->t6 = 0.3816288;
						  break;

		 case 3101:     /* NY East */
						  c->t1 = 500000.0;
						  c->t2 = 267600.0;
						  c->t3 = 2391.0;
						  c->t4 = 22.84247;
						  c->t5 = 0.9999666667;
						  c->t6 = 0.3808377;
						  break;

		 case 3102:     /* NY Central */
						  c->t1 = 500000.0;
						  c->t2 = 275700.0;
						  c->t3 = 2391.0;
						  c->t4 = 22.84247;
						  c->t5 = 0.9999375000;
						  c->t6 = 0.3808450;
						  break;

		  case 3103:     /* NY West */
						  c->t1 = 500000.0;
						  c->t2 = 282900.0;
						  c->t3 = 2391.0;
						  c->t4 = 22.84247;
						  c->t5 = 0.9999375000;
						  c->t6 = 0.3808750;
						  break;

		 case 3800:     /* RI */
						  c->t1 = 500000.0;
						  c->t2 = 257400.0;
						  c->t3 = 2456.0;
						  c->t4 = 19.72344;
						  c->t5 = 0.9999937500;
						  c->t6 = 0.3809220;
						  break;

		  case 4400:     /* VT */
						  c->t1 = 500000.0;
						  c->t2 = 261000.0;
						  c->t3 = 2541.0;
						  c->t4 = 16.76677;
						  c->t5 = 0.9999642857;
						  c->t6 = 0.3807420;
						  break;

		 case 4901:     /* WY East */
						  c->t1 = 500000.0;
						  c->t2 = 378600.0;
						  c->t3 = 2431.0;
						  c->t4 = 20.83533;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3808422;
						  break;


		  case 4902:     /* WY East Central */
						  c->t1 = 500000.0;
						  c->t2 = 386400.0;
						  c->t3 = 2431.0;
						  c->t4 = 20.83533;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3808422;
						  break;

		 case 4903:     /* WY West Central */
						  c->t1 = 500000.0;
						  c->t2 = 391500.0;
						  c->t3 = 2431.0;
						  c->t4 = 20.83533;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3808422;
						  break;

		  case 4904:     /* WY West */
						  c->t1 = 500000.0;
						  c->t2 = 396300.0;
						  c->t3 = 2431.0;
						  c->t4 = 20.83533;
						  c->t5 = 0.9999411765;
						  c->t6 = 0.3808422;
						  break;
        case 5002:
		        			c->ak_c = 500000.0;
                     c->ak_cm = 511200.0;
                     break;
	  	  case 5003:
        					c->ak_c = 500000.0;
                     c->ak_cm = 525600.0;
                     break;
	     case 5004:
		        			c->ak_c = 500000.0;
                     c->ak_cm = 540000.0;
                     break;
        case 5005:
        					c->ak_c = 500000.0;
                     c->ak_cm = 554400.0;
                     break;
        case 5006:
        					c->ak_c = 500000.0;
                     c->ak_cm = 568800.0;
                     break;
        case 5007:
        					c->ak_c = 700000.0;
                     c->ak_cm = 583200.0;
                     break;
        case 5008:
		        			c->ak_c = 500000.0;
                     c->ak_cm = 597600.0;
                     break;
        case 5009:
		        			c->ak_c = 600000.0;
                     c->ak_cm = 612000.0;
                     break;
        case 5001:	break;
		default:	return BAD_SP27_ZONE;
	 }

	 return CC_SUCCESS;
}

