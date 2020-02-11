# Auto-Unlocker
A one-click Unlocker for VMWare Player and Workstation.

**A C++ port of the [DrDonk Python Unlocker](https://github.com/DrDonk/unlocker)**

## DrDonk Unlocker

Unlocker 3 is designed for VMware Workstation 11-15 and Player 7-15.

The patch code carries out the following modifications dependent on the product
being patched:

* Fix vmware-vmx and derivatives to allow macOS to boot
* Fix vmwarebase .dll or .so to allow Apple to be selected during VM creation
* Download a copy of the latest VMware Tools for macOS

Note that not all products recognise the darwin.iso via install tools menu item.
You will have to manually mount the darwin.iso for example on Workstation 11 and Player 7.

In all cases make sure VMware is not running, and any background guests have
been shutdown.

## How to use
I created the C++ version of this tool to avoid issues between different Python versions and OSes.

**Windows:** just download from the *releases* section, extract to a folder (store it if you want to be able to uninstall later) and run the executable. The program should take care of everything by itself.

**Linux:** you can find an experimental x64 .deb package in the latest release. It should work on Debian-derived distributions (including Ubuntu, etc.) although I've not tested it extensively. Install it with apt which will take care of all the dependencies. If it doesn't work for you or if you don't have a Debian-derived distribution you need to compile the program first. After you compiled it, ```chmod +x``` if needed and run ```sudo auto-unlocker```.

**Command line options:**

* ```--install``` *(Default behavior when called with no arguments)* Installs the patch.
* ```--uninstall``` Uninstalls the patch. Works only if the patch was previously installed and the *backup* folder is intact
* ```--help``` Shows a list of the available options

**I tested the program on Windows with VMware Player 15.0.** I plan to test it on Linux soon, though as of now I don't know if it works there, or with other VMware versions.

## Compile
The program can be compiled on Windows with CMake, provided you get or compile the needed libraries first. Run CMake on the source dir and it will generate a ready to build project.

The program needs the following libraries:

* libcurl
* zlib
* libarchive


**Windows:** 
Run *CMake* on the source folder to generate a Visual Studio Project. The CMakeList is set up to compile with static libraries, if you have shared ones change *CMakeLists.txt* file accordingly (**UNLOCKER_STATIC_LIBS_WIN** variable). Point the CMake utility to the proper library/headers paths when prompted or add them to your PATH.

**Linux:**
Use the embedded Makefile. Get needed libraries (or compile them and install) first, then run ```make```

*Note regarding default g++ version on Ubuntu:* by default, Ubuntu ships with g++ v7, which doesn't support natively c++17 filesystem library, although it includes it in the experimental namespace which should work fine. There's a wrapper that should automatically detect if filesystem is natively supported or not and choose the right namespace. If this doesn't work or you have issues try compiling it with g++8 or g++9.