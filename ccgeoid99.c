#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dir.h>
#include "ccgeoid99.h"
#include "ccfileutil.h"
#include "ccerror.h"

struct GEOID99_HEADER{
	double glamn;
   double glomn;
   double dla;
   double dlo;
   long nla;
   long nlo;
   long ikind;
};

struct GEOID99_DATA{
	FILE *fp;
	char filename[MAXPATH];
   double glamn;
   double glomn;
   double glamx;
   double glomx;
   double dla;
   double dlo;
   long nla;
   long nlo;
};

void describe_geoid99_data_struct(struct GEOID99_DATA *geoid99_data, char *desc);
struct GEOID99_DATA *which1(double xlon, double ylat);

struct GEOID99_DATA g_geoid99_data[MAXGEOID99AREAS];
int g_geoid99_area_count;
int g_using_geoid99_default_areas;

int geoid99_interp(struct GEOID99_DATA *geoid99_data,
							double xlon, double ylat, double *ght);
int geoid99_nint(double val);
float qfit(float x, float f0, float f1, float f2);
int initialize_geoid99_internal(char *geoid99_file_path, CC_MAXFILELIST geoid_files);


/*********************************************************************
*  initialize_geoid99()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid99(char *geoid99_file_path)
{
CC_MAXFILELIST geoid99_files = {"g1999u01.bin","g1999u02.bin",
											"g1999u03.bin","g1999u04.bin",
                                 "g1999u05.bin","g1999u06.bin",
                                 "g1999u07.bin","g1999u08.bin",
                                 "g1999h01.bin","g1999p01.bin",
                                 "g1999a01.bin","g1999a02.bin",
                                 "g1999a03.bin","g1999a04.bin"};

	g_geoid99_area_count = 14;
	g_using_geoid99_default_areas = 1;

	return initialize_geoid99_internal(geoid99_file_path,geoid99_files);
}



/*********************************************************************
*  initialize_geoid03()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid03(char *geoid99_file_path)
{
CC_MAXFILELIST geoid03_files = {"g2003u01.bin","g2003u02.bin",
											"g2003u03.bin","g2003u04.bin",
                                 "g2003u05.bin","g2003u06.bin",
                                 "g2003u07.bin","g2003u08.bin"};

	g_geoid99_area_count = 8;
	g_using_geoid99_default_areas = 1;

	return initialize_geoid99_internal(geoid99_file_path,geoid03_files);
}



/*********************************************************************
*  initialize_geoid99_from_file()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall initialize_geoid99_from_file(char *geoid99_file_path, char *area_filename)
{
CC_MAXFILELIST geoid99_files;
int rc;

	rc = read_cc_area_file(area_filename, geoid99_files);
   if(rc < 0) return rc;
	g_geoid99_area_count = rc;
	g_using_geoid99_default_areas = 0;

	return initialize_geoid99_internal(geoid99_file_path,geoid99_files);
}



/*********************************************************************
*  initialize_geoid99_internal()
*
*	Need to document.
**********************************************************************/
int initialize_geoid99_internal(char *geoid99_file_path, CC_MAXFILELIST geoid99_files)
{
struct GEOID99_HEADER header;
int count=0;
int i;
char drive[MAXDRIVE];
char directory[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];

   if(g_geoid99_area_count == NULL) return NULL_GEOID99_AREA_COUNT;
   if(g_geoid99_area_count < 1) return GEOID99_AREA_COUNT_ZERO_OR_LESS;

   if(geoid99_file_path[strlen(geoid99_file_path) - 1] != '\\')
   		strcat(geoid99_file_path,"\\");
         
   clean_up_geoid99();

	for(i=0;i<g_geoid99_area_count;i++)
   {
		strcpy(drive,"");
	   fnsplit(geoid99_files[i],drive,directory,name,ext);
      
      if(strcmp(drive,"")) strcpy(g_geoid99_data[i].filename,geoid99_files[i]);
      else sprintf(g_geoid99_data[i].filename,"%s%s",geoid99_file_path,geoid99_files[i]);

		g_geoid99_data[i].fp = fopen(g_geoid99_data[i].filename,"rb");
      if(!g_geoid99_data[i].fp) continue;

		fread(&header, sizeof(struct GEOID99_HEADER),1,g_geoid99_data[i].fp);

		g_geoid99_data[i].nla = header.nla;
		g_geoid99_data[i].nlo = header.nlo;

		g_geoid99_data[i].glamn = header.glamn;
		g_geoid99_data[i].glamx = header.glamn + (double)(header.nla - 1) * header.dla;
		g_geoid99_data[i].glomn = header.glomn;
		g_geoid99_data[i].glomx = header.glomn + (double)(header.nlo - 1) * header.dlo;

		g_geoid99_data[i].dla = header.dla;
		g_geoid99_data[i].dlo = header.dlo;
      count++;
   }

   return count;
}

