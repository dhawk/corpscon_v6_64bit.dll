#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <dir.h>
#include "ccvert.h"
#include "ccfileutil.h"
#include "ccerror.h"

/*
	VERTCON_HEADER can be read directly from the beginning of the
    .94 files.  xo and yo combine to give the south-west
    (lower-left) corner of the data file.  Refer to
    open_nadcon_data_file() to see how to read las/los files.
*/
struct VERTCON_HEADER{
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
	VERTCON_DATA hold all the important information about each
    .94 file.  Each .las and each .los file will have a
    data_file_info structure assocated with it.
*/
struct VERTCON_DATA{
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

struct VERTCON_DATA *get_vertcon_area_ptr(double x, double y);
struct VERTCON_DATA g_vertcon_data[MAXVERTCONAREAS];
int g_vertcon_area_count;

void describe_vertcon_data_struct(struct VERTCON_DATA *vertcon_data, char *desc);
int initialize_vertcon_internal(char *vertcon_file_path, CC_MAXFILELIST vertcon_files );
float vertcon_round(float value);

/*********************************************************************
*  initialize_vertcon()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_vertcon(char *vertcon_file_path)
{
CC_MAXFILELIST vertcon_files = {"vertcone.94","vertconc.94","vertconw.94"};

	g_vertcon_area_count = 3;

	return initialize_vertcon_internal(vertcon_file_path,vertcon_files);
}


/*********************************************************************
*  initialize_vertcon_from_file()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_vertcon_from_file(char *vertcon_file_path, char *area_filename)
{
CC_MAXFILELIST vertcon_files;
int rc;

	rc = read_cc_area_file(area_filename, vertcon_files);
   if(rc < 0) return rc;

   g_vertcon_area_count = rc;

	return initialize_vertcon_internal(vertcon_file_path,vertcon_files);
}


/*********************************************************************
*  initialize_vertcon_internal()
*
*	Need to document.
**********************************************************************/
int initialize_vertcon_internal(char *vertcon_file_path, CC_MAXFILELIST vertcon_files)
{
struct VERTCON_HEADER vertcon_header;
int i;
int count=0;
char drive[MAXDRIVE];
char directory[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];

   if(g_vertcon_area_count == NULL) return NULL_VERTCON_AREA_COUNT;
   if(g_vertcon_area_count < 1) return VERTCON_AREA_COUNT_ZERO_OR_LESS;

   if(vertcon_file_path[strlen(vertcon_file_path) - 1] != '\\')
   	strcat(vertcon_file_path,"\\");

   clean_up_vertcon();

	for(i=0;i<g_vertcon_area_count;i++)
   {
		strcpy(drive,"");
	   fnsplit(vertcon_files[i],drive,directory,name,ext);

      if(strcmp(drive,"")) strcpy(g_vertcon_data[i].filename,vertcon_files[i]);
      else sprintf(g_vertcon_data[i].filename,"%s%s",vertcon_file_path,vertcon_files[i]);

		g_vertcon_data[i].fp = fopen(g_vertcon_data[i].filename,"rb");
      if(!g_vertcon_data[i].fp) continue;

		fread(&vertcon_header, sizeof(struct VERTCON_HEADER),1,g_vertcon_data[i].fp);
										/*
													Copy nc, nr, and nz from
													 nadcon_header structure to
													 data_file_info structure.
										*/
		g_vertcon_data[i].nc = vertcon_header.nc;
		g_vertcon_data[i].nr = vertcon_header.nr;
		g_vertcon_data[i].nz = vertcon_header.nz;
										/*
													Find minumum and maximum
													 x and y values.
										*/
		g_vertcon_data[i].xmin = vertcon_header.xo;
		g_vertcon_data[i].xmax = vertcon_header.xo
										+ (vertcon_header.nc - 1)
										* vertcon_header.dx - 1.0;
		g_vertcon_data[i].ymin = vertcon_header.yo;
		g_vertcon_data[i].ymax = vertcon_header.yo
										+ (vertcon_header.nr - 1)
										* vertcon_header.dy;
										/*
													Copy delta values.
										*/
		g_vertcon_data[i].dx = fabs(vertcon_header.dx);
		g_vertcon_data[i].dy = fabs(vertcon_header.dy);
										/*
													Calculate record length.
													 This is important.  It is used
													 later to calculate offsets into
													 the data file.
										*/
		g_vertcon_data[i].lrecl = 4 * (vertcon_header.nc + 1);
      count++;
   }

   return count;
}


/*********************************************************************
*  vertcon()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall vertcon(double x, double y, double *ght)
{
static struct VERTCON_DATA *vertcon_data=NULL;
int irow,jcol;
double xgrid,ygrid;
static FILE *last_fp = NULL;
static long last_jcol = -1;
static long last_irow = -1;
static double ay1,bee1,cee1,dee1;
double tee1, tee2, tee3, tee4, zee;
float ftemp=0.0;
long offset;
double zee1,zee2,zee3,zee4;
float fght;

	if(ght == NULL) return NULL_VERTCON_GHT;

   *ght = -999.0;

  	vertcon_data = get_vertcon_area_ptr(x,y);
   if(!vertcon_data) return VERTCON_OB;

	xgrid = (x - vertcon_data->xmin) / vertcon_data->dx + 1.0;
   ygrid = (y - vertcon_data->ymin) / vertcon_data->dy + 1.0;
	irow = (long) ygrid;
   jcol = (long) xgrid;

	if(irow != last_irow || jcol != last_jcol || !last_fp || vertcon_data->fp != last_fp)
	{
								/*
											Get Latitude Values
								*/
								/*
											Calculate offset into binary
												file using row, column, and
												record length(lrecl).
								*/
		offset = irow * vertcon_data->lrecl + 4*jcol;
		fseek(vertcon_data->fp, offset, SEEK_SET);
		fread(&ftemp, sizeof(float),1,vertcon_data->fp); tee1 = ftemp;
		fread(&ftemp, sizeof(float),1,vertcon_data->fp); tee3 = ftemp;

