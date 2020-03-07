#ifndef CCGEOID_H
#define CCGEOID_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXGEOIDAREAS 50

DLLEXPORT int _stdcall geoid(double x, double y, double *ght);

DLLEXPORT int _stdcall describe_geoid_areas(char *desc);

DLLEXPORT int _stdcall initialize_geoid90(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid93(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid96(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid_from_file(char *geoid_file_path, char *area_filename);

DLLEXPORT int _stdcall clean_up_geoid(void);

#endif
