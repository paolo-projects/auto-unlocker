#ifndef TEST_H
#define TEST_H

#include <stdarg.h>
#include <stdio.h>
#include <cstring>
#include "colors.h"

#include <sys/stat.h>
#ifdef WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

void test_info(const char* message, ...) {
	va_list argp;
	va_start(argp, message);
	char msg[512] = ANSI_COLOR_BLUE;
	strcat(msg, message);
	strcat(msg, ANSI_COLOR_RESET "\n");
	vprintf(msg, argp);
	va_end(argp);
}

void test_status(const char* message, ...) {
	va_list argp;
	va_start(argp, message);
	char msg[512] = ANSI_COLOR_YELLOW;
	strcat(msg, message);
	strcat(msg, ANSI_COLOR_RESET "\n");
	vprintf(msg, argp);
	va_end(argp);
}

void test_error(const char* message, ...) {
	va_list argp;
	va_start(argp, message);
	char msg[512] = ANSI_COLOR_RED;
	strcat(msg, message);
	strcat(msg, ANSI_COLOR_RESET "\n");
	vprintf(msg, argp);
	va_end(argp);
}

void test_success(const char* message, ...) {
	va_list argp;
	va_start(argp, message);
	char msg[512] = ANSI_COLOR_GREEN;
	strcat(msg, message);
	strcat(msg, ANSI_COLOR_RESET "\n");
	vprintf(msg, argp);
	va_end(argp);
}

void updateProgress(float progress)
{
	printf("Status: %.0f %%  \r", progress * 100);
}

bool fileExists(const std::string& file)
{
#ifdef WIN32
	DWORD dwAttrib = GetFileAttributesA(file.c_str());
	return dwAttrib != INVALID_FILE_ATTRIBUTES;
#elif defined(__linux__)
	return access("./test.tar", F_OK) == 0;
#endif
}

std::string getCwd()
{
	char cwd[1024] = {};
#ifdef WIN32
	GetCurrentDirectoryA(1024, cwd);
#elif defined(__linux__)
	getcwd(cwd, 1024);
#endif
	return std::string(cwd);
}

#define TEST_ERROR(reason) test_error("Test failed: %s", reason); exit(1)
#define TEST_SUCCESS() test_success("Test succeeded")

#define BEGIN_TEST(what) test_status("Testing: %s", what)
#define ASSERT_TRUE(condition) if(!(condition)) { test_error("Test failed at condition: %s", #condition); exit(1); }

#endif // TEST_H