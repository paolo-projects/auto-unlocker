#ifndef PATCHUTILS_H
#define PATCHUTILS_H

#include "filesystem.hpp"
#include <fstream>
#include <optional>
#include <sstream>
#include <regex>
#include <iomanip>
#include <cstdarg>
#include <array>
#include <cstring>

#include "debug.h"
#include "config.h"

#if defined(__GNUC__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#elif defined(_MSC_VER)
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

class PatchException : public std::runtime_error
{
public:
	PatchException(const char* message)
		: std::runtime_error(message)
	{}
	PatchException(const std::string& message)
		: std::runtime_error(message)
	{}
};

// couldn't find a better name LOL
PACK(struct LLQQQQLLQQ
{
	unsigned long l1;
	unsigned long l2;
	unsigned long long q1;
	unsigned long long q2;
	unsigned long long q3;
	unsigned long long q4;
	unsigned long l3;
	unsigned long l4;
	unsigned long long q5;
	unsigned long long q6;
});

// it's starting to feel awkward
PACK(struct QQq
{
	unsigned long long Q1, Q2;
	long long q3;
});

PACK(struct smc_key_struct
{
	char s0[4];
	unsigned char B1;
	char s2[4];
	unsigned char B3;
	unsigned char padding[6];
	unsigned long long Q4;
});

class Patcher {
public:
	// Utils
	static std::string rot13(const std::string& in);
	static std::string hexRepresentation(long long bin);
	static std::vector<char> makeVector(const char* arr, size_t size);
	static std::string hexRepresentation(std::vector<char> bin);
	static std::string hexRepresentation(std::string bin);
	static std::optional<unsigned long long> searchForOffset(const std::vector<char>& memstream, const std::vector<char>& sequence, unsigned long long from = 0);
	static std::optional<unsigned long long> searchForLastOffset(const std::vector<char>& memstream, const std::vector<char>& sequence);
	static std::optional<unsigned long long> searchForOffset(std::fstream& stream, const std::vector<char>& sequence, unsigned long long from = 0);
	static std::optional<unsigned long long> searchForLastOffset(std::fstream& stream, const std::vector<char>& sequence);
	static std::vector<char> readFile(std::fstream& stream);
	static bool cmpcarr(const char* c1, const char* c2, size_t len);
	static void printkey(int i, unsigned long long offset, const smc_key_struct& smc_key, const std::vector<char>& smc_data);

	// Core functions
	static void patchSMC(fs::path name, bool isSharedObj);
	static std::pair<unsigned long long, unsigned long long> patchKeys(std::fstream& file, long long key);
	static void patchBase(fs::path name);
	static void patchVmkctl(fs::path name);
	static void patchElf(std::fstream& file, long long oldoffset, long long newoffset);
};

#endif // PATCHUTILS_H
