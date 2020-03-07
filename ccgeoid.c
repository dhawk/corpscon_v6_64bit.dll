#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <math.h>
#include <dir.h>
#include "ccgeoid.h"
#include "ccfileutil.h"
#include "ccerror.h"

void describe_geoid_data_struct(struct GEOID_DATA *geoid_data, char *desc);

struct GEOID_DATA *get_geoid_area_ptr(double x, double y);

int geoid_intrp(struct GEOID_DATA *geoid_data, double xpt, double ypt, float *ght);
float qterp2(float x, float f0, float f1, float f2);
int initialize_geoid(char *geoid_file_path, CC_MAXFILELIST geoid_files);

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

struct GEOID_DATA g_geoid_data[MAXGEOIDAREAS];
int g_geoid_area_count;



/*********************************************************************
*  initialize_geoid90()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid90(char *geoid_file_path)
{
CC_MAXFILELIST geoid_files = {"geoid90e.geo","geoid90c.geo",
													"geoid90w.geo"};

	g_geoid_area_count = 3;

	return initialize_geoid(geoid_file_path,geoid_files);
}


/*********************************************************************
*  initialize_geoid93()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid93(char *geoid_file_path)
{
CC_MAXFILELIST geoid_files = {"geoid93e.geo","geoid93c.geo",
										"geoid93w.geo","ala94.geo",
                              "haw93.geo","prvi93.geo"};

	g_geoid_area_count = 6;

	return initialize_geoid(geoid_file_path,geoid_files);
}


/*********************************************************************
*  initialize_geoid96()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid96(char *geoid_file_path)
{
CC_MAXFILELIST geoid_files = {"geo96nw.geo","geo96nc.geo",
										"geo96ne.geo","geo96sw.geo",
                              "geo96sc.geo","geo96se.geo",
                              "geo96an.geo","geo96as.geo",
                              "geo96hw.geo","geo96pr.geo"};

	g_geoid_area_count = 10;

	return initialize_geoid(geoid_file_path,geoid_files);
}


/*********************************************************************
*  initialize_geoid_from_file()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid_from_file(char *geoid_file_path, char *area_filename)
{
CC_MAXFILELIST geoid_files;
int rc;

	rc = read_cc_area_file(area_filename, geoid_files);

   if(rc < 0) return rc;

	g_geoid_area_count = rc;

	return initialize_geoid(geoid_file_path,geoid_files);
}


/*********************************************************************
*  initialize_geoid()
*
*	Need to document.
**********************************************************************/
int initialize_geoid(char *geoid_file_path, CC_MAXFILELIST geoid_files)
{
struct GEOID_HEADER geoid_header;
char drive[MAXDRIVE];
char directory[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];
int i;
int count;
double idx1,idy1;

   if(g_geoid_area_count == NULL) return NULL_GEOID_AREA_COUNT;
   if(g_geoid_area_count < 1) return GEOID_AREA_COUNT_ZERO_OR_LESS;

   if(geoid_file_path[strlen(geoid_file_path) - 1] != '\\')
   	strcat(geoid_file_path,"\\");

   clean_up_geoid();

	for(i=0;i<g_geoid_area_count;i++)
   {
		strcpy(drive,"");
	   fnsplit(geoid_files[i],drive,directory,name,ext);

      if(strcmp(drive,"")) strcpy(g_geoid_data[i].filename,geoid_files[i]);
      else sprintf(g_geoid_data[i].filename,"%s%s",geoid_file_path,geoid_files[i]);

		g_geoid_data[i].fp = fopen(g_geoid_data[i].filename,"rb");
      if(g_geoid_data[i].fp == NULL) continue;

		fread(&geoid_header,sizeof(struct GEOID_HEADER),1,g_geoid_data[i].fp);

		g_geoid_data[i].nc = geoid_header.nc;
		g_geoid_data[i].nr = geoid_header.nr;
		g_geoid_data[i].nz = geoid_header.nz;

									/*
													Find minumum and maximum
													 x and y values.
									*/

		g_geoid_data[i].xmin = geoid_header.xo;
		g_geoid_data[i].xmax = geoid_header.xo
									+ (geoid_header.nc - 1)
									* geoid_header.dx;
		g_geoid_data[i].ymin = geoid_header.yo;
		g_geoid_data[i].ymax = geoid_header.yo
									+ (geoid_header.nr - 1)
									* geoid_header.dy;

		if((float) modf(geoid_header.dx*3600.0,&idx1) >= 0.5) idx1+=1.0;
		g_geoid_data[i].dx = ((float) idx1) / 3600.0;

		if((float) modf(geoid_header.dy*3600.0,&idy1) >= 0.5) idy1+=1.0;
		g_geoid_data[i].dy = ((float) idy1) / 3600.0;


									/*
													Calculate record length.
													 This is important.  It is used
													 later to calculate offsets into
													 the data file.
									*/
		g_geoid_data[i].lrecl = 4 * (geoid_header.nc + 1);
      count++;
   }

   return count;
}


