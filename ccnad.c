#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dir.h>
#include <windows.h>
#include "ccfileutil.h"
#include "ccnad.h"
#include "ccerror.h"

/*
	NADCON_HEADER can be read directly from the beginning of the
    las/los files.  xo and yo combine to give the south-west
    (lower-left) corner of the data file.  Refer to
    open_nadcon_data_file() to see how to read las/los files.
*/
struct NADCON_HEADER{
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

/*
	NADCON_FILE_INFO hold all the important information about each
    las/los file.  Each .las and each .los file will have a
    data_file_info structure assocated with it.  Refer to
    open_nadcon_data_files() to see how structure is filled.
*/
struct NADCON_DATA{
	FILE *fp_las;
   FILE *fp_los;
	char filename[512];
	double xmin;	/* Minimum lon value (positive east) */
	double xmax;	/* Maximum lon value (positive east) */
	double ymin;	/* Minimum lat value (positive north) */
	double ymax;	/* Maximum lat value (positive north) */
	double dx;		/* Spacing between columns (degrees lon) */
	double dy;		/* Spacing between rows (degrees lat) */
	long nc;		/* Number of columns in data file */
	long nr;		/* Number of rows in data file */
	long nz;		/* Number of z's in data file (usually 1) */
	long lrecl;		/* Data file record length (from original FORTRAN) */
};

int coeff(double tee1, double tee2, double tee3, double tee4,
				double *ay, double *bee, double *cee, double *dee);

int surf(double xgrid, double ygrid, double *zee,
			double ay, double bee, double cee, double dee, long irow, long jcol);

struct NADCON_DATA *get_standard_nadcon_area_ptr(double x, double y);

int fgrid_and_intrp(struct NADCON_DATA *nadcon_data,
							double *xin, double *yin,
                     double *xout, double *yout,
                     double *dlas, double *dlos);

int iterative_loop_solve(struct NADCON_DATA *nadcon_data,
									double *xin, double *yin,
      		               double *xout, double *yout,
            		         double *dlas, double *dlos);

int latlon_to_meters(double *lat1, double *lon1, double *lat2, double *lon2
						,double *latm, double *lonm);

int load_nadcon_area(struct NADCON_DATA *nadcon_data, char *base_laslos);

void describe_nadcon_data_struct(struct NADCON_DATA *nadcon_data, char *desc);
int initialize_nadcon_internal(char *nadcon_file_path, CC_MAXFILELIST nadcon_areas);

struct NADCON_DATA nad83_to_hpgn_nadcon_data = {NULL,NULL,"",1.0,-1.0,1.0,-1.0,0.0,0.0,0,0,0,0};
struct NADCON_DATA hpgn_to_nad83_nadcon_data = {NULL,NULL,"",1.0,-1.0,1.0,-1.0,0.0,0.0,0,0,0,0};
struct NADCON_DATA g_standard_nadcon_data[MAXNADCONAREAS];

int g_standard_nadcon_area_count;

/*********************************************************************
*  initialize_nadcon()
*
*  Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_nadcon(char *nadcon_file_path)
{
CC_MAXFILELIST nadcon_areas = {"conus","prvi","hawaii","stlrnc","stgeorge","stpaul","alaska"};

	g_standard_nadcon_area_count = 7;

	return initialize_nadcon_internal(nadcon_file_path, nadcon_areas);
}

/*********************************************************************
*  initialize_nadcon_from_file()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_nadcon_from_file(char *nadcon_file_path, char *area_filename)
{
CC_MAXFILELIST nadcon_areas;
int rc;

	rc = read_cc_area_file(area_filename, nadcon_areas);
   if(rc < 0) return rc;

	g_standard_nadcon_area_count = rc;

	return initialize_nadcon_internal(nadcon_file_path,nadcon_areas);
}


/*********************************************************************
*  initialize_nadcon()
*
*  Need to document.
**********************************************************************/
int initialize_nadcon_internal(char *nadcon_file_path, CC_MAXFILELIST nadcon_areas)
{
struct NADCON_DATA empty_nadcon_data = {NULL,NULL,"",1.0,-1.0,1.0,-1.0,0.0,0.0,0,0,0,0};
char base_file[MAXPATH];
int i,rc;
int load_count=0;
char drive[MAXDRIVE];
char directory[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];

   if(g_standard_nadcon_area_count == NULL) return NULL_STD_NADCON_AREA_COUNT;
   if(g_standard_nadcon_area_count < 1) return STD_NADCON_AREA_COUNT_ZERO_OR_LESS;

   if(nadcon_file_path[strlen(nadcon_file_path) - 1] != '\\')
   	strcat(nadcon_file_path,"\\");

	clean_up_nadcon();

   for(i=0;i<g_standard_nadcon_area_count;i++)
   {
		strcpy(drive,"");
	   fnsplit(nadcon_areas[i],drive,directory,name,ext);

      if(strcmp(drive,"")) strcpy(base_file,nadcon_areas[i]);
      else sprintf(base_file,"%s%s",nadcon_file_path,nadcon_areas[i]);

		rc = load_nadcon_area(&g_standard_nadcon_data[i],base_file);
      if(rc != CC_SUCCESS) g_standard_nadcon_data[i] = empty_nadcon_data;
      else load_count++;
   }

   return load_count;
}


DLLEXPORT int _stdcall clean_up_nadcon(void)
{
int i;

	for(i=0;i<g_standard_nadcon_area_count;i++)
   {
   	fclose(g_standard_nadcon_data[i].fp_las);
   	fclose(g_standard_nadcon_data[i].fp_los);
      g_standard_nadcon_data[i].fp_las = NULL;
      g_standard_nadcon_data[i].fp_los = NULL;
   }

   return CC_SUCCESS;
}


DLLEXPORT int _stdcall clean_up_nad83_to_hpgn(void)
{
  	fclose(nad83_to_hpgn_nadcon_data.fp_las);
  	fclose(nad83_to_hpgn_nadcon_data.fp_los);
   nad83_to_hpgn_nadcon_data.fp_las = NULL;
   nad83_to_hpgn_nadcon_data.fp_los = NULL;

   return CC_SUCCESS;
}


DLLEXPORT int _stdcall clean_up_hpgn_to_nad83(void)
{
  	fclose(hpgn_to_nad83_nadcon_data.fp_las);
  	fclose(hpgn_to_nad83_nadcon_data.fp_los);
   hpgn_to_nad83_nadcon_data.fp_las = NULL;
   hpgn_to_nad83_nadcon_data.fp_los = NULL;

   return CC_SUCCESS;
}


/*********************************************************************
*  nad27_to_nad83()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall nad27_to_nad83(double *xin, double *yin, double *xout, double *yout,
							double *dlas, double *dlos, double *dlam, double *dlom)
{
struct NADCON_DATA *nadcon_data;
int rc;

	nadcon_data = get_standard_nadcon_area_ptr(*xin,*yin);
   if(!nadcon_data) return NADCON_OB;


   rc = fgrid_and_intrp(nadcon_data,xin,yin,xout,yout,dlas,dlos);
   if(rc != CC_SUCCESS) return rc;

	latlon_to_meters(yin, xin, yout, xout, dlam, dlom);

   return CC_SUCCESS;
}



/*********************************************************************
*  nad83_to_nad27()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall nad83_to_nad27(double *xin, double *yin, double *xout, double *yout,
							double *dlas, double *dlos, double *dlam, double *dlom)
{
struct NADCON_DATA *nadcon_data;
int rc;

	nadcon_data = get_standard_nadcon_area_ptr(*xin,*yin);
   if(!nadcon_data) return NADCON_OB;

   rc = iterative_loop_solve(nadcon_data,xin,yin,xout,yout,dlas,dlos);
   if(rc != CC_SUCCESS) return rc;

	latlon_to_meters(yin, xin, yout, xout, dlam, dlom);

   return CC_SUCCESS;
}



/*********************************************************************
*  initialize_nad83_to_hpgn()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_nad83_to_hpgn(char *base_laslos)
{
	return load_nadcon_area(&nad83_to_hpgn_nadcon_data, base_laslos);
}



/*********************************************************************
*  nad83_to_hpgn()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall nad83_to_hpgn(double *xin, double *yin, double *xout, double *yout,
							double *dlas, double *dlos, double *dlam, double *dlom)
{
int rc;

   rc = fgrid_and_intrp(&nad83_to_hpgn_nadcon_data,xin,yin,xout,yout,dlas,dlos);
   if(rc != CC_SUCCESS) return rc;

	latlon_to_meters(yin, xin, yout, xout, dlam, dlom);

   return CC_SUCCESS;
}



/*********************************************************************
*  initialize_hpgn_to_nad83()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_hpgn_to_nad83(char *base_laslos)
{
	return load_nadcon_area(&hpgn_to_nad83_nadcon_data, base_laslos);
}



/*********************************************************************
*  hpgn_to_nad83()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall hpgn_to_nad83(double *xin, double *yin, double *xout, double *yout,
							double *dlas, double *dlos, double *dlam, double *dlom)
{
int rc;

   rc = iterative_loop_solve(&hpgn_to_nad83_nadcon_data,xin,yin,xout,yout,dlas,dlos);
   if(rc != CC_SUCCESS) return rc;

	latlon_to_meters(yin, xin, yout, xout, dlam, dlom);

   return CC_SUCCESS;
}



/*********************************************************************
*  get_standard_nadcon_area_ptr()
*
*	Need to document.
**********************************************************************/
struct NADCON_DATA *get_standard_nadcon_area_ptr(double x, double y)
{
int i;

