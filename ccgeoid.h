#ifndef CCGEOID_H
#define CCGEOID_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXGEOIDAREAS 50

/*
	GEOID_HEADER can be read directly from the beginning of the
    las/los files.  xo and yo combine to give the south-west
    (lower-left) corner of the data file.
*/
struct GEOID_HEADER{
	char rident[56];/* Character identity? */
	char pgm[8];	/* Another character identy? */
   long nc;		/* Number of columns in data file */
	long nr;		/* Number of rows in data file */
	long nz;		/* Number of z's in data file (usually 1) */
   float xo;		/* Far left lon value (positive east) */
	float dx;		/* Spacing between columns (degrees lon) */
	float yo;		/* Lower lat value (positive north) */
	float dy;		/* Spacing between rows (degrees lat) */
	float angle;	/* Some angle value? (usually 0) */
};
typedef struct GEOID_HEADER *pGEOID_HEADER;

/*
	GEOID_DATA hold all the important information about each
    las/los file.  Each .geo file will have a
    GEOID_DATA structure assocated with it.
*/
struct GEOID_DATA{
	FILE *fp;
	char filename[MAXFILE];
	float xmin;	/* Minimum lon value (positive east) */
	float xmax;	/* Maximum lon value (positive east) */
	float ymin;	/* Minimum lat value (positive north) */
	float ymax;	/* Maximum lat value (positive north) */
	float dx;		/* Spacing between columns (degrees lon) */
	float dy;		/* Spacing between rows (degrees lat) */
	long nc;		/* Number of columns in data file */
	long nr;		/* Number of rows in data file */
	long nz;		/* Number of z's in data file (usually 1) */
	long lrecl;		/* Data file record length (from original FORTRAN) */
};
typedef struct GEOID_DATA *pGEOID_DATA;

DLLEXPORT int _stdcall geoid(double x, double y, double *ght);

DLLEXPORT int _stdcall describe_geoid_areas(char *desc);

DLLEXPORT int _stdcall initialize_geoid90(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid93(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid96(char *geoid_file_path);
DLLEXPORT int _stdcall initialize_geoid_from_file(char *geoid_file_path, char *area_filename);

DLLEXPORT int _stdcall clean_up_geoid(void);

#endif
