
/**
 * This test works by applying the patch to test files located in testing/orig directory.
 * The files are: vmwarebase.dll, vmware-vmx.exe, vmware-vmx-debug.exe
 * They are finally compared to the files in testing/good directory, which have to be patched
 * with a known working patch.
 * The files are not included for copyright reasons.
 */

#include "filesystem.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "filesystem.hpp"
#include "patchutils.h"
#include "installinfoutils.h"
#include "tar.h"

void test_tar();
void do_test_patch();
void test_failed(bool to_cleanup = true);
void test_equalness();
bool test_equal_file(fs::path file1, fs::path file2);
void cleanup();

std::string basepath_str = "";

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		basepath_str = std::string(argv[1]);
		std::cout << "Basepath for test files is: " << basepath_str << "\n";
	}

	test_tar();
	do_test_patch();
	test_equalness();
	cleanup();

	std::cout << "Done.\n";

	return 0;
}

void test_tar()
{
	Tar tarfile((fs::temp_directory_path() / "com.vmware.fusion.zip.tar").string());
	tarfile.extract("com.vmware.fusion.zip", (fs::temp_directory_path() / "com.vmware.fusion.zip").string());
}

void do_test_patch()
{
	fs::path basepath = ".";
	if (basepath_str.length() > 0)
		basepath = basepath_str;

	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path orig_basedir = basepath / "testing" / "orig";
	fs::path dest_basedir = basepath / "testing" / "dst";

	std::cout << "Creating copies of test files into dst directory...\n";

	// Copy original files to destination
	for (const std::string& file : binList) {
		// Skip this file as it's not provided in latest release
		if (file != "vmware-vmx-stats.exe") {
			try {
				fs::copy_file(orig_basedir / file, dest_basedir / file);
			}
			catch (const std::exception& exc) {
				std::cerr << "Copy of file " << file << " failed with error: " << exc.what() << "\n";
				test_failed(false);
			}
		}
	}

	fs::path vmx = dest_basedir / binList[0];
	fs::path vmx_debug = dest_basedir / binList[1];
	//fs::path vmx_stats = vmx_path / binList[2];
	fs::path vmwarebase = dest_basedir / binList[3];

	if (!fs::exists(vmx))
	{
		std::cerr << "Vmx file not found\n";
		test_failed(false);
	}
	if (!fs::exists(vmx_debug))
	{
		std::cerr << "Vmx-debug file not found\n";
		test_failed(false);
	}
	if (!fs::exists(vmwarebase))
	{
		std::cerr << "vmwarebase.dll file not found\n";
		test_failed(false);
	}

	logd("File: " + vmx.filename().string());
	Patcher::patchSMC(vmx, false);

	logd("File: " + vmx_debug.filename().string());
	Patcher::patchSMC(vmx_debug, false);

	logd("File: " + vmwarebase.filename().string());
	Patcher::patchBase(vmwarebase);
}

void test_failed(bool to_cleanup)
{
	if (to_cleanup)
		cleanup();
	exit(1);
}

void test_equalness()
{
	fs::path basepath = ".";
	if (basepath_str.length() > 0)
		basepath = basepath_str;

	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path dest_basedir = basepath / "testing" / "dst";
	fs::path good_basedir = basepath / "testing" / "good";

	for (const std::string& file : binList) {
		// Skip this file as it's not provided in latest release
		if (file != "vmware-vmx-stats.exe") {
			if (!test_equal_file(dest_basedir / file, good_basedir / file)) {
				std::cerr << "Test for file " << file << " failed!\n";
				test_failed();
			}
			else {
				std::cout << "Test for file " << file << " success!\n";
			}
		}
	}
}

bool test_equal_file(fs::path file1, fs::path file2) {
	std::ifstream file1_i(file1, std::ios::binary);
	std::ifstream file2_i(file2, std::ios::binary);

	char buffer1[1024], buffer2[1024];
	size_t read1, read2;

	while (!file1_i.eof() && !file2_i.eof()) {
		file1_i.read(buffer1, 1024);
		read1 = file1_i.gcount();
		file2_i.read(buffer2, 1024);
		read2 = file2_i.gcount();

		if (read1 != read2)
			return false;

		if (!std::equal(buffer1, buffer1 + read1, buffer2))
			return false;
	}
	return true;
}

void cleanup()
{
	std::cout << "Cleaning up test files...\n";

	fs::path basepath = ".";
	if (basepath_str.length() > 0)
		basepath = basepath_str;

	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path dest_basedir = basepath / "testing" / "dst";

	for (const std::string& file : binList) {
		// Skip this file as it's not provided in latest release
		if (file != "vmware-vmx-stats.exe") {
			fs::remove(dest_basedir / file);
		}
	}
}