	if(g_standard_nadcon_area_count == NULL) return NULL;

   for(i=0;i<g_standard_nadcon_area_count;i++)
   {
   	if(!g_standard_nadcon_data[i].fp_las) continue;
   	if(!g_standard_nadcon_data[i].fp_los) continue;
   	if(g_standard_nadcon_data[i].xmin >= x) continue;
   	if(g_standard_nadcon_data[i].xmax <= x) continue;
   	if(g_standard_nadcon_data[i].ymin >= y) continue;
   	if(g_standard_nadcon_data[i].ymax <= y) continue;
      return &g_standard_nadcon_data[i];
   }

   return NULL;
}



/*********************************************************************
*  fgrid_and_intrp()
*
*	Need to document.
**********************************************************************/
int fgrid_and_intrp(struct NADCON_DATA *nadcon_data,
							double *xpt1, double *ypt1,
                     double *xpt2, double *ypt2,
                     double *dlas, double *dlos)
{
double xgrid,ygrid;
long irow,jcol;
double ay1,bee1,cee1,dee1;
double ay2,bee2,cee2,dee2;
double tee1, tee2, tee3, tee4, zee;
float ftemp;
long offset;

	if(nadcon_data == NULL) return NULL_NADCON_DATA_STRUCT;
	if(xpt1 == NULL) return NULL_XPT1;
	if(ypt1 == NULL) return NULL_YPT1;
	if(xpt2 == NULL) return NULL_XPT2;
	if(ypt2 == NULL) return NULL_YPT2;
	if(dlas == NULL) return NULL_DLAS;
	if(dlos == NULL) return NULL_DLOS;

	if(!nadcon_data->fp_las) return NULL_FP_LAS;
	if(!nadcon_data->fp_los) return NULL_FP_LOS;
 	if(nadcon_data->xmin >= *xpt1) return NADCON_OB;
 	if(nadcon_data->xmax <= *xpt1) return NADCON_OB;
 	if(nadcon_data->ymin >= *ypt1) return NADCON_OB;
 	if(nadcon_data->ymax <= *ypt1) return NADCON_OB;

	xgrid = (*xpt1 - nadcon_data->xmin) / nadcon_data->dx + 1.0;
	ygrid = (*ypt1 - nadcon_data->ymin) / nadcon_data->dy + 1.0;
	irow = (long) ygrid;
	jcol = (long) xgrid;

								/*
											Calculate offset into binary
												file using row, column, and
												record length(lrecl).
								*/
	offset = irow * nadcon_data->lrecl + 4*jcol;

	fseek(nadcon_data->fp_las,offset,SEEK_SET);
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_las); tee1 = ftemp;
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_las); tee3 = ftemp;

								/*
											Go forward 1 record length less
												8 bytes to get next values.  The
												minus 8 is required because two
												numbers were read above.
								*/
	fseek(nadcon_data->fp_las,nadcon_data->lrecl-8,SEEK_CUR);
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_las); tee2 = ftemp;
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_las); tee4 = ftemp;

	coeff(tee1,tee2,tee3,tee4,&ay1,&bee1,&cee1,&dee1);
	surf(xgrid, ygrid, &zee ,ay1, bee1, cee1, dee1, irow, jcol);
	*dlas = zee;

								/*
											Calculate offset into binary
												file using row, column, and
												record length(lrecl).
								*/
	offset = irow * nadcon_data->lrecl + 4*jcol;

	fseek(nadcon_data->fp_los,offset,SEEK_SET);
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_los); tee1 = ftemp;
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_los); tee3 = ftemp;

								/*
											Go forward 1 record length less
												8 bytes to get next values.  The
												minus 8 is required because two
												numbers were read above.
								*/
	fseek(nadcon_data->fp_los,nadcon_data->lrecl-8,SEEK_CUR);
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_los); tee2 = ftemp;
	fread(&ftemp, sizeof(float),1,nadcon_data->fp_los); tee4 = ftemp;

	coeff(tee1,tee2,tee3,tee4,&ay2,&bee2,&cee2,&dee2);
	surf(xgrid, ygrid, &zee ,ay2, bee2, cee2, dee2, irow, jcol);
	*dlos = zee;

	*ypt2 = *ypt1 + (*dlas)/3600.0;
	*xpt2 = *xpt1 - (*dlos)/3600.0;

   return CC_SUCCESS;
}



