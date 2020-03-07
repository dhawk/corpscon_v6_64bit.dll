#ifndef GRIDS27_H
#define GRIDS27_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

DLLEXPORT int _stdcall geo27_to_sp27(int zone, int unit,
													double *lat, double *lon,
													double *north, double *east,
                              			double *conv, double *kp);

DLLEXPORT int _stdcall sp27_to_geo27(int zone, int unit,
													double *lat, double *lon,
													double *north, double *east,
                              			double *conv, double *kp);

DLLEXPORT int _stdcall describe_sp27_zone_constants(int zone, char *desc);

int get_grids27_function(int opt, int zone, int *sp_function);

int init_sp27_constants(int zone, struct SP27_CONSTANTS *c);

int geo27_to_tm27(struct SP27_CONSTANTS *c,
								double *lat_in, double *lon_in,
								double *north, double *east,
								double *conv, double *kp);
int geo27_to_lam27(struct SP27_CONSTANTS *c,
								double *lat_in, double *lon_in,
								double *north, double *east,
								double *conv, double *kp);
int tm27_to_geo27(struct SP27_CONSTANTS *c,
								double *lat, double *lon,
								double *north_in, double *east_in,
								double *conv, double *kp);
int lam27_to_geo27(struct SP27_CONSTANTS *c,
								double *lat, double *lon,
								double *north_in, double *east_in,
								double *conv, double *kp);

int Direct_AK1_27(struct SP27_CONSTANTS *sp27_c,
								double *lat_in, double *lon_in,
								double *north, double *east,
								double *conv, double *kp);
int Inverse_AK1_27(struct SP27_CONSTANTS *sp27_c,
								double *lat, double *lon,
								double *north_in, double *east_in,
								double *conv, double *kp);
int Direct_AK2thru9_27(struct SP27_CONSTANTS *sp27_c,
								double *lat_in, double *lon_in,
								double *north, double *east,
								double *conv, double *kp);
int Inverse_AK2thru9_27(struct SP27_CONSTANTS *sp27_c,
								double *lat, double *lon,
								double *north_in, double *east_in,
								double *conv, double *kp);


struct SP27_CONSTANTS{
	double t1,t2,t3,t4,t5,t6;
	double l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11;
   double ak_c,ak_cm;
};

#endif
