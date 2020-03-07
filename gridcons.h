#include <math.h>

#ifndef GRIDCONS_H
#define GRIDCONS_H

#define RAD (180.0 * M_1_PI)

#define GRS80_a 6378137.0
#define GRS80_f (1.0 / 298.25722210088)

#define CLARKE1866_a 6378206.4
#define CLARKE1866_f (1.0 / 294.9786982)

//#define NAD27 27
//#define NAD83 83
//#define NAD91 91

#define GEO_TO_SP 1
#define SP_TO_GEO 2
#define GEO_TO_UTM 3
#define UTM_TO_GEO 4

#define US_FOOT 1
#define INT_FOOT 2
#define METER 3

//double torad(int deg, int min);
double torad(double deg, double min);
double todd(int deg, int min);
double compute_sf(double in);

void DD_to_DMS(double dd, char *sign, int *deg, int *min, double *sec);
//double cvt(short curr,short des,double pt);
double cvt(int curr,int des,double pt);

double ufoot2meter(double val);
double ufoot2ifoot(double val);
double ifoot2meter(double val);
double ifoot2ufoot(double val);
double meter2ufoot(double val);
double meter2ifoot(double val);
double cvt_none(double val);
int get_cvt_function(int curr, int des, int *cvt_function);


int ConvertToDecDeg(double *value,char *str);
#endif