/*********************************************************************
*  iterative_loop_solve()
*
*	Need to document.
**********************************************************************/
int iterative_loop_solve(struct NADCON_DATA *nadcon_data,
									double *xin, double *yin,
      		               double *xout, double *yout,
            		         double *dlas, double *dlos)
{
int i,itmax=10;
int rc;
double xtemp=0.0;
double ytemp=0.0;
double xdif;
double ydif;
double small=1.0e-9;
double xpt1;
double ypt1;
double xpt2;
double ypt2;

	if(nadcon_data == NULL) return NULL_NADCON_DATA_STRUCT;
	if(xin == NULL) return NULL_XPT1;
	if(yin == NULL) return NULL_YPT1;
	if(xout == NULL) return NULL_XPT2;
	if(yout == NULL) return NULL_YPT2;
	if(dlas == NULL) return NULL_DLAS;
	if(dlos == NULL) return NULL_DLOS;

	xtemp = xpt1 = xpt2 = *xin;
	ytemp = ypt1 = ypt2 = *yin;

	for(i=0;i<itmax;i++)
	{
		rc = fgrid_and_intrp(nadcon_data,&xtemp,&ytemp,&xpt2,&ypt2,dlas,dlos);
      if(rc != CC_SUCCESS) return rc;

		xdif = xpt1 - xpt2;
      ydif = ypt1 - ypt2;

      if(i==0){
			if(fabs(xdif) > small) xtemp = xpt1 - *dlos / 3600.0;
			if(fabs(ydif) > small) ytemp = ypt1 - *dlas / 3600.0;
		}
      else{
			if(fabs(xdif) > small) xtemp = xtemp - (xpt2 - xpt1);
			if(fabs(ydif) > small) ytemp = ytemp - (ypt2 - ypt1);
		}

      if(fabs(ydif) <= small && fabs(xdif) <= small){
      	*xout = xtemp;
			*yout = ytemp;

			return CC_SUCCESS;
      }
	}

   return NADCON_ITERATIONS_EXCEEDED;
}



