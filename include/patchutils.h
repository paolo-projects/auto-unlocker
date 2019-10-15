#ifndef PATCHUTILS_H
#define PATCHUTILS_H

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <regex>

#include "debugutils.h"
#include "config.h"
#include "circularbuffer.h"

namespace fs = std::filesystem;

namespace Patcher
{
	struct LLQQQQLLQQ // couldn't find a better name LOL
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
	};

	struct QQq // it's starting to feel awkward
	{
		unsigned long long Q1, Q2;
		long long q3;
	};

	struct smc_key_struct
	{
		char s0[4];
		unsigned char B1;
		char s2[4];
		unsigned char B3;
		unsigned char padding[6];
		unsigned long long Q4;
	};

	// Utils
	std::string rot13(const std::string& in);
	std::vector<char> makeVector(const char* arr, size_t size);
	std::string hexRepresentation(long long bin);
	std::string hexRepresentation(std::vector<char> bin);
	std::string hexRepresentation(std::string bin);
	std::optional<unsigned long long> searchForOffset(const std::vector<char>& memstream, const std::vector<char>& sequence);
	std::optional<unsigned long long> searchForLastOffset(const std::vector<char>& memstream, const std::vector<char>& sequence);
	std::optional<unsigned long long> searchForOffset(std::fstream& stream, const std::vector<char>& sequence);
	std::optional<unsigned long long> searchForLastOffset(std::fstream& stream, const std::vector<char>& sequence);
	bool cmpcarr(const char* c1, const char* c2, size_t len);
	void printkey(int i, unsigned long long offset, const smc_key_struct& smc_key, const std::vector<char>& smc_data);

	// Core functions
	bool patchSMC(fs::path name, bool isSharedObj);
	std::pair<unsigned long long, unsigned long long> patchKeys(std::fstream& file, long long key);
	bool patchBase(fs::path name);
	bool patchVmkctl(fs::path name);
	bool patchElf(std::fstream& file, long long oldoffset, long long newoffset);
}

#endif // PATCHUTILS_H