/*********************************************************************
*  clean_up_geoid99()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall clean_up_geoid99(void)
{
int i;

	for(i=0;i<g_geoid99_area_count;i++)
   {
   	fclose(g_geoid99_data[i].fp);
      g_geoid99_data[i].fp = NULL;
   }

   return CC_SUCCESS;
}

/*********************************************************************
*  geoid99()
*
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall geoid99(double xlon, double ylat, double *ght)
{
struct GEOID99_DATA *geoid99_data;

	*ght = -999.0;

	if(xlon < 0.0) xlon += 360.0;
   if(ylat < 0.0) ylat += 360.0;

	geoid99_data = which1(xlon,ylat);

	if(!geoid99_data) return GEOID99_OB;

	return geoid99_interp(geoid99_data, xlon, ylat, ght);
}


/*********************************************************************
*  which1()
*
*	Mimics which1 rouine in NGS intg program.  Need to document.
**********************************************************************/
struct GEOID99_DATA *which1(double xlon, double ylat)
{
int i;
int rank[MAXGEOID99AREAS];
int ne,se,we,ee;
double glamx,glamn,glomx,glomn;
double dla,dlo;

	for(i=0;i<g_geoid99_area_count;i++)
   {
   	rank[i] = 0;

      if(!g_geoid99_data[i].fp) continue;

      glamx = g_geoid99_data[i].glamx;
      glamn = g_geoid99_data[i].glamn;
      glomx = g_geoid99_data[i].glomx;
      glomn = g_geoid99_data[i].glomn;

      if(ylat > glamx || ylat < glamn || xlon > glomx || xlon < glomn) continue;

      ne = se = we = ee = 0;

      dla = g_geoid99_data[i].dla;
      dlo = g_geoid99_data[i].dlo;

      if((glamx - ylat) <= (dla / 2.0)) ne = 1;
      else if((ylat - glamn) <= (dla / 2.0)) se = 1;

      if((glomx - xlon) <= (dlo / 2.0)) ee = 1;
      else if((xlon - glomn) <= (dlo / 2.0)) we = 1;

      /* this is code for CONUS/Alaska overlap from NGS intg code */
      if(i!= 0 && g_using_geoid99_default_areas == 1
      		&& ylat <= 58.0 && ylat >= 49
            &&	xlon >= 230 && xlon <= 234
            && rank[0] > 0) return &g_geoid99_data[0];

      if(!ne && !se && !we && !ee) return &g_geoid99_data[i];

      if(ne && !we && !ee) rank[i] = 2;
      if(se && !we && !ee) rank[i] = 2;
      if(we && !ne && !se) rank[i] = 2;
      if(ee && !ne && !se) rank[i] = 2;
      if(ne && we) rank[i] = 1;
      if(se && we) rank[i] = 1;
      if(se && ee) rank[i] = 1;
      if(ne && ee) rank[i] = 1;
   }

	for(i=0;i<g_geoid99_area_count;i++)
		if(rank[i] == 2) return &g_geoid99_data[i];

	for(i=0;i<g_geoid99_area_count;i++)
		if(rank[i] == 1) return &g_geoid99_data[i];

   return NULL;
}


