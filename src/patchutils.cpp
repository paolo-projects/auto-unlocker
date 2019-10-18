#include "patchutils.h"

// Settings for memory and IO related stuff
#define FREAD_BUF_SIZE 2048 // 2 kB

// Flags needed for ELF parsing
#define E_CLASS64 2
#define E_SHT_RELA 4

// Capital letters range from 0x41 to 0x5A
// other letters from 0x61 to 0x7A
// total is 26 -- double of 13

std::string Patcher::rot13(const std::string& in)
{
	std::string out;
	for (char c : in)
	{
		if (c >= 0x41 && c <= 0x5A)
		{
			// CAPITAL LETTER
			out += (((c - 0x41) + 13) % 26) + 0x41;
		}
		else if (c >= 0x61 && c <= 0x7A)
		{
			// NON CAPITAL LETTER
			out += (((c - 0x61) + 13) % 26) + 0x61;
		}
		else
		{
			// NUMBER/SYMBOL/OTHER STUFF
			out += c;
		}
	}
	return out;
}

std::vector<char> Patcher::makeVector(const char* arr, size_t size)
{
	return std::vector<char>(arr, arr+size);
}

bool Patcher::cmpcarr(const char* c1, const char* c2, size_t len)
{
	return std::equal(c1, c1+len, c2);
}

// Different implementations for reusing the same memory stream
std::optional<unsigned long long> Patcher::searchForOffset(const std::vector<char>& memstream, const std::vector<char>& sequence, unsigned long long from)
{
	if (from >= memstream.size() - sequence.size())
		return {};

	auto sRes = std::search(memstream.begin()+from, memstream.end(), sequence.begin(), sequence.end());
	if (sRes != memstream.end())
		return (sRes - memstream.begin());
	else
		return {};
}

std::optional<unsigned long long> Patcher::searchForLastOffset(const std::vector<char>& memstream, const std::vector<char>& sequence)
{
	auto sRes = std::find_end(memstream.begin(), memstream.end(), sequence.begin(), sequence.end());
	if (sRes != memstream.end())
		return (sRes - memstream.begin());
	else
		return {};
}

std::optional<unsigned long long> Patcher::searchForOffset(std::fstream& stream, const std::vector<char>& sequence, unsigned long long from)
{
	std::vector<char> memFile = readFile(stream);

	if (from >= memFile.size() - sequence.size())
		return {};

	auto sRes = std::search(memFile.begin()+from, memFile.end(), sequence.begin(), sequence.end());
	if (sRes != memFile.end())
		return (sRes - memFile.begin());
	else
		return {};
}

std::optional<unsigned long long> Patcher::searchForLastOffset(std::fstream& stream, const std::vector<char>& sequence)
{
	std::vector<char> memFile = readFile(stream);

	auto sRes = std::find_end(memFile.begin(), memFile.end(), sequence.begin(), sequence.end());
	if (sRes != memFile.end())
		return (sRes - memFile.begin());
	else
		return {};
}

std::vector<char> Patcher::readFile(std::fstream& stream)
{
	std::vector<char> memFile;

	stream.clear();
	stream.seekg(std::ios::beg);

	std::array<char, FREAD_BUF_SIZE> buffer{};
	while (!stream.eof())
	{
		stream.read(buffer.data(), buffer.size());
		size_t readBytes = stream.gcount();
		memFile.insert(memFile.end(), buffer.begin(), buffer.begin()+readBytes);
	}
	
	return memFile;
}

std::string Patcher::hexRepresentation(std::string bin)
{
	std::stringstream out;
	for (char c : bin)
	{
		out << std::setfill('0') << std::setw(2) << std::hex << (int)c << " ";
	}
	return out.str();
}

std::string Patcher::hexRepresentation(std::vector<char> bin)
{
	std::stringstream out;
	for (char c : bin)
	{
		out << std::setfill('0') << std::setw(2) << std::hex << (int)c << " ";
	}
	return out.str();
}

std::string Patcher::hexRepresentation(long long bin)
{
	std::stringstream out;

	out << "0x" << std::setfill('0') << std::setw(sizeof(long long)*2) << std::hex << bin;
	
	return out.str();
}

