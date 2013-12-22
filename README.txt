This application is supplied as-is and with no any kind of warranty.

This is a demo implementation of some of the aspects in Farbman et. al. Coordinates for Instant Image Cloning SIGGRAPH 2009 paper.

This is the Microsoft Windows implementation, tested with Visual Studio 2008 on Windows XP, Vista and 7 (32 and 64 bit versions).
It requires a DirectX 10 or newer GPU.

In order to build this application, one must have the following software packages installed, both binaries and header files:
QT, version 4.5.1 or newer (http://qt.nokia.com), including QT's plugin for Visual Studio
CGAL, version 3.4 or newer (http://www.cgal.org)
BOOST, version 1.36 or newer (http://www.boost.org)

If not defined, you must define the following environment variables:
CGAL_DIR - should point to the root of your CGAL installation folder.
BOOSTROOT - should point to the root of your BOOST installation folder.
QTDIR - should point to the root of your QT installation folder. 

After all other software libraries are installed, simply build the project from within Visual Studio and run.

Packaged with this archive is also GLee (http://elf-stone.com/glee.php), an easy to use free cross-platform extension loading library for OpenGL.

Several DLLs are required at runtime. If you don't have them in your system, you can use the ones in the external_dll folder.

For the Quick Open feature to work, place two images named source.jpg and target.jpg in the working directory of the exectubable,
such images are supplied in the images folder.