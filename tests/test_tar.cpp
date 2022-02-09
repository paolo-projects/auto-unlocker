#include "tar.h"
#include "test.h"

void tar_extraction();
void testTarSearch();

int main(int argc, char** argv) {
	test_info("Running TEST: Tar Extraction");

	tar_extraction();
	//testTarSearch();

#ifdef WIN32
	system("pause");
#endif
	return 0;
}

void tar_extraction() {
	BEGIN_TEST("Testing TAR Extraction");

	test_status("CWD is: %s", getCwd().c_str());

	if(!fileExists("./test.tar"))
	{
		TEST_ERROR("File test.tar does not exist in current directory");
	}

	try {
		Tar testTar("./test.tar");

		ASSERT_TRUE(testTar.contains("test.file"));

		printf("\n");
		testTar.extract("test.file", "./test.file", updateProgress);
		printf("\n");

		remove("./test.file");

		TEST_SUCCESS();
	}
	catch (const std::exception& exc) {
		TEST_ERROR(exc.what());
	}
}

void testTarSearch()
{
	try {
		Tar testTar("./test.tar");

		BEGIN_TEST("Testing Tar Search: test.file");
		auto res = testTar.search("test.file");

		bool found = false;
		for (const Tar::File& f : res) {
			if (f.name == "test.file") {
				found = true;
				break;
			}
		}
		ASSERT_TRUE(res.size() == 1);
		ASSERT_TRUE(found);
		TEST_SUCCESS();

		BEGIN_TEST("Testing Tar Search: fff");
		auto res2 = testTar.search("fff");
		found = false;
		for (const Tar::File& f : res2) {
			if (f.name == "test/fff.txt") {
				found = true;
				break;
			}
		}
		ASSERT_TRUE(res2.size() == 1);
		ASSERT_TRUE(found);

		TEST_SUCCESS();
	}
	catch (const std::exception& exc) {
		TEST_ERROR(exc.what());
	}
}