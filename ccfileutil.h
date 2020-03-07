#include <dir.h>

#ifndef CCFILEUTIL_H
#define CCFILEUTIL_H

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define MAXFILELIST 50
#define CCMAXFIELDWIDTH 64
#define CCMAXFIELDCOUNT 50

typedef char CC_FILENAME[MAXPATH];
typedef CC_FILENAME CC_MAXFILELIST[MAXFILELIST];
typedef char CC_FIELD[CCMAXFIELDWIDTH];
typedef CC_FIELD CC_PARSELIST[CCMAXFIELDCOUNT];

int read_cc_area_file(char *area_filename, CC_MAXFILELIST areas);
int cc_parse_line(char *line, char *delimiter, CC_PARSELIST parse_list);

#endif
