# Auto-Unlocker
A one-click Unlocker for VMWare Player and Workstation.

**Based on the [DrDonk Python Unlocker](https://github.com/DrDonk/unlocker)**

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

**Linux:** you need to compile the program first. After you compiled it, ```chmod +x``` if needed and run ```sudo auto-unlocker```.

**Command line options:**

* ```--install``` *(Default behavior when called with no arguments)* Installs the patch.
* ```--uninstall``` Uninstalls the patch. Works only if the patch was previously installed and the *backup* folder is intact
* ```--help``` Shows a list of the available options

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

*Note regarding default g++ version on Ubuntu:* by default, Ubuntu ships with g++ v7, which doesn't support natively c++17 features. To compile the program you have to install at least g++-8, set CXX variable when running make and add stdc++fs library dependence (```make CXX=g++-8 LIBS=-lstdc++fs```)

Alternatively, you can install g++-9 and avoid adding the library dependency.
