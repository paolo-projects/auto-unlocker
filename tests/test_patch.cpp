
/**
 * This test works by applying the patch to test files located in testing/orig directory.
 * The files are: vmwarebase.dll, vmware-vmx.exe, vmware-vmx-debug.exe
 * They are finally compared to the files in testing/good directory, which have to be patched
 * with a known working patch.
 * The files are not included for copyright reasons.
 */

#include "test.h"
#include "filesystem.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "filesystem.hpp"
#include "patcher.h"
#include "installinfo.h"
#include "tar.h"
#include "unlocker.h"

void test_tar();
void do_test_patch();
void test_failed(bool to_cleanup = true);
void test_equalness();
bool test_equal_file(fs::path file1, fs::path file2);
bool test_download_files();
void cleanup();

std::string basepath_str = "";

int main(int argc, char** argv)
{
	//return test_download_files() ? 0 : 1;

	if (argc > 1)
	{
		basepath_str = std::string(argv[1]);
		test_info("Basepath for test files is: %s", argv[1]);
	}
	else {
		std::string cwd = getCwd();
		test_info("Basepath for test is the cwd: %s", cwd.c_str());
	}

	//test_tar();
	do_test_patch();
	test_equalness();
	cleanup();

	test_info("Done.");

	return 0;
}

void test_tar()
{
	Tar tarfile((fs::temp_directory_path() / "com.vmware.fusion.zip.tar").string());
	tarfile.extract("com.vmware.fusion.zip", (fs::temp_directory_path() / "com.vmware.fusion.zip").string());
}

void do_test_patch()
{
	BEGIN_TEST("the patch by comparing with a known working patch\n" \
		"This test needs the following directories in the `testing` folder:\n" \
		"\torig -> the original files\n" \
		"\tgood -> the files patched with a known working patch\n" \
		"In both folders the following files are needed (for Windows):\n" \
		"\tvmwarebase.dll\n" \
		"\tvmware-vmx.exe\n" \
		"\tvmware-vmx-debug.exe");

#ifndef _WIN32
	TEST_ERROR("This test is only for windows os");
#endif

	fs::path basepath = ".";
	if (basepath_str.length() > 0) {
		basepath = basepath_str;
	}

	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path orig_basedir = basepath / "testing" / "orig";
	fs::path dest_basedir = basepath / "testing" / "dst";

	test_info("Creating copies of test files into dst directory...");

	// Copy original files to destination
	for (const std::string& file : binList) {
		// Skip this file as it's not provided in latest release
		if (file != "vmware-vmx-stats.exe") {
			try {
				fs::copy_file(orig_basedir / file, dest_basedir / file, fs::copy_options::overwrite_existing);
			}
			catch (const std::exception& exc) {
				test_error("Copy of file %s failed with error: %s", file.c_str(), exc.what());
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
		test_error("Vmx file not found");
		test_failed(false);
	}
	if (!fs::exists(vmx_debug))
	{
		test_error("Vmx-debug file not found");
		test_failed(false);
	}
	if (!fs::exists(vmwarebase))
	{
		test_error("vmwarebase.dll file not found\n");
		test_failed(false);
	}

	test_info("File: %s", vmx.filename().string().c_str());
	Patcher::patchSMC(vmx, false);

	test_info("File: %s", vmx_debug.filename().string().c_str());
	Patcher::patchSMC(vmx_debug, false);

	test_info("File: %s", vmwarebase.filename().string().c_str());
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
				test_error("Test for file %s failed!", file.c_str());
				test_failed();
			}
			else {
				test_success("Test for file %s success!\n", file.c_str());
			}
		}
	}
}

bool test_equal_file(fs::path file1, fs::path file2) {
	/*std::ifstream file1_i(file1, std::ios::binary);
	std::ifstream file2_i(file2, std::ios::binary);*/
	constexpr size_t FBUF_SIZE = 16 * 1024;

	FILE* f1, * f2;
	f1 = fopen(file1.string().c_str(), "rb");
	f2 = fopen(file2.string().c_str(), "rb");

	if (f1 == NULL || f2 == NULL) {
		return false;
	}

	std::array<char, FBUF_SIZE> buffer1, buffer2;
	size_t read1, read2;

	bool result = true;

	while (!feof(f1) && !feof(f2)) {
		read1 = fread(buffer1.data(), 1, FBUF_SIZE, f1);
		read2 = fread(buffer2.data(), 1, FBUF_SIZE, f2);

		if (read1 != read2 ||
			!std::equal(buffer1.begin(), buffer1.begin() + read1, buffer2.begin())) {
			result = false;
			break;
		}
	}

	fclose(f1);
	fclose(f2);
	return result;
}

bool test_download_files() {
	try {
		return downloadTools(fs::path("./tools"));
	}
	catch (const std::exception& exc) {
		fprintf(stderr, exc.what());
		return false;
	}
}

void cleanup()
{
	test_info("Cleaning up test files...");

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
