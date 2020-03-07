#ifndef CCNAD_H
#define CCNAD_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXNADCONAREAS 50

DLLEXPORT int _stdcall describe_standard_nadcon_areas(char *desc);
DLLEXPORT int _stdcall describe_nad83_to_hpgn_nadcon_area(char *desc);
DLLEXPORT int _stdcall describe_hpgn_to_nad83_nadcon_area(char *desc);

DLLEXPORT int _stdcall initialize_nadcon(char *nadcon_file_path);
DLLEXPORT int _stdcall initialize_nadcon_from_file(char *nadcon_file_path, char *area_filename);
DLLEXPORT int _stdcall initialize_nad83_to_hpgn(char *base_laslos);
DLLEXPORT int _stdcall initialize_hpgn_to_nad83(char *base_laslos);

DLLEXPORT int _stdcall clean_up_nadcon(void);
DLLEXPORT int _stdcall clean_up_nad83_to_hpgn(void);
DLLEXPORT int _stdcall clean_up_hpgn_to_nad83(void);

DLLEXPORT int _stdcall nad27_to_nad83(double *xin, double *yin,
													double *xout, double *yout,
                                       double *dlas, double *dlos,
                                       double *dlam, double *dlom);

DLLEXPORT int _stdcall  nad83_to_nad27(double *xin, double *yin,
													double *xout, double *yout,
                                       double *dlas, double *dlos,
                                       double *dlam, double *dlom);

DLLEXPORT int _stdcall nad83_to_hpgn(double *xin, double *yin,
													double *xout, double *yout,
                                       double *dlas, double *dlos,
                                       double *dlam, double *dlom);

DLLEXPORT int _stdcall hpgn_to_nad83(double *xin, double *yin,
													double *xout, double *yout,
                                       double *dlas, double *dlos,
                                       double *dlam, double *dlom);

#endif