/*********************************************************************
*  load_nadcon_area()
*
*	Need to document.
**********************************************************************/
int load_nadcon_area(struct NADCON_DATA *nadcon_data, char *base_laslos)
{
char las_file[MAXPATH];
char los_file[MAXPATH];
char drive[MAXDRIVE];
char directory[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];
struct NADCON_HEADER las_header;
struct NADCON_HEADER los_header;
FILE *fp_las;
FILE *fp_los;

   fnsplit(base_laslos,drive,directory,name,ext);
	fnmerge(las_file,drive,directory,name,"las");
	fnmerge(los_file,drive,directory,name,"los");

   fp_las = fopen(las_file,"rb");
   fp_los = fopen(los_file,"rb");

   if(!fp_las || !fp_los){
   	fclose(fp_las);
      fclose(fp_los);
      return CANNOT_OPEN_LASLOS_FILES;
   }

   fread(&las_header,sizeof(struct NADCON_HEADER),1,fp_las);
   fread(&los_header,sizeof(struct NADCON_HEADER),1,fp_los);
   fclose(fp_las);
   fclose(fp_los);

   if(las_header.nc != los_header.nc) return LASLOS_HEADER_NC_UNEQUAL;
   if(las_header.nr != los_header.nr) return LASLOS_HEADER_NR_UNEQUAL;
   if(las_header.xo != los_header.xo) return LASLOS_HEADER_XO_UNEQUAL;
   if(las_header.dx != los_header.dx) return LASLOS_HEADER_DX_UNEQUAL;
   if(las_header.yo != los_header.yo) return LASLOS_HEADER_YO_UNEQUAL;
   if(las_header.dy != los_header.dy) return LASLOS_HEADER_DY_UNEQUAL;

   nadcon_data->fp_las = fopen(las_file,"rb");
   nadcon_data->fp_los = fopen(los_file,"rb");

   if(!nadcon_data->fp_las || !nadcon_data->fp_los)
   {
   	fclose(nadcon_data->fp_las);
   	fclose(nadcon_data->fp_los);
	   nadcon_data->fp_las = NULL;
   	nadcon_data->fp_los = NULL;
   }
   strcpy(nadcon_data->filename,base_laslos);
   nadcon_data->nc = las_header.nc;
   nadcon_data->nr = las_header.nr;
   nadcon_data->nz = las_header.nz;
   nadcon_data->dx = (double) las_header.dx;
   nadcon_data->dy = (double) las_header.dy;
   nadcon_data->xmin = (double) las_header.xo;
	nadcon_data->xmax = (double) las_header.xo + (double) (las_header.nc - 1) * (double) las_header.dx;
   nadcon_data->ymin = (double) las_header.yo;
	nadcon_data->ymax = (double) las_header.yo + (double) (las_header.nr - 1) * (double) las_header.dy;
	nadcon_data->lrecl = 4 * (las_header.nc + 1);

   return CC_SUCCESS;
}


