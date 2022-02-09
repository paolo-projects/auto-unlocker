#include "ziparchive.h"
#include "test.h"

void zip_extraction();

int main(int argc, char** argv) {
	test_info("Running TEST: Zip Extraction");

	zip_extraction();

#ifdef WIN32
	system("pause");
#endif
	return 0;
}

void zip_extraction() {
	BEGIN_TEST("Testing ZIP Extraction");

	test_status("CWD is: %s", getCwd().c_str());

	if (!fileExists("./test.zip"))
	{
		TEST_ERROR("File test.zip does not exist in current directory");
	}

	try {
		Zip testZip("./test.zip");

		printf("\n");
		testZip.extract("test.file", "./test.file", updateProgress);
		printf("\n");

		remove("./test.file");

		TEST_SUCCESS();
	}
	catch (const std::exception& exc) {
		TEST_ERROR(exc.what());
	}
}