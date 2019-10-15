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

**Windows:** just download from the *releases* section and run the executable. The program should take care of everything by itself.

**Linux:** you need to compile the program first.

## Compile
The program can be compiled with CMake. Run CMake on the source dir and it will generate a ready to build project.

The program needs the following libraries:

* libcurl
* zlib
* libarchive


**Windows:** 
Run *CMake* on the source folder to generate a Visual Studio Project. The CMakeList is set up to compile with static libraries, if you have shared ones change it accordingly. Point the script to the proper library/headers folders when prompted.

**Linux:**
Same procedure. Run *CMake* to generate a makefile, then run *make* to compile.