								/*
											Go forward 1 record length less
												8 bytes to get next values.  The
												minus 8 is required because two
												numbers were read above.
								*/
		fseek(vertcon_data->fp,vertcon_data->lrecl-8,SEEK_CUR);
		fread(&ftemp, sizeof(float),1,vertcon_data->fp); tee2 = ftemp;
		fread(&ftemp, sizeof(float),1,vertcon_data->fp); tee4 = ftemp;

								/*
											Calculate coefficients.
								*/
		/* coeff() calculations */
      ay1 = tee1;
		bee1 = tee3 - tee1;
		cee1 = tee2 - tee1;
		dee1 = tee4 - tee3 - tee2 + tee1;

	}

	/* surf() calculations */
	zee1 = ay1;

	zee2 = bee1*(xgrid - (double) jcol);
	zee3 = cee1*(ygrid - (double) irow);
	zee4 = dee1*(xgrid - (double) jcol)*(ygrid - (double) irow);

	zee = zee1 + zee2 + zee3 + zee4;

								/*
											Set ght.
								*/
	fght = zee*0.001;
   fght = vertcon_round(fght);
   *ght = fght;

								/*
											Set check for last read.
								*/
	last_irow = irow;
	last_jcol = jcol;
   last_fp = vertcon_data->fp;

   return CC_SUCCESS;
}

/*********************************************************************
*  vertcon_round()
*
*	This routine mimics the ROUND routine in NGS version of VERTCON
*  in FORTRAN.  I don't think it's needed, but it is include for
*  consistency.
**********************************************************************/
float vertcon_round(float value)
{
float sign;
int intht;

	sign = 1.0;
	if(value < 0.0) sign = -1.0;
   intht = sign * (fabs(value) + 0.0005) * 1000.0;

   return intht * 0.001;
}


/*********************************************************************
*  clean_up_vertcon()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall clean_up_vertcon(void)
{
int i;

	for(i=0;i<g_vertcon_area_count;i++)
   {
   		fclose(g_vertcon_data[i].fp);
         g_vertcon_data[i].fp = NULL;
   }

	return CC_SUCCESS;
}


/*********************************************************************
*  get_vertcon_area_ptr()
*
*	Need to document.
**********************************************************************/
struct VERTCON_DATA *get_vertcon_area_ptr(double x, double y)
{
int i;

	for(i=0;i<g_vertcon_area_count;i++)
   {
   	if(!g_vertcon_data[i].fp) continue;
      if(g_vertcon_data[i].xmin >= x) continue;
      if(g_vertcon_data[i].xmax <= x) continue;
      if(g_vertcon_data[i].ymin >= y) continue;
      if(g_vertcon_data[i].ymax <= y) continue;

      return &g_vertcon_data[i];
   }

   return NULL;
}


/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_vertcon_areas(char *desc)
{
int i;

	if(g_vertcon_area_count < 1) return VERTCON_AREA_COUNT_ZERO_OR_LESS;

	strcpy(desc,"");

	for(i=0;i<g_vertcon_area_count;i++)
   	describe_vertcon_data_struct(&g_vertcon_data[i], desc);

   return CC_SUCCESS;
}



/*********************************************************************
*  describe_vertcon_data_struct()
*
*	Need to document.
**********************************************************************/
void describe_vertcon_data_struct(struct VERTCON_DATA *vertcon_data, char *desc)
{
char temp[256];

	if(!vertcon_data) return;

   sprintf(temp,"filename = %s\n",vertcon_data->filename);
   strcat(desc,temp);
   sprintf(temp,"fp = %d\n",vertcon_data->fp);
   strcat(desc,temp);
   sprintf(temp,"nc = %d\n",vertcon_data->nc);
   strcat(desc,temp);
   sprintf(temp,"nr = %d\n",vertcon_data->nr);
   strcat(desc,temp);
   sprintf(temp,"xmin = %lf\n",vertcon_data->xmin);
   strcat(desc,temp);
   sprintf(temp,"xmax = %lf\n",vertcon_data->xmax);
   strcat(desc,temp);
   sprintf(temp,"dx   = %lf\n",vertcon_data->dx);
   strcat(desc,temp);
   sprintf(temp,"ymin = %lf\n",vertcon_data->ymin);
   strcat(desc,temp);
   sprintf(temp,"ymax = %lf\n",vertcon_data->ymax);
   strcat(desc,temp);
   sprintf(temp,"dy   = %lf\n",vertcon_data->dy);
   strcat(desc,temp);
}