/*********************************************************************
*  geoid99_interp()
*
*	Need to document.
**********************************************************************/
int geoid99_interp(struct GEOID99_DATA *geoid99_data,
							double xlon, double ylat, double *ght)
{
double glamn, glamx;
double glomn, glomx;
double dla, dlo;
long nlo, nla;
double ylatn, xlonn;
int irown, icoln;
float f1,f2,f3,f4,f5,f6,f7,f8,f9;
float xx,yy;
float fx1,fx2,fx3;
float ftemp;
long offset;
long header_offset;
float val;

	if(!geoid99_data) return NULL_GEOID99_DATA_STRUCT;
   if(geoid99_data->fp < 0) return BAD_GEOID99_DATA_STRUCT_FP;

	header_offset = sizeof(struct GEOID99_HEADER);

	glamn = geoid99_data->glamn;
	glamx = geoid99_data->glamx;
	glomn = geoid99_data->glomn;
	glomx = geoid99_data->glomx;
	dla = geoid99_data->dla;
	dlo = geoid99_data->dlo;
	nla = geoid99_data->nla;
	nlo = geoid99_data->nlo;

   if(ylat > glamx || ylat < glamn || xlon < glomn || xlon > glomx) return GEOID99_OB;

   irown = geoid99_nint((ylat - glamn) / dla) + 1;
   icoln = geoid99_nint((xlon - glomn) / dlo) + 1;

   ylatn = glamn + (irown - 1) * dla;
   xlonn = glomn + (icoln - 1) * dlo;

   if(fabs(ylat - ylatn) < 0.0000000001 && fabs(xlon - xlonn) < 0.0000000001)
   {
   	offset = header_offset + ((irown - 1) * nlo + (icoln - 1)) * sizeof(float);
		fseek(geoid99_data->fp,offset,SEEK_SET);
		fread(&ftemp, sizeof(float),1,geoid99_data->fp);
      *ght = ftemp;

      return CC_SUCCESS;
   }

   if(irown == 1) irown = 2;
   if(irown == nla) irown = nla - 1;
   if(icoln == 1) icoln = 2;
   if(icoln == nlo) icoln = nlo - 1;

   offset = header_offset + ((irown - 2) * nlo + icoln - 2) * sizeof(float);
	fseek(geoid99_data->fp,offset,SEEK_SET);
	fread(&f1, sizeof(float),1,geoid99_data->fp);
	fread(&f2, sizeof(float),1,geoid99_data->fp);
	fread(&f3, sizeof(float),1,geoid99_data->fp);

   offset = header_offset + ((irown - 1) * nlo + icoln - 2) * sizeof(float);
	fseek(geoid99_data->fp,offset,SEEK_SET);
	fread(&f4, sizeof(float),1,geoid99_data->fp);
	fread(&f5, sizeof(float),1,geoid99_data->fp);
	fread(&f6, sizeof(float),1,geoid99_data->fp);

   offset = header_offset + (irown * nlo + icoln - 2) * sizeof(float);
	fseek(geoid99_data->fp,offset,SEEK_SET);
	fread(&f7, sizeof(float),1,geoid99_data->fp);
	fread(&f8, sizeof(float),1,geoid99_data->fp);
	fread(&f9, sizeof(float),1,geoid99_data->fp);

   xx = (xlon - (glomn + (icoln - 2) * dlo)) / dlo;
   yy = (ylat - (glamn + (irown - 2) * dla)) / dla;

   fx1 = qfit(xx,f1,f2,f3);
   fx2 = qfit(xx,f4,f5,f6);
   fx3 = qfit(xx,f7,f8,f9);
   val = qfit(yy,fx1,fx2,fx3);
   *ght = val;

   return CC_SUCCESS;
}

/*
	Routine to mimic behavior of FORTRAN nint function.
	nint rounds a number to the next integer with midway values rounded up.
*/
int geoid99_nint(double val)
{
int low;
int high;
double mid;

	low = floor(val);
   high = ceil(val);

	mid = (high + low) / 2.0;

   if(val < mid) return low;

   return high;
}

float qfit(float x, float f0, float f1, float f2)
{
float df0;
float df1;
float d2f0;

	df0 = f1 - f0;
	df1 = f2 - f1;
	d2f0 = df1 - df0;

	return f0 + x * df0 + 0.5 * x * (x - 1.0) * d2f0;
}

/*********************************************************************
*	Need to document.
**********************************************************************/
DLLEXPORT int _stdcall describe_geoid99_areas(char *desc)
{
int i;

 	if(g_geoid99_area_count < 1) return GEOID99_AREA_COUNT_ZERO_OR_LESS;

	strcpy(desc,"");

	for(i=0;i<g_geoid99_area_count;i++)
   	describe_geoid99_data_struct(&g_geoid99_data[i], desc);

   return CC_SUCCESS;
}



/*********************************************************************
*  describe_nadcon_data_struct()
*
*	Need to document.
**********************************************************************/
void describe_geoid99_data_struct(struct GEOID99_DATA *geoid99_data, char *desc)
{
char temp[256];

	if(!geoid99_data) return;

   sprintf(temp,"filename = %s\n",geoid99_data->filename);
   strcat(desc,temp);
   sprintf(temp,"glamx = %lf\n",geoid99_data->glamx);
   strcat(desc,temp);
   sprintf(temp,"glamn = %lf\n",geoid99_data->glamn);
   strcat(desc,temp);
   sprintf(temp,"glomx = %lf\n",geoid99_data->glomx);
   strcat(desc,temp);
   sprintf(temp,"glomn = %lf\n",geoid99_data->glomn);
   strcat(desc,temp);
   sprintf(temp,"dla = %lf\n",geoid99_data->dla);
   strcat(desc,temp);
   sprintf(temp,"dlo = %lf\n",geoid99_data->dlo);
   strcat(desc,temp);
   sprintf(temp,"nla = %ld\n",geoid99_data->nla);
   strcat(desc,temp);
   sprintf(temp,"nlo = %ld\n",geoid99_data->nlo);
   strcat(desc,temp);
}

