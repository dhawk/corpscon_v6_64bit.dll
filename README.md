## 64-bit version of corpscon_v6 DLL

The popular **corpscon_v6.dll** was developed by the Army Geospatial Center of the US Army Corps of Engineers and is widely used by the GIS community for GIS coordinate conversions.  Documentation and downloads are available at 

https://www.agc.army.mil/What-we-do/Corpscon/

Unfortunately the only version of the DLL on the Army website is 32-bit.  The goal of this project was to make a 64-bit version available.  The source files were downloaded from the Army website and the minimal changes necessary were made to compile and link them into a 64-bit DLL using Embarcadero® C++Builder 10.3.3 (Community Edition).   The 64-bit version is renamed **corpscon_v6_64bit.dll** in order to allow the two versions of the DLL to exist in the same directory.

#### Installation

The first step is to install any necessary files from the Army website.  Then download a pre-built copy of **corpscon_v6_64bit.dll** from https://github.com/dhawk/corpscon_v6_64bit.dll/releases/download/v1/corpscon_v6_64bit.dll.  Place it in the same directory as the other files from the Army website.

#### Status

*It seems to work...*

No test cases are available on the Army website and I have not developed any myself.  The entry points I use in my application appear to function the same as in the 32-bit DLL, but otherwise my testing is minimal.  

#### Bug-Reporting

If you find a problem related to the 64-bit conversion I would like to know about it.  Please post an issue at https://github.com/dhawk/corpscon_v6_64bit.dll/issues  describing how the 32 bit version available from the Army website behaves differently from the 64-bit version and let me know what environment you are calling it from.  I'll look at it, but I may or may not have the necessary expertise to fix it.  I almost definitely do not have the expertise necessary to fix any bugs in the original code (such bugs will appear in both versions of the DLL).

#### Licensing

There seem to be no restrictions on the distribution or use of this software given on the Army website, but refer to it for the final say.  My minor modifications are hereby placed in the public domain. 