/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_standard_nadcon_areas(char *desc)
{
int i;

 	if(g_standard_nadcon_area_count < 1) return STD_NADCON_AREA_COUNT_ZERO_OR_LESS;

	strcpy(desc,"");

	for(i=0;i<g_standard_nadcon_area_count;i++)
   	describe_nadcon_data_struct(&g_standard_nadcon_data[i], desc);

   return CC_SUCCESS;
}



/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_nad83_to_hpgn_nadcon_area(char *desc)
{
	strcpy(desc,"");

  	describe_nadcon_data_struct(&nad83_to_hpgn_nadcon_data, desc);

   return CC_SUCCESS;
}



/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_hpgn_to_nad83_nadcon_area(char *desc)
{
	strcpy(desc,"");

  	describe_nadcon_data_struct(&hpgn_to_nad83_nadcon_data, desc);

   return CC_SUCCESS;
}



/*********************************************************************
*  describe_nadcon_data_struct()
*
*	Need to document.
**********************************************************************/
void describe_nadcon_data_struct(struct NADCON_DATA *nadcon_data, char *desc)
{
char temp[256];

	if(!nadcon_data) return;

   sprintf(temp,"filename = %s\n",nadcon_data->filename);
   strcat(desc,temp);
   sprintf(temp,"nc = %d\n",nadcon_data->nc);
   strcat(desc,temp);
   sprintf(temp,"nr = %d\n",nadcon_data->nr);
   strcat(desc,temp);
   sprintf(temp,"xmin = %lf\n",nadcon_data->xmin);
   strcat(desc,temp);
   sprintf(temp,"xmax = %lf\n",nadcon_data->xmax);
   strcat(desc,temp);
   sprintf(temp,"dx   = %lf\n",nadcon_data->dx);
   strcat(desc,temp);
   sprintf(temp,"ymin = %lf\n",nadcon_data->ymin);
   strcat(desc,temp);
   sprintf(temp,"ymax = %lf\n",nadcon_data->ymax);
   strcat(desc,temp);
   sprintf(temp,"dy   = %lf\n",nadcon_data->dy);
   strcat(desc,temp);
}