void Patcher::printkey(int i, unsigned long long offset, const smc_key_struct& smc_key, const std::vector<char>& smc_data)
{
	std::stringstream result;
	result << std::setfill('0') << std::setw(3) << (i + 1);
	result << " " << hexRepresentation(offset);
	{
		std::vector<char> arr( smc_key.s0, smc_key.s0 + 4);	// copy into new vector
		std::reverse(arr.begin(), arr.end()); // reverse -- as [::-1] does in python
		std::string arr_str(arr.begin(), arr.end());
		result << " " << arr_str;
	}
	{
		result << " " << std::setfill('0') << std::setw(2) << (int)smc_key.B1;
	}
	{
		std::vector<char> arr(smc_key.s2, smc_key.s2 + 4);	// copy into new vector
		std::reverse(arr.begin(), arr.end()); // reverse -- as [::-1] does in python
		std::replace(arr.begin(), arr.end(), char(0x00), ' '); // replace 0x0 with ' '
		std::string arr_str(arr.begin(), arr.end());
		result << " " << arr_str;
	}
	{
		result << " 0x" << std::setfill('0') << std::setw(2) << std::hex << (int)smc_key.B3;
	}
	{
		result << " " << hexRepresentation(smc_key.Q4);
	}
	result << " " << hexRepresentation(smc_data);

	logd(result.str());
}

void Patcher::patchSMC(fs::path name, bool isSharedObj)
{
	std::fstream i_file(name, std::ios_base::binary | std::ios_base::out | std::ios_base::in);
	if (!i_file.good())
		throw PatchException("Couldn't open file %s", name.c_str());

	long smc_old_memptr = 0;
	long smc_new_memptr = 0;

	// Read file into string variable
	//vmx = f.read();

	logd("Patching file: " + name.filename().string());

	// Setup hex string for vSMC headers
	// These are the private and public key counts
	char smc_header_v0[] = SMC_HEADER_V0;
	char smc_header_v1[] = SMC_HEADER_V1;

	// Setup hex string for #KEY key
	char key_key[] = KEY_KEY;

	// Setup hex string for $Adr key
	char adr_key[] = ADR_KEY;

	// Load up whole file into memory
	std::vector<char> memFile = readFile(i_file);

	// Find the vSMC headers
	auto res = searchForOffset(memFile, makeVector(smc_header_v0, SMC_HEADER_V0_SZ));
	if (!res.has_value())
		throw PatchException("Couldn't find smc_header_v0_offset");

	unsigned long long smc_header_v0_offset = res.value() - 8;

	res = searchForOffset(memFile, makeVector(smc_header_v1, SMC_HEADER_V1_SZ));
	if (!res.has_value())
		throw PatchException("Couldn't find smc_header_v1_offset");
	unsigned long long smc_header_v1_offset = res.value() -8;

	// Find '#KEY' keys
	res = searchForOffset(memFile, makeVector(key_key, KEY_KEY_SZ)); //FIXME: doesn't work
	if (!res.has_value())
		throw PatchException("Couldn't find smc_key0 offset");
	unsigned long long smc_key0 = res.value();

	res = searchForLastOffset(memFile, makeVector(key_key, KEY_KEY_SZ));
	if (!res.has_value())
		throw PatchException("Couldn't find smc_key1 offset");
	unsigned long long smc_key1 = res.value();

	// Find '$Adr' key only V1 table
	res = searchForOffset(memFile, makeVector(adr_key, ADR_KEY_SZ));
	if (!res.has_value())
		throw PatchException("Couldn't find smc_adr offset");
	unsigned long long smc_adr = res.value();

	memFile.clear();

	// Print vSMC0 tables and keys
	logd("appleSMCTableV0 (smc.version = \"0\")");
	logd("appleSMCTableV0 Address      : " + hexRepresentation(smc_header_v0_offset));
	logd("appleSMCTableV0 Private Key #: 0xF2/242");
	logd("appleSMCTableV0 Public Key  #: 0xF0/240");

	if ((smc_adr - smc_key0) != 72)
	{
		logd("appleSMCTableV0 Table        : " + hexRepresentation(smc_key0));
		auto res = patchKeys(i_file, smc_key0);
		smc_old_memptr = res.first;
		smc_new_memptr = res.second;
	}
	else if ((smc_adr - smc_key1) != 72)
	{
		logd("appleSMCTableV0 Table        : " + hexRepresentation(smc_key1));
		auto res = patchKeys(i_file, smc_key1);
		smc_old_memptr = res.first;
		smc_new_memptr = res.second;
	}
	logd("");

	// Print vSMC1 tables and keys
	logd("appleSMCTableV1 (smc.version = \"1\")");
	logd("appleSMCTableV1 Address      : " + hexRepresentation(smc_header_v1_offset));
	logd("appleSMCTableV1 Private Key #: 0x01B4/436");
	logd("appleSMCTableV1 Public Key  #: 0x01B0/432");
	
	if ((smc_adr - smc_key0) == 72)
	{
		logd("appleSMCTableV1 Table        : " + hexRepresentation(smc_key0));
		auto res = patchKeys(i_file, smc_key0);
		smc_old_memptr = res.first, smc_new_memptr = res.second;
	}
	else if ((smc_adr - smc_key1) == 72)
	{
		logd("appleSMCTableV1 Table        : " + hexRepresentation(smc_key1));
		auto res = patchKeys(i_file, smc_key1);
		smc_old_memptr = res.first, smc_new_memptr = res.second;
	}
	logd("");

	// Find matching RELA record in.rela.dyn in ESXi ELF files
	// This is temporary code until proper ELF parsing written
	if (isSharedObj)
	{
		logd("Modifying RELA records from: " + hexRepresentation(smc_old_memptr) + " to " + hexRepresentation(smc_new_memptr));
		patchElf(i_file, smc_old_memptr, smc_new_memptr);
	}

	// Tidy up
	i_file.close();
}

