MINI-SCHEME

This is a modified version of the original Mini-scheme interpreter from Atsushi Moriwaki ( moriwaki@kurims.kurims.kyoto-u.ac.jp ) contained in folder minischeme1. It has been modified by Jacques Bailhache ( jacquesbailhache@hotmail.com ). New features have been added such as calls to Windows API functions. It can be compiled to 4 versions :
- Console Linux
- Console Windows
- Windows
- Windows Mobile

The build of the Linux version is made by the script "build-minischeme-linux.sh", producing the executable "minischeme-linux".

The console Windows, Windows and Windows mobile versions can be built with Pelles C 6.00, respectively with the projects minischeme-csl.ppj, minischeme-win32.ppj and minischeme-wm/ppj.
When you build a version after building another, you must use the menu item "Project / Rebuild" to rebuild all the object files for the new version.

For the Windows Mobile version, the file init.scm must be copied to the root of the filesystem.

This Scheme interpreter also contains an interpreter of another language called LPIA, which uses a postfixed notation like Forth. It can be accessed by typing (TOPLPIA). There are LPIA code examples in the files suffixed with ".lpi". These files can be loaded with the instruction LOAD. Example :
LPIA> ("util.lpi" LOAD)
The folder LPIA contains other versions of LPIA interpreters.






