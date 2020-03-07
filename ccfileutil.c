#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include "ccfileutil.h"
#include "ccerror.h"

/**************************************************************
*  This routine opens an area file and reads it's contents
*  into a CC_MAXFILELIST.
**************************************************************/
int read_cc_area_file(char *area_filename, CC_MAXFILELIST areas)
{
FILE *fp_area;
char line_str[512];
char *line_start;
int area_count=0;

   fp_area = fopen(area_filename,"r");
   if(fp_area == NULL) return CANNOT_OPEN_AREALIST_FILE;

   while(!feof(fp_area))
   {
      strcpy(line_str,"");
   	fgets(line_str,511,fp_area);
      if(feof(fp_area) && strlen(line_str) < 1) continue;
      if(strlen(line_str) < 1) continue;
      line_start = line_str;
      while(strlen(line_start) > 0)
      {
      	if(*line_start == ' ') { line_start++; continue; }
      	if(*line_start == '\t') { line_start++; continue; }
         break;
      }
      if(line_start == NULL) continue;
      if(strlen(line_start) < 1) continue;
      line_start = strrev(line_start);

      while(strlen(line_start) > 0)
      {
      	if(*line_start == ' ') { line_start++; continue; }
      	if(*line_start == '\t') { line_start++; continue; }
      	if(*line_start == '\n') { line_start++; continue; }
         break;
      }

      if(line_start == NULL) continue;
      if(strlen(line_start) < 1) continue;
      line_start = strrev(line_start);

      if(*line_start == '#') continue;

      strcpy(areas[area_count],line_start);
      area_count++;
   }

   fclose(fp_area);

   return area_count;
}

int cc_parse_line(char *line, char *delimiter, CC_PARSELIST parse_list)
{
char *p;
int i=0;

	if(line == NULL) return 0;
   if(delimiter == NULL) return DELIMITER_IS_NULL;

   p = strtok(line,delimiter);
   if(p == NULL) return 0;

   while(p)
   {
   	strcpy(parse_list[i],p);
      p = strtok(NULL,delimiter);
      i++;
   }

   return i;
}