std::pair<unsigned long long, unsigned long long> Patcher::patchKeys(std::fstream& file, long long key)
{
	// Setup struct pack string
	//std::string key_pack = "=4sB4sB6xQ";
		// smc_old_memptr = 0
	unsigned long long smc_new_memptr = 0;

	// Do Until OSK1 read
	int	i = 0;
	while (true)
	{
		// Read key into struct strand data byte str
		long long offset = key + ((unsigned long long)i * 72);
		file.clear();
		file.seekg(offset);
		smc_key_struct smc_key = { 0 };
		file.read(reinterpret_cast<char*>(&smc_key), 24);
		//auto smc_key = structUnpack(key_pack, rr);
		std::vector<char> smc_data;
		auto sz = smc_key.B1;
		smc_data.resize(sz);
		file.read(smc_data.data(), sz );

		// Reset pointer to beginning of key entry
		file.clear();
		file.seekg(offset);
		auto smc_key_0 = smc_key.s0;
		std::string cmp = "SKL+";
		std::string cmp1 = "0KSO";
		std::string cmp2 = "1KSO";
		if (cmpcarr(smc_key_0, cmp.data(), 4))
		{
			// Use the + LKS data routine for OSK0 / 1
			smc_new_memptr = smc_key.Q4;
			logd("+LKS Key: ");
			printkey(i, offset, smc_key, smc_data); // too lazy for this one - but I coded it eventually
		}
		else if (cmpcarr(smc_key_0, cmp1.data(), 4))
		{
			//Write new data routine pointer from + LKS
			logd("OSK0 Key Before:");
			printkey(i, offset, smc_key, smc_data);
			// smc_old_memptr = smc_key[4]
			file.clear();
			file.seekg(offset);
			smc_key_struct towrite = { 0 };
			memcpy(towrite.s0, smc_key.s0, 4);
			towrite.B1 = smc_key.B1;
			memcpy(towrite.s2, smc_key.s2, 4);
			towrite.B3 = smc_key.B3;
			towrite.Q4 = smc_new_memptr;

			file.write(reinterpret_cast<char*>(&towrite), sizeof(smc_key_struct));
			file.flush();

			// Write new data for key
			file.clear();
			file.seekg(offset + 24);
			std::string smc_new_data = rot13(SMC_NEW_DATA); // lulz
			file.write(smc_new_data.data(), smc_new_data.size());
				//f.write(smc_new_data.encode('UTF-8'))
			file.flush();

			// Re - read and print key
			file.clear();
			file.seekg(offset);
			file.read(reinterpret_cast<char*>(&smc_key), 24);
			
			smc_data.clear();
			smc_data.resize(smc_key.B1);
			file.read(smc_data.data(), smc_key.B1);
			logd("OSK0 Key After:");
			printkey(i, offset, smc_key, smc_data);
		}
		else if (cmpcarr(smc_key.s0, cmp2.data(), 4))
		{
			// Write new data routine pointer from + LKS
			logd("OSK1 Key Before:");
			printkey(i, offset, smc_key, smc_data);
			unsigned long long smc_old_memptr = smc_key.Q4;
			
			file.clear();
			file.seekg(offset);

			smc_key_struct towrite = { 0 };
			memcpy(towrite.s0, smc_key.s0, 4);
			towrite.B1 = smc_key.B1;
			memcpy(towrite.s2, smc_key.s2, 4);
			towrite.B3 = smc_key.B3;
			towrite.Q4 = smc_new_memptr;

			file.write(reinterpret_cast<char*>(&towrite), sizeof(smc_key_struct));
			file.flush();

			// Write new data for key
			file.clear();
			file.seekg(offset + 24);
			std::string smc_new_data = rot13(SMC_NEW_DATA2); // so funny
			file.write(smc_new_data.data(), smc_new_data.size());

			file.flush();

			// Re - read and print key
			file.clear();
			file.seekg(offset);

			file.read(reinterpret_cast<char*>(&smc_key), 24);
			
			smc_data.clear();
			smc_data.resize(smc_key.B1);
			file.read(smc_data.data(), smc_key.B1);

			logd("OSK1 Key After:");
			printkey(i, offset, smc_key, smc_data);

			// Finished so get out of loop

			return std::make_pair(smc_old_memptr, smc_new_memptr);
		}

		i += 1;
	}
}

