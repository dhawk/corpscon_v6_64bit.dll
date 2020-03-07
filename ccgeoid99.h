#ifndef CCGEOID99_H
#define CCGEOID99_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXGEOID99AREAS 50

DLLEXPORT int _stdcall initialize_geoid99(char *geoid99_file_path);
DLLEXPORT int _stdcall initialize_geoid03(char *geoid99_file_path);
DLLEXPORT int _stdcall initialize_geoid99_from_file(char *geoid99_file_path, char *area_filename);
DLLEXPORT int _stdcall geoid99(double xlon, double ylat, double *ght);
DLLEXPORT int _stdcall clean_up_geoid99(void);
DLLEXPORT int _stdcall describe_geoid99_areas(char *desc);

#endif