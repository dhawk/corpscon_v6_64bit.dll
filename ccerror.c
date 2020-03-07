#include <stdio.h>
#include <string.h>
#include "ccerror.h"

DLLEXPORT int _stdcall GetErrorMessage(int err_code, char *msg)
{
char lmsg[256];

	switch(err_code)
   {
   	case CC_SUCCESS:
      		strcpy(lmsg,"Success"); break;
      case NULL_LAT_IN:
      		strcpy(lmsg,"Input latitude is NULL"); break;
      case NULL_LAT_OUT:
      		strcpy(lmsg,"Output latitude is NULL"); break;
      case NULL_LON_IN:
      		strcpy(lmsg,"Input longitude is NULL"); break;
      case NULL_LON_OUT:
      		strcpy(lmsg,"Output longitude is NULL"); break;
      case NULL_NORTH_IN:
      		strcpy(lmsg,"Input northing is NULL"); break;
      case NULL_NORTH_OUT:
      		strcpy(lmsg,"Output northing is NULL"); break;
      case NULL_EAST_IN:
      		strcpy(lmsg,"Input easting is NULL"); break;
      case NULL_EAST_OUT:
      		strcpy(lmsg,"Output easting is NULL"); break;
      case NULL_KP:
      		strcpy(lmsg,"Scale Factor is NULL"); break;
      case NULL_CONV:
      		strcpy(lmsg,"Convergence is NULL"); break;
      case BAD_INPUT_UNIT_SPECIFIED:
      		strcpy(lmsg,"Bad input unit specified"); break;
      case BAD_OUTPUT_UNIT_SPECIFIED:
      		strcpy(lmsg,"Bad output unit specified"); break;
      case NULL_UNIT_FUNCTION:
      		strcpy(lmsg,"Unit function is NULL"); break;
      case BAD_GRIDS83_OPT:
      		strcpy(lmsg,"Bad grids83 Option"); break;
      case BAD_SP83_ZONE:
      		strcpy(lmsg,"Bad state plane 83 Zone"); break;
      case NULL_GRIDS83_FUNCTION:
      		strcpy(lmsg,"Grids83 function is NULL"); break;
      case NULL_GRIDS83_CONST_STRUCT:
      		strcpy(lmsg,"Grids83 constant structure is NULL"); break;
      case NULL_LAM83_CONST_STRUCT:
      		strcpy(lmsg,"Lambert 83 constant structure is NULL"); break;
      case NULL_TM83_CONST_STRUCT:
      		strcpy(lmsg,"Transverse Mercator 83 constant structure is NULL"); break;
      case NULL_OM83_CONST_STRUCT:
      		strcpy(lmsg,"Oblique Mercator 83 constant structure is NULL"); break;
      case BAD_GRIDS27_OPT:
      		strcpy(lmsg,"Bad grids27 Option"); break;
      case BAD_SP27_ZONE:
      		strcpy(lmsg,"Bad state plane 27 Zone"); break;
      case NULL_GRIDS27_FUNCTION:
      		strcpy(lmsg,"Grids27 function is NULL"); break;
      case NULL_SP27_CONST_STRUCT:
      		strcpy(lmsg,"State plane 27 Constant Structure is NULL"); break;
      case CANNOT_OPEN_AREALIST_FILE:
      		strcpy(lmsg,"Cannot open area list file"); break;
      case NULL_VERTCON_AREA_COUNT:
      		strcpy(lmsg,"Vertcon area count is NULL"); break;
      case VERTCON_AREA_COUNT_ZERO_OR_LESS:
      		strcpy(lmsg,"Vertcon area count is <= 0"); break;
      case NULL_VERTCON_GHT:
      		strcpy(lmsg,"Vertcon VHT NULL"); break;
      case VERTCON_OB:
      		strcpy(lmsg,"Point out-of-bounds in Vertcon"); break;
      case NULL_GEOID_AREA_COUNT:
      		strcpy(lmsg,"Geoid area count is NULL"); break;
      case GEOID_AREA_COUNT_ZERO_OR_LESS:
      		strcpy(lmsg,"Geoid area count is <= 0"); break;
      case NULL_GEOID_GHT:
      		strcpy(lmsg,"Geoid GHT NULL"); break;
      case GEOID_OB:
      		strcpy(lmsg,"Point out-of-bounds in Geoid90/93/96"); break;
      case NULL_GEOID_DATA_STRUCT:
      		strcpy(lmsg,"Geoid data struct is NULL"); break;
      case BAD_GEOID_DATA_STRUCT_FP:
      		strcpy(lmsg,"Geoid data struct file pointer is BAD"); break;
      case NULL_GEOID99_AREA_COUNT:
      		strcpy(lmsg,"Geoid99 area count is NULL"); break;
      case GEOID99_AREA_COUNT_ZERO_OR_LESS:
      		strcpy(lmsg,"Geoid99 area count is <= 0"); break;
      case NULL_GEOID99_GHT:
      		strcpy(lmsg,"Geoid99 GHT NULL"); break;
      case GEOID99_OB:
      		strcpy(lmsg,"Point out-of-bounds in Geoid99/03"); break;
      case NULL_GEOID99_DATA_STRUCT:
      		strcpy(lmsg,"Geoid99 data struct is NULL"); break;
      case BAD_GEOID99_DATA_STRUCT_FP:
      		strcpy(lmsg,"Geoid99 data struct file pointer is BAD"); break;
      case NULL_STD_NADCON_AREA_COUNT:
      		strcpy(lmsg,"Nadcon area count is NULL"); break;
      case STD_NADCON_AREA_COUNT_ZERO_OR_LESS:
      		strcpy(lmsg,"Nadcon area count is <= 0"); break;
      case NULL_FP_LAS:
      		strcpy(lmsg,"LAS file pointer is NULL"); break;
      case NULL_FP_LOS:
      		strcpy(lmsg,"LOS file pointer is NULL"); break;
      case NADCON_OB:
      		strcpy(lmsg,"Point out-of-bounds in Nadcon"); break;
      case NULL_NADCON_DATA_STRUCT:
      		strcpy(lmsg,"Nadcon data struct is NULL"); break;
      case NULL_XPT1:
      		strcpy(lmsg,"Nadcon XPT1 is NULL"); break;
      case NULL_YPT1:
      		strcpy(lmsg,"Nadcon YPT1 is NULL"); break;
      case NULL_XPT2:
      		strcpy(lmsg,"Nadcon XPT2 is NULL"); break;
      case NULL_YPT2:
      		strcpy(lmsg,"Nadcon YPT2 is NULL"); break;
      case NULL_DLAS:
      		strcpy(lmsg,"Nadcon DLAS is NULL"); break;
      case NULL_DLOS:
      		strcpy(lmsg,"Nadcon DLOS is NULL"); break;
      case CANNOT_OPEN_LASLOS_FILES:
      		strcpy(lmsg,"Cannot open Nadcon LAS/LOS files"); break;
      case LASLOS_HEADER_NC_UNEQUAL:
      		strcpy(lmsg,"NC unequal in LAS/LOS headers"); break;
      case LASLOS_HEADER_NR_UNEQUAL:
      		strcpy(lmsg,"NR unequal in LAS/LOS headers"); break;
      case LASLOS_HEADER_XO_UNEQUAL:
      		strcpy(lmsg,"XO unequal in LAS/LOS headers"); break;
      case LASLOS_HEADER_DX_UNEQUAL:
      		strcpy(lmsg,"DX unequal in LAS/LOS headers"); break;
      case LASLOS_HEADER_YO_UNEQUAL:
      		strcpy(lmsg,"YO unequal in LAS/LOS headers"); break;
      case LASLOS_HEADER_DY_UNEQUAL:
      		strcpy(lmsg,"DY unequal in LAS/LOS headers"); break;
      case NADCON_ITERATIONS_EXCEEDED:
      		strcpy(lmsg,"Convergence iterations exceeded in Nadcon"); break;
      case BAD_INPUT_SYSTEM_SPECIFIED:
      		strcpy(lmsg,"Bad input system specified"); break;
      case BAD_INPUT_DATUM_SPECIFIED:
      		strcpy(lmsg,"Bad input datum specified"); break;
      case BAD_INPUT_UNITS_SPECIFIED:
      		strcpy(lmsg,"Bad input units specified"); break;
      case BAD_OUTPUT_SYSTEM_SPECIFIED:
      		strcpy(lmsg,"Bad output system specified"); break;
      case BAD_OUTPUT_DATUM_SPECIFIED:
      		strcpy(lmsg,"Bad output datum specified"); break;
      case BAD_OUTPUT_UNITS_SPECIFIED:
      		strcpy(lmsg,"Bad output units specified"); break;
      case BAD_INPUT_VDATUM_SPECIFIED:
      		strcpy(lmsg,"Bad input vertical datum specified"); break;
      case BAD_INPUT_VUNITS_SPECIFIED:
      		strcpy(lmsg,"Bad input vertical units specified"); break;
      case BAD_OUTPUT_VDATUM_SPECIFIED:
      		strcpy(lmsg,"Bad output vertical datum specified"); break;
      case BAD_OUTPUT_VUNITS_SPECIFIED:
      		strcpy(lmsg,"Bad output vertical units specified"); break;
      case BAD_GEOID_CODE_BASE_SPECIFIED:
      		strcpy(lmsg,"Bad Geoid code base specified"); break;
      case DELIMITER_IS_NULL:
      		strcpy(lmsg,"Field delimiter is NULL"); break;
      case BAD_XIN_PARSE_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad Lon/Easting/X input parse field specified"); break;
      case BAD_YIN_PARSE_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad Lat/Northing/Y input parse field specified"); break;
      case BAD_ZIN_PARSE_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad Elevation/Z input parse field specified"); break;
      case BAD_USNGIN_PARSE_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad USNG input parse field specified"); break;
      case BAD_XIN_FIELD_WIDTH_SPECIFIED:
      		strcpy(lmsg,"Bad Lon/Easting/X input field width specified"); break;
      case BAD_YIN_FIELD_WIDTH_SPECIFIED:
      		strcpy(lmsg,"Bad Lat/Northing/Y input field width specified"); break;
      case BAD_XOUT_FIELD_WIDTH_SPECIFIED:
      		strcpy(lmsg,"Bad Lon/Easting/X output field width specified"); break;
      case BAD_YOUT_FIELD_WIDTH_SPECIFIED:
      		strcpy(lmsg,"Bad Lat/Northing/Y output field width specified"); break;
      case BAD_POINT_NAME_IN_PARSE_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad point name input parse field specified"); break;
      case BAD_VALUE_CODE_SPECIFIED:
      		strcpy(lmsg,"Bad value code specified"); break;
      case INPUT_LINE_IS_NULL:
      		strcpy(lmsg,"Input line is NULL"); break;
      case INPUT_LINE_NOT_PARSED:
      		strcpy(lmsg,"Input line not parsed"); break;
      case INPUT_FIELD_X_COULD_NOT_BE_PARSED:
      		strcpy(lmsg,"Lon/Easting/X input field could not be parsed"); break;
      case INPUT_FIELD_Y_COULD_NOT_BE_PARSED:
      		strcpy(lmsg,"Lat/Northing/Y input field could not be parsed"); break;
      case INPUT_FIELD_Z_COULD_NOT_BE_PARSED:
      		strcpy(lmsg,"Elevation/Z input field could not be parsed"); break;
      case INPUT_FIELD_POINT_NAME_COULD_NOT_BE_PARSED:
      		strcpy(lmsg,"Point name input field could not be parsed"); break;
      case NULL_OUTPUT_LINE_FORMAT_STRING:
      		strcpy(lmsg,"Output line format string is NULL"); break;
      case BAD_OUTPUT_LINE_FORMAT_STRING:
      		strcpy(lmsg,"Output line format string is bad"); break;
      case BAD_FIELD_SPECIFIED:
      		strcpy(lmsg,"Bad field specified"); break;
      case KEY_NOT_FOUND:
      		strcpy(lmsg,"Key not found"); break;
      case CANT_OPEN_CONFIG_SCRIPT_FILE:
      		strcpy(lmsg,"Cannot open config script file"); break;
      case NO_GEOID_FUNCTION_SPECIFIED:
      		strcpy(lmsg,"No Geoid function specified"); break;
      case NO_VERTCON_FUNCTION_SPECIFIED:
      		strcpy(lmsg,"No Vertcon function specified"); break;
      case CANNOT_CALCULATE_CF:
      		strcpy(lmsg,"Cannot calculate combined factor"); break;
      case USNG_IN_STRING_NULL:
      		strcpy(lmsg,"USNG input string is null"); break;
      case USNG_IN_STRING_BAD_LENGTH:
      		strcpy(lmsg,"USNG input string length is invalid"); break;
      case USNG_IN_STRING_BAD_GZD_COL:
      		strcpy(lmsg,"USNG input string has bad GZD column"); break;
      case USNG_IN_STRING_BAD_GZD_ROW:
      		strcpy(lmsg,"USNG input string has bad GZD row"); break;
      case USNG_IN_STRING_BAD_SID_COL:
      		strcpy(lmsg,"USNG input string has bad SID column"); break;
      case USNG_IN_STRING_BAD_SID_ROW:
      		strcpy(lmsg,"USNG input string has bad SID row"); break;
      case USNG_IN_STRING_BAD_EAST:
      		strcpy(lmsg,"USNG input string has bad easting"); break;
      case USNG_IN_STRING_BAD_NORTH:
      		strcpy(lmsg,"USNG input string has bad northing"); break;
      case USNG_IN_STRING_CANNOT_CALCULATE_NORTH_MIN:
      		strcpy(lmsg,"Cannot calculate north min from USNG input string"); break;
      case USNG_IN_CANNOT_CONVERT_USNG83_TO_GEO83:
      		strcpy(lmsg,"Cannot convert USNG to geo83"); break;
      case USNG_IN_CANNOT_CONVERT_USNG27_TO_GEO27:
      		strcpy(lmsg,"Cannot convert USNG to geo27"); break;
      case USNG_OUT_CANNOT_CONVERT_GEO83_TO_USNG83:
      		strcpy(lmsg,"Cannot convert geo83 to USNG"); break;
      case USNG_OUT_CANNOT_CONVERT_GEO27_TO_USNG27:
      		strcpy(lmsg,"Cannot convert geo27 to USNG"); break;
      case USNG_OUT_BAD_DIGITS_SPECIFIED:
      		strcpy(lmsg,"Bad USNG digits specified in output"); break;
      case USNG_OUT_BAD_LAT:
      		strcpy(lmsg,"Bad USNG latitude in output"); break;
      default:
      	sprintf(lmsg,"Other Error",err_code);
         break;
   }

   sprintf(msg,"%s (%d).",lmsg,err_code);

	return 1;
}