#ifndef CCVERT_H
#define CCVERT_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXVERTCONAREAS 25

DLLEXPORT int _stdcall initialize_vertcon(char *vertcon_file_path);
DLLEXPORT int _stdcall initialize_vertcon_from_file(char *vertcon_file_path, char *area_filename);
DLLEXPORT int _stdcall vertcon(double x, double y, double *ght);
DLLEXPORT int _stdcall clean_up_vertcon(void);
DLLEXPORT int _stdcall describe_vertcon_areas(char *desc);

#endif