/*********************************************************************
*  clean_up_geoid()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall clean_up_geoid(void)
{
int i;

	for(i=0;i<MAXGEOIDAREAS;i++)
   {
   		fclose(g_geoid_data[i].fp);
   		g_geoid_data[i].fp = NULL;
	}

	return CC_SUCCESS;
}


/*********************************************************************
*  geoid()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall geoid(double x, double y, double *ght)
{
struct GEOID_DATA *geoid_data;
float fght;
int rc;

	*ght = -999.0;

  	geoid_data = get_geoid_area_ptr(x,y);
   if(!geoid_data) return GEOID_OB;

   rc = geoid_intrp(geoid_data, x, y, &fght);
   if(rc == CC_SUCCESS) *ght = fght;

	return rc;
}


/*********************************************************************
*  get_geoid_area_ptr()
*
*	Need to document.
**********************************************************************/
struct GEOID_DATA *get_geoid_area_ptr(double x, double y)
{
int i;

	for(i=0;i<g_geoid_area_count;i++)
   {
   	if(!g_geoid_data[i].fp) continue;
      if(g_geoid_data[i].xmin >= x) continue;
      if(g_geoid_data[i].xmax <= x) continue;
      if(g_geoid_data[i].ymin >= y) continue;
      if(g_geoid_data[i].ymax <= y) continue;

      return &g_geoid_data[i];
   }

   return NULL;
}


