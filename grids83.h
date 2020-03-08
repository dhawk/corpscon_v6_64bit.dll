#ifndef GRIDS83_H
#define GRIDS83_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

struct TM83_CONSTANTS{
	double a,f,esq;
	double fe,fn,or,sf,cm;
	double r,U0,U2,U4,U6,V0,V2,V4,V6;
	double so,omo;
// Used by UTM_S
//	double A,B,C;
};
typedef struct TM83_CONSTANTS *pTM83_CONSTANTS;

struct OM83_CONSTANTS{
	double a,flat,esq,e;
	double fe,fn,sf,phic,lamc,alphac;
	double b,c,d,f,g,i,lamo;
   double f0,f2,f4,f6;
};
typedef struct OM83_CONSTANTS *pOM83_CONSTANTS;

struct LAM83_CONSTANTS{
	double a,f,e;
	double fe,fn,ps,pn,pb,l0;
	double sinphi0,rb,k;
};
typedef struct LAM83_CONSTANTS *pLAM83_CONSTANTS;

struct GRIDS83_CONSTANTS{
	struct TM83_CONSTANTS tm83_c;
   struct LAM83_CONSTANTS lam83_c;
   struct OM83_CONSTANTS om83_c;
};
typedef struct GRIDS83_CONSTANTS *pGRIDS83_CONSTANTS;


DLLEXPORT int _stdcall geo83_to_sp83(int zone, int unit,
													double *lat_in, double *lon_in,
                                       double *north, double *east,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall geo83_to_utm83(int zone, int unit,
													double *lat_in, double *lon_in,
                                       double *north, double *east,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall geo27_to_utm27(int zone, int unit,
													double *lat_in, double *lon_in,
                                       double *north, double *east,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall sp83_to_geo83(int zone, int unit,
													double *lat, double *lon,
                                       double *north_in, double *east_in,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall utm83_to_geo83(int zone, int unit,
													double *lat, double *lon,
                                       double *north_in, double *east_in,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall utm27_to_geo27(int zone, int unit,
													double *lat, double *lon,
                                       double *north_in, double *east_in,
                                       double *conv, double *kp);
DLLEXPORT int _stdcall describe_sp83_zone_constants(int zone, char *desc);

int get_grids83_function(int opt, int zone, int *sp_function);
int init_sp83_constants(int zone, pGRIDS83_CONSTANTS c);

int geo83_to_tm83(pGRIDS83_CONSTANTS grids83_c,
						double *lat_in, double *lon_in,
                  double *north, double *east,
                  double *conv, double *kp);

int tm83_to_geo83(pGRIDS83_CONSTANTS grids83_c,
						double *lat, double *lon,
                  double *north_in, double *east_in,
                  double *conv, double *kp);

int geo83_to_lam83(pGRIDS83_CONSTANTS grids83_c,
							double *lat_in, double *lon_in,
                     double *north, double *east,
                     double *conv, double *kp);

int lam83_to_geo83(pGRIDS83_CONSTANTS grids83_c,
							double *lat, double *lon,
                     double *north_in, double *east_in,
                     double *conv, double *kp);

int init_lamb_83(int zone, pLAM83_CONSTANTS c);

int init_tm_83(int zone, pTM83_CONSTANTS c);
int init_tm_const_83(pTM83_CONSTANTS c);

int init_om_83(int zone, pOM83_CONSTANTS c);

int Direct_AK1_83(pGRIDS83_CONSTANTS grids83_c,
							double *lat_in, double *lon_in,
   	               double *north, double *east,
      	            double *conv, double *kp);
int Inverse_AK1_83(pGRIDS83_CONSTANTS grids83_c,
							double *lat, double *lon,
                     double *north_in, double *east_in,
                     double *conv, double *kp);

#endif