void Patcher::patchBase(fs::path name)
{

	//Patch file
	logd("GOS Patching: " + name.filename().string());
	std::fstream file(name, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	if (!file.good())
		throw PatchException("Couldn't open file %s", name.c_str());

	// Entry to search for in GOS table
	// Should work for Workstation 12 - 15...

	std::vector<char> memFile = readFile(file);
	/* REGEX WAY --- Seems to not work
	std::regex darwin = std::regex(DARWIN_REGEX);
	std::string buf;
	buf.resize(32);

	// Loop through each entryand set top bit
	// 0xBE -- > 0xBF (WKS 12)
	// 0x3E -- > 0x3F (WKS 14)

	unsigned int occurrences = 0;
	auto reg_iter = std::cregex_iterator(memFile.data(), memFile.data()+memFile.size(), darwin);

	for (auto it = reg_iter; it != std::cregex_iterator(); it++)
	{
		std::cmatch match = *it;
		size_t pos = match.position();

		file.clear();
		file.seekg(pos + 32);

		char flag = file.get();
		flag = (flag | (1 << 0));

		file.clear();
		file.seekg(pos + 32);

		file.put(flag);
		logd("GOS Patched flag @: " + hexRepresentation(pos));

		occurrences++;
	} */

	/* Iterative way */
	std::vector<char> darwinPattern[4];

	darwinPattern[0] = DARWIN_PATTERN_PERM_1;
	darwinPattern[1] = DARWIN_PATTERN_PERM_2;
	darwinPattern[2] = DARWIN_PATTERN_PERM_3;
	darwinPattern[3] = DARWIN_PATTERN_PERM_4;
	
	const size_t mStreamLen = memFile.size();
	
	for (int i = 0; i < 4; i++)
	{
		const std::vector<char>& darwinPatt = darwinPattern[i];
		std::optional<unsigned long long> val = 0;
		do
		{
			val = searchForOffset(memFile, darwinPatt, val.value());
			if (val.has_value())
			{
				auto pos = val.value();
				file.clear();
				file.seekg(pos + 32);

				char flag = file.get();
				flag |= 1;

				file.clear();
				file.seekg(pos + 32);

				file.put(flag);
				logd("GOS Patched flag @: " + hexRepresentation(pos));
				val = val.value() + 40;
			}
		} while (val.has_value());
	}

	file.flush();
	file.close();

	logd("GOS Patched: " + name.filename().string());
}

void Patcher::patchVmkctl(fs::path name)
{	
	logd("smcPresent Patching: " + name.filename().string());
	std::fstream file(name, std::ios_base::in | std::ios_base::out);
	
	std::string find_str = VMKCTL_FIND_STR;
	std::vector<char> find_crs(find_str.begin(), find_str.end());
	auto offset = searchForOffset(file, find_crs);

	if (offset.has_value())
	{
		std::string rep_str = VMKCTL_REPLACE_STR;

		file.clear();
		file.seekg(offset.value());
		file.write(rep_str.data(), rep_str.size());

		file.flush();
		file.close();
	}
	else
		throw PatchException("Couldn't find Vmkctl offset");
}

void Patcher::patchElf(std::fstream& file, long long oldoffset, long long newoffset)
{
	file.clear();
	file.seekg(std::ios_base::beg);
	char buf[5];
	memset(buf, 0, 5);
	file.read(buf, 4);
	if (strcmp(buf, "\x7f""ELF") != 0)
	{
		throw PatchException("Not an ELF binary.");
	}
	unsigned char u = file.get();
	if (u != E_CLASS64)
	{
		throw PatchException("Not a 64 bit binary.");
	}
	file.clear();
	file.seekg(40);
	unsigned long long e_shoff;
	file.read(reinterpret_cast<char*>(&e_shoff), sizeof(unsigned long long));
	
	file.clear();
	file.seekg(58);
	unsigned short e_shentsize, e_shnum, e_shstrndx;
	file.read(reinterpret_cast<char*>(&e_shentsize), sizeof(unsigned short));
	file.read(reinterpret_cast<char*>(&e_shnum), sizeof(unsigned short));
	file.read(reinterpret_cast<char*>(&e_shstrndx), sizeof(unsigned short));

	printf("e_shoff: 0x%02llX e_shentsize: 0x%02X e_shnum:0x%02X e_shstrndx:0x%02X\n", e_shoff, e_shentsize,
		e_shnum, e_shstrndx);

	for (unsigned short i = 0; i < e_shnum; i++)
	{
		file.clear();
		file.seekg(e_shoff + i * (unsigned long long)e_shentsize);
		LLQQQQLLQQ e_sh = { 0 };
		
		file.read(reinterpret_cast<char*>(&e_sh), e_shentsize);
		auto e_sh_type = e_sh.l2;
		auto e_sh_offset = e_sh.q3;
		auto e_sh_size = e_sh.q4;
		auto e_sh_entsize = e_sh.q6;
		if (e_sh_type == E_SHT_RELA)
		{
			auto e_sh_nument = int(e_sh_size / e_sh_entsize);
			for (int j = 0; j < e_sh_nument; j++)
			{
				file.clear();
				file.seekg(e_sh_offset + e_sh_entsize * j);
				QQq rela;
				file.read(reinterpret_cast<char*>(&rela), e_sh_entsize);
				auto r_offset = rela.Q1;
				auto r_info = rela.Q2;
				auto r_addend = rela.q3;
				if (r_addend == oldoffset)
				{
					r_addend = newoffset;
					file.clear();
					file.seekg(e_sh_offset + e_sh_entsize * j);
					QQq towr;
					towr.Q1 = r_offset;
					towr.Q2 = r_info;
					towr.q3 = r_addend;
					file.write(reinterpret_cast<char*>(&towr), sizeof(QQq));
					logd("Relocation modified at: " + hexRepresentation(e_sh_offset + e_sh_entsize * j));
				}
			}
		}
	}
}
