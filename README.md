## 64-bit version of corpscon_v6 DLL

**corpscon_v6.dll** was developed by the Army Geospatial Center of the US Army Corps of Engineers and is widely used by the GIS community for coordinate conversions.  Documentation and downloads are available at 

https://www.agc.army.mil/What-we-do/Corpscon/

Unfortunately the only version of the DLL on the Army website is 32-bit.  The goal of this project was to make a 64-bit version available.  The source files were downloaded from the Army website and the minimal changes necessary were made to compile and link them into a 64-bit DLL using Embarcadero® C++Builder 10.3.3 (Community Edition).   The 64-bit version is renamed **corpscon_v6_64bit.dll** in order to allow the two versions of the DLL to exist in the same directory.

#### Installation

The first step is to install all necessary files from the Army website.  A pre-built copy of **corpscon_v6_64bit.dll** can be downloaded from the GitHub repository (click releases near the top of this webpage).  Place it in the same directory as the other files from the Army website.

#### Status

*It seems to work...*

No test cases are available on the Army website and I have not developed any myself.  The entry points I use in my application appear to function the same as in the 32-bit dll, but overall my testing is minimal.  Keep in mind that this is fifteen year old C code that wouldn't even compile until a minor comment error was fixed (which indicates that this was not the exact source used to generate the 32-bit DLL on the Army website).  That source then generated 124 warnings when compiled using the ancient (1997) Borland C++ 5.02 compiler (which may or may not have been the version used to generate the 32-bit DLL).  My minor changes got that down to 69 warnings with the newer Community Edition compiler.  It is always possible that some of those warnings might indicate something important - so buyer beware.

#### Bug-Reporting

If you find a problem related to the 64-bit conversion I would like to know about it.  Please post an issue describing how the 32 bit version available from the Army website behaves differently from the 64-bit version and let me know what environment you are calling it from.  I'll look at it, but I may or may not have the necessary expertise to fix it.  I almost definitely do not have the expertise necessary to fix any bugs in the original code (such bugs will appear in both versions of the DLL).

#### Licensing

There seem to be no restrictions on the distribution or use of this software given on the Army website, but refer to it for the final say.  My minor modifications are hereby placed in the public domain. 