/*********************************************************************
*	latlon_to_meters() converts the difference between two lat/lon
*	 positions to meters.
*
*	 Notes From NADCON
*	 This method uses ellipsoidal rather than spherical parameters.
*	 Original code came from Ed McKay.  The reference used by Ed
*	 McKay was:
*		'A Course in Higher Geodesy' by P.W. Zakatov, Isreal
*		  Program for Scientific Translations, Jerusalem, 1962
*
*	 This subroutine was set up for positive west longitude.
**********************************************************************/
int latlon_to_meters(double *lat1, double *lon1, double *lat2, double *lon2
						,double *latm, double *lonm)
{
/* GRS 80 Parameters? */
double axis = 6378137.0;
double e2 = 0.0066943800229;
double rhosec = 206264.806247;
double w, lm, lp, avlat;
double lat1s, lat2s, lon1s, lon2s, dlat, dlon;

									/*
                                    	Convert decimal degrees to
                                        decimal seconds (positive
                                        west longitude)
                                    */
	lat1s = (*lat1) * 60.0 * 60.0 / rhosec;
    lon1s = -(*lon1) * 60.0 * 60.0 / rhosec;
	 lat2s = (*lat2) * 60.0 * 60.0 / rhosec;
    lon2s = -(*lon2) * 60.0 * 60.0 /rhosec;

    dlat = (lat2s - lat1s) * rhosec;
	 dlon = (lon2s - lon1s) * rhosec;

	 avlat = (lat1s + lat2s) / 2.0;

	 w = sqrt(1.0 - e2 * sin(avlat) * sin(avlat));
	 lm = axis * (1.0 - e2) / (w*w*w * rhosec);
	 lp = axis * cos(avlat) / (w * rhosec);

	 *latm = lm * dlat;
	 *lonm = lp * dlon;

	 return CC_SUCCESS;
}

/*********************************************************************
*	coeff() is a direct conversion from the NADCON routine COEFF.
*    coeff() generates the coefficients for the surface
*    function surf().  coeff() is also used in VERTCON.
**********************************************************************/
int coeff(double tee1, double tee2, double tee3, double tee4
			,double *ay, double *bee, double *cee, double *dee)
{
	*ay = tee1;
    *bee = tee3 - tee1;
    *cee = tee2 - tee1;
    *dee = tee4 - tee3 - tee2 + tee1;

    return CC_SUCCESS;
}

/*********************************************************************
*	surf() is a direct conversion from the NADCON routine SURF.
*    It does interpolation, though I'm not sure exactly how it
*    does it.  surf() is also used in VERTCON.
**********************************************************************/
int surf(double xgrid, double ygrid, double *zee
			,double ay, double bee, double cee, double dee
				,long irow, long jcol)
{
double zee1,zee2,zee3,zee4;

	zee1 = ay;
	 zee2 = bee*(xgrid - (double) jcol);
	 zee3 = cee*(ygrid - (double) irow);
    zee4 = dee*(xgrid - (double) jcol)*(ygrid - (double) irow);

    *zee = zee1 + zee2 + zee3 + zee4;

    return CC_SUCCESS;
}

