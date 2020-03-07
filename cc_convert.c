#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <dir.h>
#include "ccnad.h"
#include "ccvert.h"
#include "ccgeoid.h"
#include "ccgeoid99.h"
#include "grids83.h"
#include "grids27.h"
#include "gridcons.h"
#include "cc_convert.h"
#include "ccfileutil.h"
#include "usng.h"
#include "ccerror.h"

static CORPSCON_CONFIG g_config;
static CORPSCON_DATA_PT g_data;


/******************************************************************

This section contains the routines used for the conversion routines

******************************************************************/


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_initialize_convert()
{
int rc;
char hpgn_path[MAXPATH];

g_grid2geo_function = dummy_grid2geo;
g_usng2geo_function = dummy_usng2geo;
g_geo2grid_function = dummy_geo2grid;
g_geo2usng_function = dummy_geo2usng;
g_togeo83_function = dummy_nad;
g_fromgeo83_function = dummy_nad;
g_geoid_function = no_geoid_function;
g_vertcon_function = no_vertcon_function;
g_z_function1 = dummy_z;
g_z_function2 = dummy_z;
g_vunit_function_in = cvt_none;
g_vunit_function_out = cvt_none;
g_cf_function_in = no_cf_function;
g_cf_function_out = no_cf_function;

   if(g_config.in.vdatum == 0 || g_config.out.vdatum == 0)
   {
   	g_config.in.vdatum = 0;
   	g_config.out.vdatum = 0;
   }

	switch(g_config.in.datum)
   {
   	case CCNAD27:
			g_togeo83_function = nad27_to_nad83;
      	if(g_config.in.system == CCSTATEPLANE)
	         	g_grid2geo_function = sp27_to_geo27;
      	else if(g_config.in.system == CCUTM)
   	      	g_grid2geo_function = utm27_to_geo27;
      	else if(g_config.in.system == CCUSNG)
   	      	g_usng2geo_function = usng27_to_geo27;
      	break;
   	case CCNAD83:
			g_togeo83_function = dummy_nad;
      	if(g_config.in.system == CCSTATEPLANE)
	         	g_grid2geo_function = sp83_to_geo83;
      	else if(g_config.in.system == CCUTM)
   	      	g_grid2geo_function = utm83_to_geo83;
      	else if(g_config.in.system == CCUSNG)
   	      	g_usng2geo_function = usng83_to_geo83;
      	break;
   	case CCNAD91:
			g_togeo83_function = hpgn_to_nad83;
      	if(g_config.in.system == CCSTATEPLANE)
	         	g_grid2geo_function = sp83_to_geo83;
      	else if(g_config.in.system == CCUTM)
   	      	g_grid2geo_function = utm83_to_geo83;
      	else if(g_config.in.system == CCUSNG)
   	      	g_usng2geo_function = usng83_to_geo83;
      	break;
      default: return BAD_INPUT_DATUM_SPECIFIED;
   }

	switch(g_config.out.datum)
   {
   	case CCNAD27:
			g_fromgeo83_function = nad83_to_nad27;
      	if(g_config.out.system == CCSTATEPLANE)
	        	g_geo2grid_function = geo27_to_sp27;
      	else if(g_config.out.system == CCUTM)
         	g_geo2grid_function = geo27_to_utm27;
      	else if(g_config.out.system == CCUSNG)
         	g_geo2usng_function = geo27_to_usng27;
      	break;
   	case CCNAD83:
			g_fromgeo83_function = dummy_nad;
      	if(g_config.out.system == CCSTATEPLANE)
         	g_geo2grid_function = geo83_to_sp83;
      	else if(g_config.out.system == CCUTM)
         	g_geo2grid_function = geo83_to_utm83;
      	else if(g_config.out.system == CCUSNG)
         	g_geo2usng_function = geo83_to_usng83;
      	break;
   	case CCNAD91:
			g_fromgeo83_function = nad83_to_hpgn;
      	if(g_config.out.system == CCSTATEPLANE)
         	g_geo2grid_function = geo83_to_sp83;
      	else if(g_config.out.system == CCUTM)
         	g_geo2grid_function = geo83_to_utm83;
      	else if(g_config.out.system == CCUSNG)
         	g_geo2usng_function = geo83_to_usng83;
      	break;
      default: return BAD_OUTPUT_DATUM_SPECIFIED;
   }

   switch(g_config.in.system)
   {
   	case CCSTATEPLANE:
      case CCUTM:
      		g_config.in.x.width = 1;
      		g_config.in.y.width = 1;
      		g_config.in.x.precision = 3;
      		g_config.in.y.precision = 3;
            break;
      case CCUSNG:
      		g_config.in.x.width = 1;
      		g_config.in.y.width = 1;
      		g_config.in.x.precision = 5;
      		g_config.in.y.precision = 5;
            g_config.in.units = METER;
      		break;
      case CCGEOGRAPHIC:
      		if(g_config.in.x.width == 3)
	            	g_config.in.x.precision = 5;
      		else if(g_config.in.x.width == 2)
   	         	g_config.in.x.precision = 7;
      		else g_config.in.y.precision = 9;

      		if(g_config.in.y.width == 3)
            		g_config.in.y.precision = 5;
      		else if(g_config.in.y.width == 2)
            		g_config.in.y.precision = 7;
      		else g_config.in.y.precision = 9;

            break;
      default: return BAD_INPUT_SYSTEM_SPECIFIED;
   }

   if(g_config.in.vdatum == 0)
   {
   	g_cf_function_in = no_cf_function;
   }
   else
   {
	   switch(g_config.in.system)
   	{
   		case CCSTATEPLANE:
	      case CCUTM:
   	   case CCUSNG:
      			if(g_config.in.datum == CCNAD27) g_cf_function_in = nad27_cf;
      			else g_cf_function_in = nad83_cf;
      			break;
	      case CCGEOGRAPHIC:
   	   		g_cf_function_in = no_cf_function;
      	      break;
	      default: return BAD_INPUT_SYSTEM_SPECIFIED;
   	}
   }

   if(g_config.out.vdatum == 0)
   {
   	g_cf_function_out = no_cf_function;
   }
   else    // 'this is where to look for combined scale factor bug "MARK"
   {
	   switch(g_config.out.system)
   	{
   		case CCSTATEPLANE:
	      case CCUTM:
   	   case CCUSNG:
      			if(g_config.out.datum == CCNAD27) g_cf_function_out = nad27_cf;
      			else g_cf_function_out = nad83_cf;
      			break;
	      case CCGEOGRAPHIC:
   	   		g_cf_function_out = no_cf_function;
      	      break;
	      default: return BAD_OUTPUT_SYSTEM_SPECIFIED;
   	}
   }

   switch(g_config.out.system)
   {
   	case CCSTATEPLANE:
      case CCUTM:
      		g_config.out.x.width = 1;
      		g_config.out.y.width = 1;
      		g_config.out.x.precision = 3;
      		g_config.out.y.precision = 3;
      		break;
      case CCUSNG:
      		g_config.out.x.width = 1;
      		g_config.out.y.width = 1;
      		g_config.out.x.precision = 5;
      		g_config.out.y.precision = 5;
            g_config.out.units = METER;
      		break;
      case CCGEOGRAPHIC:
      		if(g_config.out.x.width == 3)
            		g_config.out.x.precision = 5;
      		else if(g_config.out.x.width == 2)
            		g_config.out.x.precision = 7;
      		else g_config.out.x.precision = 9;

      		if(g_config.out.y.width == 3)
            		g_config.out.y.precision = 5;
      		else if(g_config.out.y.width == 2)
            		g_config.out.y.precision = 7;
      		else g_config.out.y.precision = 9;

            break;
      default: return BAD_OUTPUT_SYSTEM_SPECIFIED;
   }

   if(g_config.in.datum == CCNAD91)
   {
   	fnmerge(hpgn_path,"",g_config.nadcon.path,g_config.in.hpgn_area,"");
      rc = initialize_hpgn_to_nad83(hpgn_path);
      if(rc < 0) return rc;
   }

   if(g_config.out.datum == CCNAD91)
   {
   	fnmerge(hpgn_path,"",g_config.nadcon.path,g_config.out.hpgn_area,"");
      rc = initialize_nad83_to_hpgn(hpgn_path);
      if(rc < 0) return rc;
   }

   if(!(g_config.in.datum == CCNAD83 && g_config.out.datum == CCNAD83))
   {
   	rc = initialize_nadcon(g_config.nadcon.path);
      if(rc < 0) return rc;
   }

   if(g_config.in.vdatum == CCNGVD29 || g_config.out.vdatum == CCNGVD29)
   {
   	if(g_config.vertcon.custom == 1)
      	rc = initialize_vertcon_from_file(g_config.vertcon.path,g_config.vertcon.custom_file);
   	else rc = initialize_vertcon(g_config.vertcon.path);
      if(rc < 0) return rc;
  		g_vertcon_function = vertcon;
   }
   else
   {
  		g_vertcon_function = no_vertcon_function;
   }

   if(g_config.in.vdatum == CCGRS80 || g_config.out.vdatum == CCGRS80 ||
		(g_config.in.system != CCGEOGRAPHIC && g_config.in.datum != CCNAD27) ||
		(g_config.out.system != CCGEOGRAPHIC && g_config.out.datum != CCNAD27))
   {
		switch(g_config.geoid_code_base)
   	{
   		case CCGEOID90:
      			g_geoid_function = geoid;
            	if(g_config.geoid.custom == 1)
            		rc = initialize_geoid_from_file(g_config.geoid.path,g_config.geoid.custom_file);
	            else rc = initialize_geoid90(g_config.geoid.path);
			      if(rc < 0) return rc;
      	      break;
	      case CCGEOID93:
   	   		g_geoid_function = geoid;
      	      if(g_config.geoid.custom == 1)
         	   	rc = initialize_geoid_from_file(g_config.geoid.path,g_config.geoid.custom_file);
      			else rc = initialize_geoid93(g_config.geoid.path);
			      if(rc < 0) return rc;
   	         break;
      	case CCGEOID96:
      			g_geoid_function = geoid;
            	if(g_config.geoid.custom == 1)
            		rc = initialize_geoid_from_file(g_config.geoid.path,g_config.geoid.custom_file);
	      		else rc = initialize_geoid96(g_config.geoid.path);
			      if(rc < 0) return rc;
      			break;
	      case CCGEOID99:
   	   		g_geoid_function = geoid99;
      	      if(g_config.geoid.custom == 1)
         	   	rc = initialize_geoid99_from_file(g_config.geoid.path,g_config.geoid.custom_file);
      			else rc = initialize_geoid99(g_config.geoid.path);
			      if(rc < 0) return rc;
   	   		break;
	      case CCGEOID03:
   	   		g_geoid_function = geoid99;
      	      if(g_config.geoid.custom == 1)
         	   	rc = initialize_geoid99_from_file(g_config.geoid.path,g_config.geoid.custom_file);
      			else rc = initialize_geoid03(g_config.geoid.path);
			      if(rc < 0) return rc;
   	   		break;
      	case 0:
      			g_geoid_function = no_geoid_function;
      			break;
	   	default: return BAD_GEOID_CODE_BASE_SPECIFIED;
	   }
   }
   else g_geoid_function = no_geoid_function;

   switch(g_config.in.vdatum)
   {
   	case CCNGVD29:
      	g_z_function1 = ngvd29_to_navd88;
         break;
      case 0:
      case CCNAVD88:
      	g_z_function1 = dummy_z;
         break;
   	case CCGRS80:
      	g_z_function1 = grs80_to_navd88;
         break;
      default: return BAD_INPUT_VDATUM_SPECIFIED;
   }

   switch(g_config.out.vdatum)
   {
   	case CCNGVD29:
      	g_z_function2 = navd88_to_ngvd29;
         break;
      case 0:
      case CCNAVD88:
      	g_z_function2 = dummy_z;
         break;
   	case CCGRS80:
      	g_z_function2 = navd88_to_grs80;
         break;
      default: return BAD_OUTPUT_VDATUM_SPECIFIED;
   }

   switch(g_config.in.vunits)
   {
   	case US_FOOT:
      	g_vunit_function_in = ufoot2meter;
         break;
   	case INT_FOOT:
      	g_vunit_function_in = ifoot2meter;
         break;
      case 0:
      case METER:
      	g_vunit_function_in = cvt_none;
         break;
      default: return BAD_INPUT_VUNITS_SPECIFIED;
   }

   switch(g_config.out.vunits)
   {
   	case US_FOOT:
      	g_vunit_function_out = meter2ufoot;
         break;
   	case INT_FOOT:
      	g_vunit_function_out = meter2ifoot;
         break;
      case 0:
      case METER:
      	g_vunit_function_out = cvt_none;
         break;
      default: return BAD_OUTPUT_VUNITS_SPECIFIED;
   }

   switch(g_config.in.units)
   {
   	case US_FOOT:
         if(g_config.out.units == INT_FOOT)
         	g_grid_shift_unit_function = ufoot2ifoot;
      	else if(g_config.out.units == METER)
         	g_grid_shift_unit_function = ufoot2meter;
         else
         	g_grid_shift_unit_function = cvt_none;
         break;
   	case INT_FOOT:
         if(g_config.out.units == US_FOOT)
         	g_grid_shift_unit_function = ifoot2ufoot;
      	else if(g_config.out.units == METER)
         	g_grid_shift_unit_function = ifoot2meter;
         else
         	g_grid_shift_unit_function = cvt_none;
         break;
      case METER:
         if(g_config.out.units == US_FOOT)
         	g_grid_shift_unit_function = meter2ufoot;
      	else if(g_config.out.units == INT_FOOT)
         	g_grid_shift_unit_function = meter2ifoot;
         else
         	g_grid_shift_unit_function = cvt_none;
         break;
      default:
         	g_grid_shift_unit_function = cvt_none;
            break;
   }

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_convert(void)
{
	g_data.convert_return_code = corpscon_convert_internal();

   return g_data.convert_return_code;
}


/******************************************************************
* Need to document.
******************************************************************/
int corpscon_convert_internal(void)
{
double x1,y1;
double x2,y2;
double x3,y3;
double x4,y4;
double x5,y5;
double z1,z2,z3,z4,z5;
double x_grid_shift_in, y_grid_shift_in;
int rc,g_rc,v_rc;

	if(g_grid2geo_function == NULL) return 0;
	if(g_usng2geo_function == NULL) return 0;
	if(g_togeo83_function == NULL) return 0;
	if(g_fromgeo83_function == NULL) return 0;
	if(g_geo2grid_function == NULL) return 0;
	if(g_geo2usng_function == NULL) return 0;
	if(g_z_function1 == NULL) return 0;
	if(g_z_function2 == NULL) return 0;
	if(g_geoid_function == NULL) return 0;
	if(g_vertcon_function == NULL) return 0;
	if(g_vunit_function_in == NULL) return 0;
	if(g_vunit_function_out == NULL) return 0;
	if(g_cf_function_in == NULL) return 0;
	if(g_cf_function_out == NULL) return 0;
	if(g_grid_shift_unit_function == NULL) return 0;

	x1 = g_data.in.x;
   y1 = g_data.in.y;
   z1 = g_data.in.z;
   g_data.in.conv = 0.0;
   g_data.in.kp = 0.0;
   g_data.in.cf = 0.0;
   g_data.in.dlas = 0.0;
   g_data.in.dlos = 0.0;
   g_data.in.dlam = 0.0;
   g_data.in.dlom = 0.0;
   g_data.out.conv = 0.0;
   g_data.out.kp = 0.0;
   g_data.out.cf = 0.0;
   g_data.out.dlas = 0.0;
   g_data.out.dlos = 0.0;
   g_data.out.dlam = 0.0;
   g_data.out.dlom = 0.0;
   g_data.convert_return_code = 0;

   x_grid_shift_in = (*g_grid_shift_unit_function)(x1);
   y_grid_shift_in = (*g_grid_shift_unit_function)(y1);

	z2 = (*g_vunit_function_in)(z1);

   rc = (*g_grid2geo_function)(g_config.in.zone, g_config.in.units,
	   								&y2,&x2,&y1,&x1,&g_data.in.conv,&g_data.in.kp);
   if(rc != CC_SUCCESS) return rc;

   rc = (*g_usng2geo_function)(&y2,&x2,&g_data.in.conv,&g_data.in.kp,
   										&g_data.in.usng);
   if(rc != CC_SUCCESS) return rc;

   x2 *= -1.0;

   rc = (*g_togeo83_function)(&x2,&y2,&x3,&y3,&g_data.in.dlas,&g_data.in.dlos,
   										&g_data.in.dlam,&g_data.in.dlom);
   if(rc != CC_SUCCESS) return rc;

   g_rc = (*g_geoid_function)(x3,y3,&g_data.ght);

   rc = g_cf_function_in(&g_data.in.cf,z2,g_data.in.kp,g_data.ght,g_rc);
   if(rc != CC_SUCCESS) return rc;

   v_rc = (*g_vertcon_function)(x3,y3,&g_data.vht);

   rc = (*g_z_function1)(z2,&z3,g_data.ght,g_data.vht,g_rc,v_rc);
   if(rc != CC_SUCCESS) return rc;

   rc = (*g_z_function2)(z3,&z4,g_data.ght,g_data.vht,g_rc,v_rc);

   if(rc != CC_SUCCESS) return rc;

   rc = (*g_fromgeo83_function)(&x3,&y3,&x4,&y4,&g_data.out.dlas,&g_data.out.dlos,
   											&g_data.out.dlam,&g_data.out.dlom);
   if(rc != CC_SUCCESS) return rc;

   x4 *= -1.0;

   rc = (*g_geo2grid_function)(g_config.out.zone, g_config.out.units,
   									&y4,&x4,&y5,&x5,&g_data.out.conv,&g_data.out.kp);
   if(rc != CC_SUCCESS) return rc;

   rc = (*g_geo2usng_function)(y5,x5,g_config.out.usng.precision,
   										&g_data.out.usng,&g_data.out.conv,&g_data.out.kp);
   if(rc != CC_SUCCESS) return rc;

   rc = g_cf_function_out(&g_data.out.cf,z4,g_data.out.kp,g_data.ght,g_rc);
   if(rc != CC_SUCCESS) return rc;

	z5 = (*g_vunit_function_out)(z4);

	g_data.out.x = x5;
   g_data.out.y = y5;
   g_data.out.z = z5;

   g_data.x_grid_shift = x5 - x_grid_shift_in;
   g_data.y_grid_shift = y5 - y_grid_shift_in;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_convert_line(char *line)
{
	g_data.convert_return_code = corpscon_convert_line_internal(line);

   return g_data.convert_return_code;
}


/******************************************************************
* Need to document.
******************************************************************/
int corpscon_convert_line_internal(char *line)
{
double x,y,z;
char usng[64];
int rc;
int i;
int n;

	if(line == NULL) return INPUT_LINE_IS_NULL;
   n = strlen(line);
   if(line[n-1] == '\n') line[n-1] = '\0';

	for(i=0;i<CCMAXFIELDCOUNT;i++) strcpy(g_data.fields[i],"");

	rc = cc_parse_line(line,g_config.in.field_delimiter,g_data.fields);
   if(rc < 1) return INPUT_LINE_NOT_PARSED;

   g_data.field_count = rc;

   if(g_config.in.system == CCUSNG)
   	strcpy(usng,g_data.fields[g_config.in.usng.field - 1]);
   else
   {
   	rc = xy_line_parse(&x,&y);
      if(rc != CC_SUCCESS) return rc;
   }

   if(g_config.in.z.field > 0)
   {
	   if(g_config.in.z.field > g_data.field_count)
   		return INPUT_FIELD_Z_COULD_NOT_BE_PARSED;
	   rc = ParseListForValue(g_data.fields,g_config.in.z.field,1,&z);
   	if(rc != CC_SUCCESS) return INPUT_FIELD_Z_COULD_NOT_BE_PARSED;
   }

   if(g_config.in.point_name_field > 0)
   {
	   if(g_config.in.point_name_field > g_data.field_count)
   		return INPUT_FIELD_POINT_NAME_COULD_NOT_BE_PARSED;
      strcpy(g_data.point_name,g_data.fields[g_config.in.point_name_field - 1]);
   }

   strcpy(g_data.in.usng,usng);
   g_data.in.x = x;
   g_data.in.y = y;
   g_data.in.z = z;

   rc = corpscon_convert_internal();
   if(rc != CC_SUCCESS) return rc;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_clean_up(void)
{
	clean_up_nadcon();
   clean_up_nad83_to_hpgn();
   clean_up_hpgn_to_nad83();
   clean_up_vertcon();
   clean_up_geoid();
   clean_up_geoid99();

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int xy_line_parse(double *x, double *y)
{
int rc;

   if(g_config.in.x.field + g_config.in.x.width - 1 > g_data.field_count)
   	return INPUT_FIELD_X_COULD_NOT_BE_PARSED;
   rc = ParseListForValue(g_data.fields,g_config.in.x.field,g_config.in.x.width,x);
   if(rc != CC_SUCCESS) return INPUT_FIELD_X_COULD_NOT_BE_PARSED;

   if(g_config.in.y.field + g_config.in.y.width - 1 > g_data.field_count)
   	return INPUT_FIELD_Y_COULD_NOT_BE_PARSED;
   rc = ParseListForValue(g_data.fields,g_config.in.y.field,g_config.in.y.width,y);
   if(rc != CC_SUCCESS) return INPUT_FIELD_Y_COULD_NOT_BE_PARSED;

   return CC_SUCCESS;
}


/******************************************************************

This section contains the routines used for configuration

******************************************************************/




/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_default_config(void)
{
int i;

   g_config.in.system = CCGEOGRAPHIC;
   g_config.in.datum = CCNAD27;
   g_config.in.zone = 0;
   g_config.in.units = METER;
   g_config.in.vdatum = 0;
   g_config.in.vunits = METER;
   strcpy(g_config.in.hpgn_area,"");
   g_config.in.x.field = 3;
   g_config.in.y.field = 2;
   g_config.in.z.field = 0;
   g_config.in.usng.field = 0;
   g_config.in.point_name_field = 1;
   g_config.in.x.width = 1;
   g_config.in.y.width = 1;
   g_config.in.z.width = 1;
   g_config.in.usng.width = 1;
   g_config.in.x.precision = 9;
   g_config.in.y.precision = 9;
   g_config.in.z.precision = 3;
   g_config.in.usng.precision = 5;
   strcpy(g_config.in.field_delimiter,",");

   g_config.out.system = CCGEOGRAPHIC;
   g_config.out.datum = CCNAD83;
   g_config.out.zone = 0;
   g_config.out.units = METER;
   g_config.out.vdatum = 0;
   g_config.out.vunits = METER;
   strcpy(g_config.out.hpgn_area,"");
   g_config.out.x.field = 3;
   g_config.out.y.field = 2;
   g_config.out.z.field = 0;
   g_config.out.usng.field = 0;
   g_config.out.point_name_field = 1;
   g_config.out.x.width = 1;
   g_config.out.y.width = 1;
   g_config.out.z.width = 1;
   g_config.out.usng.width = 1;
   g_config.out.x.precision = 9;
   g_config.out.y.precision = 9;
   g_config.out.z.precision = 3;
   g_config.out.usng.precision = 5;
   strcpy(g_config.out.field_delimiter,",");

   g_config.geoid.custom = 0;
   g_config.nadcon.custom = 0;
   g_config.vertcon.custom = 0;
   g_config.geoid_code_base = 0;
   strcpy(g_config.geoid.path,"");
   strcpy(g_config.nadcon.path,"");
   strcpy(g_config.vertcon.path,"");

   for(i=0;i<CCMAXFIELDCOUNT;i++) g_config.output_format[i] = 0;
   g_config.output_fields = 0;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_config_from_script_file(char *script_file)
{
FILE *fp;
char line[256];
int rc;

	fp = fopen(script_file,"r");
   if(fp == NULL) return CANT_OPEN_CONFIG_SCRIPT_FILE;

   corpscon_default_config();

   while(!feof(fp))
   {
   	rc = fgets(line,255,fp);
      if(rc == NULL) continue;
      if(line == NULL) continue;
      if(line[0] == '#') continue;
      if(line[0] == ';') continue;
      corpscon_set_config_from_script_line(line);
   }

   fclose(fp);

//   corpscon_initialize_convert();

   return CC_SUCCESS;
}

/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_set_config_from_script_line(char *line)
{
char key[128];
char value[128];
char delimiter[64];
char *p;
int n;
int rc;

	if(line == NULL) return 0;
   if(!strcmp(line,"")) return 0;
   n = strlen(line);
   if(n < 1) return 0;
   if(line[n-1] == '\n') line[n-1] = '\0';

   p = strchr(line,'=');
   if(p == NULL) return 0;

   p = strtok(line,"=");
   strcpy(key,p);
   rc = sscanf(key,"%s",key);
   if(rc == NULL || rc != 1) return 0;

   p = strtok(NULL,"=");
   if(p == NULL) return 0;
   strcpy(value,p);

   if(!strcmpi(key,"IN_SYSTEM"))	rc = SetInSystem(atol(value));
   else if(!strcmpi(key,"IN_DATUM")) rc = SetInDatum(atol(value));
   else if(!strcmpi(key,"IN_ZONE")) rc = SetInZone(atol(value));
   else if(!strcmpi(key,"IN_UNITS")) rc = SetInUnits(atol(value));
   else if(!strcmpi(key,"IN_VDATUM")) rc = SetInVDatum(atol(value));
   else if(!strcmpi(key,"IN_VUNITS")) rc = SetInVUnits(atol(value));
   else if(!strcmpi(key,"IN_HPGN_AREA")) rc = SetInHPGNArea(value);
   else if(!strcmpi(key,"OUT_SYSTEM")) rc = SetOutSystem(atol(value));
   else if(!strcmpi(key,"OUT_DATUM")) rc = SetOutDatum(atol(value));
   else if(!strcmpi(key,"OUT_ZONE")) rc = SetOutZone(atol(value));
   else if(!strcmpi(key,"OUT_UNITS")) rc = SetOutUnits(atol(value));
   else if(!strcmpi(key,"OUT_VDATUM")) rc = SetOutVDatum(atol(value));
   else if(!strcmpi(key,"OUT_VUNITS")) rc = SetOutVUnits(atol(value));
   else if(!strcmpi(key,"OUT_HPGN_AREA")) rc = SetOutHPGNArea(value);
   else if(!strcmpi(key,"IN_X_FIELD")) rc = SetXInParseField(atoi(value));
   else if(!strcmpi(key,"IN_X_FIELD_WIDTH")) rc = SetXInFieldWidth(atoi(value));
   else if(!strcmpi(key,"IN_Y_FIELD")) rc = SetYInParseField(atoi(value));
   else if(!strcmpi(key,"IN_Y_FIELD_WIDTH")) rc = SetYInFieldWidth(atoi(value));
   else if(!strcmpi(key,"IN_USNG_FIELD")) rc = SetUSNGInParseField(atoi(value));
   else if(!strcmpi(key,"IN_Z_FIELD")) rc = SetZInParseField(atoi(value));
   else if(!strcmpi(key,"OUT_X_FIELD_WIDTH")) rc = SetXOutFieldWidth(atoi(value));
   else if(!strcmpi(key,"OUT_Y_FIELD_WIDTH")) rc = SetYOutFieldWidth(atoi(value));
   else if(!strcmpi(key,"IN_POINT_NAME_FIELD")) rc = SetPointNameInField(atoi(value));
   else if(!strcmpi(key,"OUTPUT_FORMAT")) rc = SetOutputLineFormat(value);
   else if(!strcmpi(key,"NADCON_DATA_PATH")) rc = SetNadconPath(value);
   else if(!strcmpi(key,"VERTCON_DATA_PATH")) rc = SetVertconPath(value);
   else if(!strcmpi(key,"VERTCON_CUSTOM_AREA_LIST_FILE")) rc = SetVertconCustomAreaListFile(value);
   else if(!strcmpi(key,"VERTCON_USE_CUSTOM_AREAS")) rc = SetUseVertconCustomAreas(atoi(value));
   else if(!strcmpi(key,"GEOID_CODE_BASE")) rc = SetGeoidCodeBase(atoi(value));
   else if(!strcmpi(key,"GEOID_DATA_PATH")) rc = SetGeoidPath(value);
   else if(!strcmpi(key,"GEOID_CUSTOM_AREA_LIST_FILE")) rc = SetGeoidCustomAreaListFile(value);
   else if(!strcmpi(key,"GEOID_USE_CUSTOM_AREAS")) rc = SetUseGeoidCustomAreas(atoi(value));
   else if(!strcmpi(key,"IN_FIELD_DELIMITER")) delimiter_from_config(value,g_config.in.field_delimiter);
   else if(!strcmpi(key,"OUT_FIELD_DELIMITER")) delimiter_from_config(value,g_config.out.field_delimiter);
   else rc = KEY_NOT_FOUND;

   if(rc != CC_SUCCESS) return rc;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall corpscon_save_config_to_script_file(char *script_file)
{
int i;
char str1[256];
char str2[256];
char delimiter[64];
FILE *fp;

	fp = fopen(script_file,"w");
   if(fp == NULL) return CANT_OPEN_CONFIG_SCRIPT_FILE;

   fprintf(fp,"IN_SYSTEM=%d\n",g_config.in.system);
   fprintf(fp,"IN_DATUM=%d\n",g_config.in.datum);
   fprintf(fp,"IN_ZONE=%d\n",g_config.in.zone);
   fprintf(fp,"IN_UNITS=%d\n",g_config.in.units);
   fprintf(fp,"IN_VDATUM=%d\n",g_config.in.vdatum);
   fprintf(fp,"IN_VUNITS=%d\n",g_config.in.vunits);
   fprintf(fp,"IN_HPGN_AREA=%s\n",g_config.in.hpgn_area);
   fprintf(fp,"IN_X_FIELD=%d\n",g_config.in.x.field);
   fprintf(fp,"IN_X_FIELD_WIDTH=%d\n",g_config.in.x.width);
   fprintf(fp,"IN_Y_FIELD=%d\n",g_config.in.y.field);
   fprintf(fp,"IN_Y_FIELD_WIDTH=%d\n",g_config.in.y.width);
   fprintf(fp,"IN_Z_FIELD=%d\n",g_config.in.z.field);
   fprintf(fp,"IN_USNG_FIELD=%d\n",g_config.in.usng.field);
   fprintf(fp,"IN_POINT_NAME_FIELD=%d\n",g_config.in.point_name_field);
   delimiter_to_config(g_config.in.field_delimiter,delimiter);
   fprintf(fp,"IN_FIELD_DELIMITER=%s\n",delimiter);
   fprintf(fp,"OUT_SYSTEM=%d\n",g_config.out.system);
   fprintf(fp,"OUT_DATUM=%d\n",g_config.out.datum);
   fprintf(fp,"OUT_ZONE=%d\n",g_config.out.zone);
   fprintf(fp,"OUT_UNITS=%d\n",g_config.out.units);
   fprintf(fp,"OUT_VDATUM=%d\n",g_config.out.vdatum);
   fprintf(fp,"OUT_VUNITS=%d\n",g_config.out.vunits);
   fprintf(fp,"OUT_HPGN_AREA=%s\n",g_config.out.hpgn_area);
   fprintf(fp,"OUT_X_FIELD_WIDTH=%d\n",g_config.out.x.width);
   fprintf(fp,"OUT_Y_FIELD_WIDTH=%d\n",g_config.out.y.width);
   delimiter_to_config(g_config.out.field_delimiter,delimiter);
   fprintf(fp,"OUT_FIELD_DELIMITER=%s\n",delimiter);
   fprintf(fp,"NADCON_DATA_PATH=%s\n",g_config.nadcon.path);
   fprintf(fp,"VERTCON_DATA_PATH=%s\n",g_config.vertcon.path);
   fprintf(fp,"VERTCON_CUSTOM_AREA_LIST_FILE=%s\n",g_config.vertcon.custom_file);
   fprintf(fp,"USE_VERTCON_CUSTOM_AREAS=%d\n",g_config.vertcon.custom);
   fprintf(fp,"GEOID_DATA_PATH=%s\n",g_config.geoid.path);
   fprintf(fp,"GEOID_CUSTOM_AREA_LIST_FILE=%s\n",g_config.geoid.custom_file);
   fprintf(fp,"USE_GEOID_CUSTOM_AREAS=%d\n",g_config.geoid.custom);
   fprintf(fp,"GEOID_CODE_BASE=%d\n",g_config.geoid_code_base);

   strcpy(str1,"");
   strcpy(str2,"");
   for(i=0;i<g_config.output_fields;i++)
   {
   	sprintf(str1,"%d ",g_config.output_format[i]);
   	strcat(str2,str1);
   }
   fprintf(fp,"OUTPUT_FORMAT=%s\n",str2);

   fclose(fp);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int delimiter_to_config(char *delimiter_in, char *delimiter_out)
{
	if(!strcmp(delimiter_in,",")) strcpy(delimiter_out,"COMMA");
	else if(!strcmp(delimiter_in," ")) strcpy(delimiter_out,"SPACE");
	else if(!strcmp(delimiter_in,"\t")) strcpy(delimiter_out,"TAB");
   else strcpy(delimiter_out,delimiter_in);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int delimiter_from_config(char *delimiter_in, char *delimiter_out)
{
	if(!strcmp(delimiter_in,"COMMA")) strcpy(delimiter_out,",");
	else if(!strcmp(delimiter_in,"SPACE")) strcpy(delimiter_out," ");
	else if(!strcmp(delimiter_in,"TAB")) strcpy(delimiter_out,"\t");
   else strcpy(delimiter_out,delimiter_in);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInSystem(int val)
{
	switch(val)
   {
   	case CCGEOGRAPHIC: case CCSTATEPLANE: case CCUTM: case CCUSNG:
      	g_config.in.system = val; break;
      default: return BAD_INPUT_SYSTEM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutSystem(int val)
{
	switch(val)
   {
   	case CCGEOGRAPHIC: case CCSTATEPLANE: case CCUTM: case CCUSNG:
      	g_config.out.system = val; break;
      default: return BAD_OUTPUT_SYSTEM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInDatum(int val)
{
	switch(val)
   {
   	case CCNAD27: case CCNAD83: case CCNAD91:
      	g_config.in.datum = val; break;
      default: return BAD_INPUT_DATUM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutDatum(int val)
{
	switch(val)
   {
   	case CCNAD27: case CCNAD83: case CCNAD91:
      	g_config.out.datum = val; break;
      default: return BAD_OUTPUT_DATUM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInUnits(int val)
{
	switch(val)
   {
   	case METER: case US_FOOT: case INT_FOOT:
      	g_config.in.units = val; break;
      default: return BAD_INPUT_UNITS_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutUnits(int val)
{
	switch(val)
   {
   	case METER: case US_FOOT: case INT_FOOT:
      	g_config.out.units = val; break;
      default: return BAD_OUTPUT_UNITS_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInZone(int val)
{
	g_config.in.zone = val;
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutZone(int val)
{
	g_config.out.zone = val;
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInVDatum(int val)
{
	switch(val)
   {
   	case 0: case CCNGVD29: case CCNAVD88: case CCGRS80:
      	g_config.in.vdatum = val; break;
      default: return BAD_INPUT_VDATUM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutVDatum(int val)
{
	switch(val)
   {
   	case 0: case CCNGVD29: case CCNAVD88: case CCGRS80:
      	g_config.out.vdatum = val; break;
      default: return BAD_OUTPUT_VDATUM_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInVUnits(int val)
{
	switch(val)
   {
   	case METER: case US_FOOT: case INT_FOOT:
      	g_config.in.vunits = val; break;
      default: return BAD_INPUT_VUNITS_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutVUnits(int val)
{
	switch(val)
   {
   	case METER: case US_FOOT: case INT_FOOT:
      	g_config.out.vunits = val; break;
      default: return BAD_OUTPUT_VUNITS_SPECIFIED;
   }
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutUSNGDigits(int val)
{
int digits;

	if(val < 2) digits = 2;
   else if(val > 5) digits = 5;
   else digits = val;

   g_config.out.usng.precision = digits;
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetNadconPath(char *path)
{
	strcpy(g_config.nadcon.path,path);
   return CC_SUCCESS;
}



/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInHPGNArea(char *area)
{
	strcpy(g_config.in.hpgn_area,area);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutHPGNArea(char *area)
{
	strcpy(g_config.out.hpgn_area,area);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetVertconPath(char *path)
{
	strcpy(g_config.vertcon.path,path);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetVertconCustomAreaListFile(char *filename)
{
	strcpy(g_config.vertcon.custom_file,filename);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetUseVertconCustomAreas(int opt)
{
	if(opt == 1) g_config.vertcon.custom = 1;
   else g_config.vertcon.custom = 0;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetGeoidCodeBase(int val)
{
	switch(val)
   {
   	case CCGEOID90:
      case CCGEOID93:
      case CCGEOID96:
      case CCGEOID99:
      case CCGEOID03:
			   g_config.geoid_code_base = val; break;
   	default: return BAD_GEOID_CODE_BASE_SPECIFIED;
   }
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetGeoidPath(char *path)
{
	strcpy(g_config.geoid.path,path);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetGeoidCustomAreaListFile(char *filename)
{
	strcpy(g_config.geoid.custom_file,filename);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetUseGeoidCustomAreas(int opt)
{
	if(opt == 1) g_config.geoid.custom = 1;
   else g_config.geoid.custom = 0;

   return CC_SUCCESS;
}






/******************************************************************

This section contains the routines for converting single points.

******************************************************************/






/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetXIn(double val)
{
	g_data.in.x = val;
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetYIn(double val)
{
	g_data.in.y = val;
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetZIn(double val)
{
	g_data.in.z = val;
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetUSNGIn(char *val)
{
	strcpy(g_data.in.usng,val);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetConvertReturnCode(void)
{
	return g_data.convert_return_code;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT double _stdcall GetXOut(void)
{
	return g_data.out.x;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT double _stdcall GetYOut(void)
{
	return g_data.out.y;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT double _stdcall GetZOut(void)
{
	return g_data.out.z;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetUSNGOut(char *val)
{
	strcpy(val,g_data.out.usng);
	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetCorpsconValue(int code, double *val)
{
	switch(code)
   {
   	case X_OUT_SEARCH_CODE: *val = g_data.out.x; break;
   	case Y_OUT_SEARCH_CODE: *val = g_data.out.y; break;
   	case Z_OUT_SEARCH_CODE: *val = g_data.out.z; break;
   	case KP_OUT_SEARCH_CODE: *val = g_data.out.kp; break;
   	case CF_OUT_SEARCH_CODE: *val = g_data.out.cf; break;
   	case CONV_OUT_SEARCH_CODE: *val = g_data.out.conv; break;
   	case DLAS_OUT_SEARCH_CODE: *val = g_data.out.dlas; break;
   	case DLOS_OUT_SEARCH_CODE: *val = g_data.out.dlos; break;
   	case DLAM_OUT_SEARCH_CODE: *val = g_data.out.dlam; break;
   	case DLOM_OUT_SEARCH_CODE: *val = g_data.out.dlom; break;
   	case X_IN_SEARCH_CODE: *val = g_data.in.x; break;
   	case Y_IN_SEARCH_CODE: *val = g_data.in.y; break;
   	case Z_IN_SEARCH_CODE: *val = g_data.in.z; break;
   	case KP_IN_SEARCH_CODE: *val = g_data.in.kp; break;
   	case CF_IN_SEARCH_CODE: *val = g_data.in.cf; break;
   	case CONV_IN_SEARCH_CODE: *val = g_data.in.conv; break;
   	case DLAS_IN_SEARCH_CODE: *val = g_data.in.dlas; break;
   	case DLOS_IN_SEARCH_CODE: *val = g_data.in.dlos; break;
   	case DLAM_IN_SEARCH_CODE: *val = g_data.in.dlam; break;
   	case DLOM_IN_SEARCH_CODE: 	*val = g_data.in.dlom; break;
      case DLAM_TOTAL_SEARCH_CODE: *val = g_data.in.dlam + g_data.out.dlam; break;
      case DLOM_TOTAL_SEARCH_CODE: *val = g_data.in.dlom + g_data.out.dlom; break;
      case X_GRID_SHIFT_SEARCH_CODE: *val = g_data.x_grid_shift; break;
      case Y_GRID_SHIFT_SEARCH_CODE: *val = g_data.y_grid_shift; break;
      default: return BAD_VALUE_CODE_SPECIFIED;
   }

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetCorpsconValueStr(int code, char *str)
{
	switch(code)
   {
   	case POINT_NAME_SEARCH_CODE:
      	if(g_data.point_name == NULL) strcpy(str,"");
         else strcpy(str,g_data.point_name);
      	break;
   	case X_OUT_SEARCH_CODE:
      	WriteOutputStr(g_data.out.x,g_config.out.x.width,g_config.out.x.precision,str);
         break;
   	case Y_OUT_SEARCH_CODE:
      	WriteOutputStr(g_data.out.y,g_config.out.y.width,g_config.out.y.precision,str);
         break;
   	case Z_OUT_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.out.z);
         break;
   	case USNG_OUT_SEARCH_CODE:
      	sprintf(str,"%s",g_data.out.usng);
         break;
   	case KP_OUT_SEARCH_CODE:
      	sprintf(str,"%.9lf",g_data.out.kp);
         break;
   	case CF_OUT_SEARCH_CODE:
      	sprintf(str,"%.9lf",g_data.out.cf);
         break;
   	case CONV_OUT_SEARCH_CODE:
      	WriteOutputStr(g_data.out.conv,3,5,str);
         break;
   	case DLAS_OUT_SEARCH_CODE:
      	sprintf(str,"%.5lf",g_data.out.dlas);
         break;
   	case DLOS_OUT_SEARCH_CODE:
      	sprintf(str,"%.5lf",g_data.out.dlos);
         break;
   	case DLAM_OUT_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.out.dlam);
         break;
   	case DLOM_OUT_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.out.dlom);
         break;
   	case X_IN_SEARCH_CODE:
      	WriteOutputStr(g_data.in.x,g_config.in.x.width,g_config.in.x.precision,str);
         break;
   	case Y_IN_SEARCH_CODE:
      	WriteOutputStr(g_data.in.y,g_config.in.y.width,g_config.in.y.precision,str);
         break;
   	case Z_IN_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.in.z);
         break;
      case X_INPUT_FIELD_SEARCH_CODE:
      	GetInputField(g_config.in.x.field, str);
         break;
      case Y_INPUT_FIELD_SEARCH_CODE:
      	GetInputField(g_config.in.y.field, str);
         break;
      case Z_INPUT_FIELD_SEARCH_CODE:
      	GetInputField(g_config.in.z.field, str);
         break;
   	case USNG_IN_SEARCH_CODE:
      	sprintf(str,"%s",g_data.in.usng);
         break;
   	case KP_IN_SEARCH_CODE:
      	sprintf(str,"%.9lf",g_data.in.kp);
         break;
   	case CF_IN_SEARCH_CODE:
      	sprintf(str,"%.9lf",g_data.in.cf);
         break;
   	case CONV_IN_SEARCH_CODE:
      	WriteOutputStr(g_data.in.conv,3,5,str);
         break;
   	case DLAS_IN_SEARCH_CODE:
      	sprintf(str,"%.5lf",g_data.in.dlas);
         break;
   	case DLOS_IN_SEARCH_CODE:
      	sprintf(str,"%.5lf",g_data.in.dlos);
         break;
   	case DLAM_IN_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.in.dlam);
         break;
   	case DLOM_IN_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.in.dlom);
         break;
   	case DLAM_TOTAL_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.in.dlam + g_data.out.dlam);
         break;
   	case DLOM_TOTAL_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.in.dlom + g_data.out.dlom);
         break;
   	case X_GRID_SHIFT_SEARCH_CODE:
      	sprintf(str,"%.1lf",g_data.x_grid_shift);
         break;
   	case Y_GRID_SHIFT_SEARCH_CODE:
      	sprintf(str,"%.1lf",g_data.y_grid_shift);
         break;
   	case GHT_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.ght);
         break;
   	case VHT_SEARCH_CODE:
      	sprintf(str,"%.3lf",g_data.vht);
         break;
      default:
      	return BAD_VALUE_CODE_SPECIFIED;
         break;
   }

	return CC_SUCCESS;
}





/******************************************************************

This section contains the routines for converting files.

******************************************************************/




/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetXInParseField(int val)
{
	if(val < 1) return BAD_XIN_PARSE_FIELD_SPECIFIED;
   g_config.in.x.field = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetYInParseField(int val)
{
	if(val < 1) return BAD_YIN_PARSE_FIELD_SPECIFIED;
   g_config.in.y.field = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetZInParseField(int val)
{
	if(val < 0) return BAD_ZIN_PARSE_FIELD_SPECIFIED;
   g_config.in.z.field = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetUSNGInParseField(int val)
{
	if(val < 1) return BAD_USNGIN_PARSE_FIELD_SPECIFIED;
   g_config.in.usng.field = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetPointNameInField(int val)
{
	if(val < 1) return BAD_POINT_NAME_IN_PARSE_FIELD_SPECIFIED;
   g_config.in.point_name_field = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetXInFieldWidth(int val)
{
	if(val < 1 || val > 3) return BAD_XIN_FIELD_WIDTH_SPECIFIED;
   g_config.in.x.width = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetYInFieldWidth(int val)
{
	if(val < 1 || val > 3) return BAD_YIN_FIELD_WIDTH_SPECIFIED;
   g_config.in.y.width = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetXOutFieldWidth(int val)
{
	if(val < 1 || val > 3) return BAD_XOUT_FIELD_WIDTH_SPECIFIED;
   g_config.out.x.width = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetYOutFieldWidth(int val)
{
	if(val < 1 || val > 3) return BAD_YOUT_FIELD_WIDTH_SPECIFIED;
   g_config.out.y.width = val;

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetInFieldDelimiter(char *val)
{
	strcpy(g_config.in.field_delimiter,val);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutFieldDelimiter(char *val)
{
	strcpy(g_config.out.field_delimiter,val);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall SetOutputLineFormat(char *format_str)
{
char *p;
int i=0;
double dtmp;
int rc;

	if(format_str == NULL) return NULL_OUTPUT_LINE_FORMAT_STRING;

   p = strtok(format_str," ");
   if(p == NULL) return 0;

   while(p)
   {
   	rc = sscanf(p,"%lf",&dtmp);
      if(rc == NULL || rc != 1) return NULL_OUTPUT_LINE_FORMAT_STRING;
      g_config.output_format[i] = floor(dtmp);
      p = strtok(NULL," ");
      i++;
   }

   g_config.output_fields = i;

   return i;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutputLine(char *line)
{
char stmp[64];
int i;
int rc;
int n,m;

   strcpy(line,"");

   for(i=0;i<g_config.output_fields;i++)
   {
   	strcpy(stmp,"");
   	if(g_config.output_format[i] == 0) continue;
      else if(g_config.output_format[i] < 0)
      {
      	rc = GetInputField(abs(g_config.output_format[i]), stmp);
         if(rc != CC_SUCCESS) return rc;
      }
      else
      {
      	rc = GetCorpsconValueStr(g_config.output_format[i],stmp);
         if(rc != CC_SUCCESS) return rc;
      }
      strcat(line,stmp);
      strcat(line,g_config.out.field_delimiter);
   }

   m = strlen(g_config.out.field_delimiter);
   n = strlen(line);
   if(n>m) line[n-m] = '\0';

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInputField(int field, char *str)
{
	strcpy(str,"");

	if(field < 1) return BAD_FIELD_SPECIFIED;

   if(g_data.fields[field-1] == NULL) strcpy(str,"");
   else strcpy(str,g_data.fields[field-1]);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall WriteOutputStr(double val, int fields, int precision, char *str)
{
char format_str[128];
double dmin,dsec;
int ideg,imin;
char sign[2];

	if(val < 0) strcpy(sign,"-");
	else strcpy(sign,"");

   val = fabs(val);

   switch(fields)
   {
   	case 1:
      	sprintf(format_str,"%%s%%.%dlf",precision);
         break;
   	case 2:
      	sprintf(format_str,"%%s%%d %%0%d.%dlf",precision + 3, precision);
         break;
   	case 3:
      	sprintf(format_str,"%%s%%d %%02d %%0%d.%dlf",precision + 3, precision);
         break;
      default:
      	return 0;
   }

   switch(fields)
   {
   	case 1:
      	sprintf(str,format_str,sign,val);
         break;
   	case 2:
      	ideg = floor(val);
         dmin = (val - (double) ideg) * 60;
      	sprintf(str,format_str,sign,ideg,dmin);
         break;
   	case 3:
      	ideg = floor(val);
         dmin = (val - (double) ideg) * 60;
         imin = floor(dmin);
         dsec = (dmin - (double) imin) * 60;
      	sprintf(str,format_str,sign,ideg,imin,dsec);
         break;
      default:
      	return 0;
   }

	return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int ParseListForValue(CC_PARSELIST the_list, int start, int count, double *val)
{
double tmp1=0.0;
double tmp2=0.0;
double tmp3=0.0;
double out_val=0.0;
int i;
int rc;

	if(count < 1 || count > 3) return 0;
   if(count == 1)
   {
   	rc = sscanf(the_list[start-1],"%lf %lf %lf",&tmp1,&tmp2,&tmp3);
      if(rc < 0) return 0;
      if(rc < 3) tmp3 = 0.0;
      if(rc < 2) tmp2 = 0.0;
      *val = tmp1 + tmp2/60.0 + tmp3/3600.0;
      return CC_SUCCESS;
   }

   for(i=0;i<count;i++)
   {
   	rc = sscanf(the_list[start-1+i],"%lf",&tmp1);
		if(rc == NULL || rc != 1) return 0;
      out_val += tmp1 / pow(60.0,(double) i);
   }

   *val = out_val;

   return CC_SUCCESS;
}






/******************************************************************

This section contains the internal routines used by
corpscon_convert().

******************************************************************/





/******************************************************************
* Need to document.
******************************************************************/
int ngvd29_to_navd88(double zin, double *zout, double ght, double vht, int g_rc, int v_rc)
{
	if(v_rc != CC_SUCCESS) return v_rc;

   *zout = zin + vht;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int navd88_to_ngvd29(double zin, double *zout, double ght, double vht, int g_rc, int v_rc)
{
	if(v_rc != CC_SUCCESS) return v_rc;

   *zout = zin - vht;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int grs80_to_navd88(double zin, double *zout, double ght, double vht, int g_rc, int v_rc)
{
	if(g_rc != CC_SUCCESS) return g_rc;

   *zout = zin - ght;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int navd88_to_grs80(double zin, double *zout, double ght, double vht, int g_rc, int v_rc)
{
	if(g_rc != CC_SUCCESS) return g_rc;

   *zout = zin + ght;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int nad27_cf(double *cf, double z, double kp, double ght, int g_rc)
{
	*cf = -999.0;

// Not needed since 27 does not use geoid ht
//	if(g_rc != CC_SUCCESS) return CANNOT_CALCULATE_CF;

   *cf = kp * AVERAGE_R / (AVERAGE_R + z);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int nad83_cf(double *cf, double z, double kp, double ght, int g_rc)
{
	*cf = -999.0;

//	if(g_rc != CC_SUCCESS) return CANNOT_CALCULATE_CF;
// Changed so program won't error out to geoid ht is given
	if(g_rc != CC_SUCCESS) return CC_SUCCESS;

   *cf = kp * AVERAGE_R / (AVERAGE_R + z + ght);

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
int dummy_z(double zin, double *zout, double ght, double vht, int g_rc, int v_rc)
{
	*zout = zin;
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall no_geoid_function(double x, double y, double *ght)
{
	*ght = -999.0;

	return NO_GEOID_FUNCTION_SPECIFIED;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall no_vertcon_function(double x, double y, double *vht)
{
	*vht = -999.0;

	return NO_VERTCON_FUNCTION_SPECIFIED;
}


/******************************************************************
* Need to document.
******************************************************************/
int no_cf_function(double *cf, double z, double kp, double ght, int g_rc)
{
	*cf = -999.0;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall dummy_nad(double *xin, double *yin, double *xout, double *yout,
							double *dlas, double *dlos, double *dlam, double *dlom)
{
	*xout = *xin;
   *yout = *yin;
   *dlas = 0.0;
   *dlos = 0.0;
   *dlam = 0.0;
   *dlom = 0.0;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall dummy_grid2geo(int zone, int unit,
													double *lat, double *lon,
													double *north_in, double *east_in,
                              			double *conv, double *kp)
{
	*lat = *north_in;
   *lon = *east_in;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall dummy_usng2geo(double *lat, double *lon,
													double *conv, double *kp,
                                       char *usng_in)
{
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall dummy_geo2grid(int zone, int unit,
													double *lat_in, double *lon_in,
													double *north, double *east,
                              			double *conv, double *kp)
{
	*north = *lat_in;
   *east = *lon_in;

   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall dummy_geo2usng(double lat_in, double lon_in,
													int digits, char *usng_str,
                              			double *conv, double *kp)
{
	strcpy(usng_str,"0ZZZ0000000000");

   return CC_SUCCESS;
}




/******************************************************************
* Routines to retrieve configuration.
******************************************************************/





/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInSystem(void)
{
	return g_config.in.system;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutSystem(void)
{
	return g_config.out.system;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInDatum(void)
{
	return g_config.in.datum;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutDatum(void)
{
	return g_config.out.datum;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInUnits(void)
{
	return g_config.in.units;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutUnits(void)
{
	return g_config.out.units;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInZone(void)
{
	return g_config.in.zone;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutZone(void)
{
	return g_config.out.zone;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInVDatum(void)
{
	return g_config.in.vdatum;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutVDatum(void)
{
	return g_config.out.vdatum;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInVUnits(void)
{
	return g_config.in.vunits;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutVUnits(void)
{
	return g_config.out.vunits;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutUSNGDigits(void)
{
	return g_config.out.usng.precision;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetNadconPath(char *path)
{
	strcpy(path,g_config.nadcon.path);
   return CC_SUCCESS;
}



/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetInHPGNArea(char *area)
{
	strcpy(area,g_config.in.hpgn_area);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetOutHPGNArea(char *area)
{
	strcpy(area,g_config.out.hpgn_area);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetVertconPath(char *path)
{
	strcpy(path,g_config.vertcon.path);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetVertconCustomAreaListFile(char *filename)
{
	strcpy(filename,g_config.vertcon.custom_file);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetUseVertconCustomAreas(void)
{
   return g_config.vertcon.custom;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetGeoidCodeBase(void)
{
   return g_config.geoid_code_base;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetGeoidPath(char *path)
{
	strcpy(path,g_config.geoid.path);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetGeoidCustomAreaListFile(char *filename)
{
	strcpy(filename,g_config.geoid.custom_file);
   return CC_SUCCESS;
}


/******************************************************************
* Need to document.
******************************************************************/
DLLEXPORT int _stdcall GetUseGeoidCustomAreas(void)
{
   return g_config.geoid.custom;
}



