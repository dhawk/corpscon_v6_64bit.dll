#include <windows.h>
#include <dir.h>
#include <_defs.h>
//#include "dllglob.h"
#include "cc_convert.h"
#include "ccerror.h"
#include "ccnad.h"
#include "ccvert.h"
#include "ccgeoid.h"
#include "ccgeoid99.h"
#include "grids27.h"
#include "grids83.h"
#include "usng.h"
#include "gridcons.h"
#include "ccfileutil.h"
#pragma argsused

BOOL WINAPI DllEntryPoint(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
char modulename[MAXPATH];
char dir[MAXDIR];
char drive[MAXDRIVE];
char name[MAXFILE];
char ext[MAXEXT];
char msg[256];

    switch (fdwReason)
	{
        case DLL_PROCESS_ATTACH:
				GetModuleFileName(hDLLInst,modulename,MAXPATH-1);
			   fnsplit(modulename,drive,dir,name,ext);

#ifdef _WIN64
			   if(strcmpi(name,"corpscon_v6_64bit") || strcmpi(ext,".dll"))
		   	   {
				sprintf(msg,"This is the Corpscon6_64bit DLL developed by the "
      							"Engineer Research and Development Center of "
							"the U.S. Army Corps of Engineers and modified for 64 bits by DRH.  The name of "
			                  "the DLL appears to have been changed to "
                           "%s%s.",name,ext);
				   MessageBox(NULL,msg,"Warning",MB_OK | MB_ICONWARNING);
			   }
#endif

#ifdef __WIN32__
			   if(strcmpi(name,"corpscon_v6") || strcmpi(ext,".dll"))
		  	   {
			   	sprintf(msg,"This is the Corpscon6 DLL developed by the "
      							"Engineer Research and Development Center of "
                  			"the U.S. Army Corps of Engineers.  The name of "
			                  "the DLL appears to have been changed to "
                           "%s%s.",name,ext);
				   MessageBox(NULL,msg,"Warning",MB_OK | MB_ICONWARNING);
			   }
#endif
				corpscon_default_config();
            break;

        case DLL_PROCESS_DETACH:
				corpscon_clean_up();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}
