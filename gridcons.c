#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridcons.h"
#include "ccerror.h"

/*
	This routine converts deg, min to radians.
*/
double torad(double deg, double min)
/*double torad(int deg, int min)*/
{
/*	return ((double) deg + (double) min / 60.0) / RAD;*/
	return (deg + min / 60.0) / RAD;
}

/*
	This routine calculates scale factor.
*/
double compute_sf(double in)
{
	return (1.0 - 1.0 / in);
}


/* converts the point to the format desired from the current */
/* format, returns the value as a double. US Foot-1, INTL    */
/* FOOT-2, Meters-3.....                                     */
/* Written by Jeff Walker, U.S. Army Topographic Engineering */
/* Center													 */
//double cvt(short curr,short des,double pt)
double cvt(int curr,int des, double pt)
{
double  UF=3937.0,   // US FEET FOR
	UM=1200.0,   // METERS CONVERSION.
	IF=1250.0,   // INTERNATIONAL FEET FOR
	IM=381.0;    // METERS CONVERSION.
	if(curr==des) return pt;
	if(curr==US_FOOT) {
		pt*=(double)(UM/UF);
		if(des==INT_FOOT) pt*=(double)(IF/IM);}
	if(curr==INT_FOOT) {
		pt*=(double)(IM/IF);
		if(des==US_FOOT) pt*=(double)(UF/UM);}
	if(curr==METER)
		pt = des==US_FOOT ? pt*(double)(UF/UM):pt*(double)(IF/IM);
	return pt;
}

double ufoot2meter(double val){ return val * (1200.0 / 3937.0);}
double ufoot2ifoot(double val){ return val * (1200.0 / 3937.0) * (1250.0 / 381.0);}
double ifoot2meter(double val){ return val * (381.0 / 1250.0);}
double ifoot2ufoot(double val){ return val * (381.0 / 1250.0) * (3937.0 / 1200.0);}
double meter2ufoot(double val){ return val * (3937.0 / 1200.0);}
double meter2ifoot(double val){ return val * (1250.0 / 381.0);}
double cvt_none(double val){ return val;}

int get_cvt_function(int curr, int des, int *cvt_function)
{
	if(curr == des)
   {
		*cvt_function = cvt_none;
      return CC_SUCCESS;
   }

   if(curr == METER)
   {
   	if(des == US_FOOT) { *cvt_function = meter2ufoot; return CC_SUCCESS;}
   	if(des == INT_FOOT) { *cvt_function = meter2ifoot; return CC_SUCCESS;}
      return BAD_OUTPUT_UNIT_SPECIFIED;
   }

   if(curr == US_FOOT)
   {
   	if(des == METER) { *cvt_function = ufoot2meter; return CC_SUCCESS;}
      if(des == INT_FOOT) { *cvt_function = ufoot2ifoot; return CC_SUCCESS;}
      return BAD_OUTPUT_UNIT_SPECIFIED;
   }

   if(curr == INT_FOOT)
   {
   	if(des == METER) { *cvt_function = ifoot2meter; return CC_SUCCESS;}
      if(des == US_FOOT) { *cvt_function = ifoot2ufoot; return CC_SUCCESS;}
      return BAD_OUTPUT_UNIT_SPECIFIED;
   }

   return BAD_INPUT_UNIT_SPECIFIED;
}

/*
	This procedure converts a Dec.Deg to Deg-Min-Dec.Sec
	Written by Jeff Walker, U.S. Army Topographic Engineering
	Center.
*/

void DD_to_DMS(double dd, char *sign, int *deg, int *min, double *sec)
{
double dd2=0;
int d,m;
double s;
	dd2 = dd<0 ? dd2-dd:dd;
	d = (int)(dd2);
	m = (int)((dd2 - d) * 60.0);
	s = (dd2 - (double)(d + (m/60.0)))*3600.0;
	if(s>=59.99999) {
		s = 0.0;
		m ++;
	}
	if(m==60) {
		m = 0;
		d ++;
	}
	*deg = d;
	*min = m;
	*sec = s;
	*sign = dd<0?'-':' ';

/*
	if(dd<0){
		if(*deg) *deg *= -1.0;
		else if(*min) *min *= -1.0;
		else if(*sec>0.000001) *sec *= -1.0;
	}
*/
}

//	This procedure converts a dd mm ss string to decimal degrees.
int ConvertToDecDeg(double *value,char *str)
{
double deg=0.0,min=0.0,sec=0.0;
int ideg=0,imin=0;
char in[121];

	strcpy(in,str);

	switch(sscanf(in,"%lf %lf %lf",&deg,&min,&sec))
	{
		case 1:
			*value = deg;
			return 1;
		case 2:
			if(sscanf(in,"%d %lf",&ideg,&min)!=2) return 0;
			if(min>=60.0||min<0.0) return 0;
			*value = deg + min/60.0;
			return 1;
		case 3:
			if(sscanf(in,"%d %d %lf",&ideg,&imin,&sec)!=3) return 0;
			if(min>=60.0||min<0.0||sec>=60.0||sec<0.0)return 0;
			*value = deg + min/60.0 + sec/3600.0;
			return 1;
		default:
			return 0;
	}
}