/*********************************************************************
*  geoid_intrp()
*
*	Modified from NGS FORTRAN code.  Need to document.
**********************************************************************/
int geoid_intrp(struct GEOID_DATA *geoid_data, double xpt, double ypt, float *ght)
{
static float tee1, tee2, tee3, tee4, tee5, tee6, tee7, tee8, tee9;
float fx0,fx1,fx2;
long offset;
float x,y;
int ix,ix1,ix2,jy,jy1,jy2;
static int last_irow,last_jcol;
static FILE *last_fp = NULL;

	if(!geoid_data) return NULL_GEOID_DATA_STRUCT;
   if(geoid_data->fp < 0) return BAD_GEOID_DATA_STRUCT_FP;

	ix = ((float) xpt - geoid_data->xmin) / geoid_data->dx + 1.0;
	ix1 = ix + 1;
	ix2 = ix + 2;

	while(ix2 > geoid_data->nc)
	{
		ix--;
		ix1--;
		ix2--;
	}

	x = ((float) xpt - geoid_data->dx * (ix - 1) - geoid_data->xmin) / geoid_data->dx;

	if(x < 0.5 && ix > 1)
	{
		ix--;
		ix1--;
		ix2--;
		x+=1.0;
	}

	jy = ((float) ypt - geoid_data->ymin) / geoid_data->dy + 1.0;
	jy1 = jy + 1;
	jy2 = jy + 2;

	while(jy2 > geoid_data->nr)
	{
		jy--;
		jy1--;
		jy2--;
	}

	y = ((float) ypt - geoid_data->dy * (jy - 1) - geoid_data->ymin) / geoid_data->dy;

	if(y < 0.5 && jy > 1)
	{
		jy--;
		jy1--;
		jy2--;
		y+=1.0;
	}

	if(ix!=last_irow || jy!=last_jcol || geoid_data->fp != last_fp)
	{
		offset = jy * geoid_data->lrecl + 4*ix;
		fseek(geoid_data->fp,offset,SEEK_SET);
		fread(&tee1, sizeof(float),1,geoid_data->fp);
		fread(&tee2, sizeof(float),1,geoid_data->fp);
		fread(&tee3, sizeof(float),1,geoid_data->fp);

								/*
											Go forward 1 record length less
												8 bytes to get next values.  The
												minus 8 is required because two
												numbers were read above.
										  */
		fseek(geoid_data->fp,geoid_data->lrecl-12,SEEK_CUR);
		fread(&tee4, sizeof(float),1,geoid_data->fp);
		fread(&tee5, sizeof(float),1,geoid_data->fp);
		fread(&tee6, sizeof(float),1,geoid_data->fp);

								/*
											Go forward 1 record length less
												8 bytes to get next values.  The
												minus 8 is required because two
												numbers were read above.
										  */
		fseek(geoid_data->fp,geoid_data->lrecl-12,SEEK_CUR);
		fread(&tee7, sizeof(float),1,geoid_data->fp);
		fread(&tee8, sizeof(float),1,geoid_data->fp);
		fread(&tee9, sizeof(float),1,geoid_data->fp);

	}

								/*
											Model surface.
								*/
	fx0 = qterp2(x, tee1, tee2, tee3);
	fx1 = qterp2(x, tee4, tee5, tee6);
	fx2 = qterp2(x, tee7, tee8, tee9);

	*ght = qterp2(y, fx0, fx1, fx2);

									/*
											Set check for last read.
									*/
	last_irow = ix;
	last_jcol = jy;
	last_fp = geoid_data->fp;

	return CC_SUCCESS;
}


/*********************************************************************
*  qterp2()
*
*	Modified from NGS FORTRAN code.  Need to document.
**********************************************************************/
float qterp2(float x, float f0, float f1, float f2)
{
float df0,df1,d2f0;

	df0 = f1 - f0;
	df1 = f2 - f1;
	d2f0 = df1 - df0;

	return (f0 + x * df0 + 0.5*x*(x - 1.0) * d2f0);
}


/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_geoid_areas(char *desc)
{
int i;

 	if(g_geoid_area_count < 1) return GEOID_AREA_COUNT_ZERO_OR_LESS;

	strcpy(desc,"");

	for(i=0;i<g_geoid_area_count;i++)
   	describe_geoid_data_struct(&g_geoid_data[i], desc);

   return CC_SUCCESS;
}


/*********************************************************************
*  describe_geoid_data_struct()
*
*	Need to document.
**********************************************************************/
void describe_geoid_data_struct(struct GEOID_DATA *geoid_data, char *desc)
{
char temp[256];

	if(!geoid_data) return;

   sprintf(temp,"filename = %s\n",geoid_data->filename);
   strcat(desc,temp);
   sprintf(temp,"fp = %d\n",geoid_data->fp);
   strcat(desc,temp);
   sprintf(temp,"nc = %d\n",geoid_data->nc);
   strcat(desc,temp);
   sprintf(temp,"nr = %d\n",geoid_data->nr);
   strcat(desc,temp);
   sprintf(temp,"xmin = %lf\n",geoid_data->xmin);
   strcat(desc,temp);
   sprintf(temp,"xmax = %lf\n",geoid_data->xmax);
   strcat(desc,temp);
   sprintf(temp,"dx   = %lf\n",geoid_data->dx);
   strcat(desc,temp);
   sprintf(temp,"ymin = %lf\n",geoid_data->ymin);
   strcat(desc,temp);
   sprintf(temp,"ymax = %lf\n",geoid_data->ymax);
   strcat(desc,temp);
   sprintf(temp,"dy   = %lf\n",geoid_data->dy);
   strcat(desc,temp);
}

