# Auto-Unlocker
A one-click Unlocker for VMWare Player and Workstation.

**A C++ port of the [DrDonk Python Unlocker](https://github.com/DrDonk/unlocker)**

## The tool

This project is born to provide a native solution, mainly on Windows, to the original Unlocker project. 
Since the original project requires python, and it's not installed by default on windows, this tool has 
been coded to provide an all-in-one solution with no additional dependencies needed.

It relies on libcurl for the networking (get requests and file download) and libzip	for the zip extraction.
The tar extraction, being relatively easy to implement, has been coded from	scratch. 
The libraries are linked statically to provide one final executable for the sake of simplicity.

The Windows target features a GUI implemented throught the Win32 API.
Although this solution brings more complexity than using one of the many GUI libraries out there,
the resulting executable will be smaller and with better compatibility.

For the linux target, it is still shell-based, given the better-instructed user base running
this tool on linux os-es.

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

**Windows (GUI):** just download from the *releases* section, extract to a folder (store it if you want to be able to uninstall later) and run the executable.

**Linux (shell):** You need to compile the program first. After you compiled it, ```chmod +x``` if needed and run ```sudo ./auto-unlocker```. Refer to the next section for more info about compiling the tool.

**Command line options (linux):**

* ```--install``` *(Default behavior when called with no arguments)* Installs the patch.
* ```--uninstall``` Uninstalls the patch. Works only if the patch was previously installed and the *backup* folder is intact
* ```--download-tools``` Downloads the tools in the `tools` folder
* ```--help``` Shows a list of the available options

## Compile
The program can be compiled on Windows with CMake, provided you get or compile the needed libraries first. Run CMake on the source dir and it will generate a ready to build project.

The program needs the following libraries:

* libcurl
* libzip

The tar extraction is handled internally, while the zip decompression needs the `libzip` library. `libcurl` is used for downloading the tools.

**Windows:** 
Run *CMake* on the source folder to generate a Visual Studio Project. The CMakeList is set up to compile with static libraries, if you have shared ones change *CMakeLists.txt* file accordingly (**UNLOCKER_STATIC_LIBS_WIN** variable). Point the CMake utility to the proper library/headers paths when prompted or add them to your PATH.

**Linux:**
Use the embedded Makefile. Get needed libraries (or compile them and install) first, then run ```make```

On Ubuntu:

```bash
sudo apt-get install libcurl4-openssl-dev libzip-dev
git clone https://github.com/paolo-projects/auto-unlocker
cd auto-unlocker
make
chmod +x ./auto-unlocker
sudo ./auto-unlocker
```