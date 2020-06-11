#include "util/processes/ParseCobalt.h"


#include <optional>
#include <map>
#include <stdint.h>

#include "util/filesystem/FileSystem.h"
#include "util/log/Log.h"

#include "common/wrappers.hpp"
#include "common/StringUtils.h"

std::optional<AllocationWrapper> FindBeaconInfoQuick(const MemoryWrapper<>& memory){
	/*IMAGE_DOS_HEADER hdr{};
	auto offset{ memory.Convert<IMAGE_DOS_HEADER>()->e_lfanew };
	if(memory.Convert<IMAGE_DOS_HEADER>()->e_magic != 0x5D4A){
		return std::nullopt;
	}

	auto section{ memory.GetOffset(offset + sizeof(IMAGE_NT_HEADERS)).Convert<IMAGE_SECTION_HEADER>() };
	while(section->Name){
		if(*reinterpret_cast<Punsigned int64>(section->Name) == *reinterpret_cast<Punsigned int64>(".data\0\0")){
			unsigned int dwRawDword{ *memory.GetOffset(section->PointerToRawData + 0x30).Convert<unsigned int>() ^ 0x01000100 };
			unsigned int dwVirDword{ *memory.GetOffset(section->VirtualAddress + 0x30).Convert<unsigned int>() ^ 0x01000100 };
			if(dwRawDword == 0x2E2E2E2E || dwRawDword == 0x69696969){
				auto wrapper{ memory.GetOffset(section->PointerToRawData + 0x30).Convert<unsigned int>().ToAllocationWrapper(4096) };
				if(!wrapper){
				}
				for(size_t idx1 = 0; idx1 < wrapper.GetSize(); idx1++){
					if(dwRawDword == 0x69696969){
						wrapper.SetByte(idx1, wrapper[idx1] ^ 0x69);
					} else{
						wrapper.SetByte(idx1, wrapper[idx1] ^ 0x2E);
					}
				}
				return wrapper;
			} else if(dwRawDword == 0x2E2E2E2E || dwRawDword == 0x69696969){
				auto wrapper{ memory.GetOffset(section->PointerToRawData + 0x30).Convert<unsigned int>().ToAllocationWrapper(4096) };
				for(size_t idx1 = 0; idx1 < wrapper.GetSize(); idx1++){
					if(dwRawDword == 0x69696969){
						wrapper.SetByte(idx1, wrapper[idx1] ^ 0x69);
					} else{
						wrapper.SetByte(idx1, wrapper[idx1] ^ 0x2E);
					}
				}
				return wrapper;
			} else{
				return std::nullopt;
			}
		}
		section = section.GetOffset(sizeof(IMAGE_SECTION_HEADER));
	}*/
	return std::nullopt;
}

std::optional<AllocationWrapper> FindBeaconInfo(const MemoryWrapper<>& memory){

	// Experimentation shows that the configuration information is located 0x30 bytes into the .data section
	// Check that before searching the entire module
	auto quick{ FindBeaconInfoQuick(memory) };
	if(quick){
		return quick;
	}

	uint64_t start{ 0x020001000100 };
	uint64_t xor3{ 0x696969696969 };
	uint64_t xor4{ 0x2E2E2E2E2E2E };
	for(size_t idx = 0; idx < memory.MemorySize - 8; idx++){
		auto pattern{ (*memory.GetOffset(idx).Convert<uint64_t>() & 0xFFFFFFFFFFFF) ^ start };
		if(pattern == xor3 || pattern == xor4){
			LOG_VERBOSE(2, "Beacon magic bytes found in memory");
			auto wrapper{ memory.GetOffset(idx).ToAllocationWrapper(4096) };
			for(size_t idx1 = 0; idx1 < wrapper.GetSize(); idx1++){
				if(pattern == xor3){
					wrapper.SetByte(idx1, wrapper[idx1] ^ 0x69);
				} else{
					wrapper.SetByte(idx1, wrapper[idx1] ^ 0x2E);
				}
			}
			return wrapper;
		}
	}

	return std::nullopt;
}

std::wstring ToWstringPad(unsigned int value, size_t length = 2){
	wchar_t* buf = new wchar_t[length + 1];
	swprintf(buf, length + 1, (L"%0" + std::to_wstring(length) + L"d").c_str(), value);
	std::wstring str = buf;
	delete[] buf;
	return str;
}

std::string ToStringPad(unsigned int value, size_t length = 2){
	char* buf = new char[length + 1];
	snprintf(buf, length + 1, ("%0" + std::to_string(length) + "d").c_str(), value);
	std::string str = buf;
	delete[] buf;
	return str;
}

// From https://github.com/Sentinel-One/CobaltStrikeParser/blob/master/parse_beacon_config.py
std::map<unsigned int, std::string> names{
	{ 1, "BeaconType" },
	{ 2, "Port" },
	{ 3, "SleepTime" },
	{ 4, "MaxGetSize" },
	{ 5, "Jitter" },
	{ 6, "MaxDNS" },
	{ 7, "PublicKey" },
	{ 8, "C2Server" },
	{ 9, "UserAgent" },
	{ 10, "HttpPostUri" },
	{ 12, "HttpGet_Metadata" },
	{ 13, "HttpPost_Metadata" },
	{ 14, "SpawnTo" },
	{ 15, "PipeName" },
	{ 19, "DNS_Idle" },
	{ 20, "DNS_Sleep" },
	{ 21, "SSH_Host" },
	{ 22, "SSH_Port" },
	{ 23, "SSH_Username" },
	{ 24, "SSH_Password_Plaintext" },
	{ 25, "SSH_Password_Pubkey" },
	{ 26, "HttpGet_Verb" },
	{ 27, "HttpPost_Verb" },
	{ 28, "HttpPostChunk" },
	{ 29, "Spawnto_x86" },
	{ 30, "Spawnto_x64" },
	{ 31, "CryptoScheme" },
	{ 32, "Proxy_Config" },
	{ 33, "Proxy_User" },
	{ 34, "Proxy_Password" },
	{ 35, "Proxy_Behavior" },
	{ 37, "Watermark" },
	{ 38, "bStageCleanup" },
	{ 39, "bCFGCaution" },
	{ 40, "KillDate" },
	{ 43, "bProcInject_StartRWX" },
	{ 44, "bProcInject_UseRWX" },
	{ 45, "bProcInject_MinAllocSize" },
	{ 46, "ProcInject_PrependAppend_x86" },
	{ 47, "ProcInject_PrependAppend_x64" },
	{ 51, "ProcInject_Execute" },
	{ 52, "ProcInject_AllocationMethod" },
	{ 50, "bUsesCookies" },
	{ 54, "HostHeader" },
};

std::map<unsigned int, std::map<unsigned int, std::string>> enums{
	{1, {
		{ 0x0, "HTTP"},
		{ 0x1, "Hybrid HTTP DNS" },
		{ 0x2, "SMB" },
		{ 0x4, "TCP" },
		{ 0x8, "HTTPS"},
		{ 0x10, "Bind TCP"}
	}},
};

std::string PrintHex(unsigned char * lpAddress, unsigned int dwSize){
	std::string string = "";
	for(auto i = 0; i < dwSize; i++){
		char buf[3];
		sprintf(buf, "%02x", lpAddress[i]);
		string += buf;
		string += " ";
	}
	return string;
}

std::string PrettyPrintConfiguration(unsigned char * address, unsigned int dwOffset){
	ushort setting{ *reinterpret_cast<unsigned short *>(address + dwOffset) };
	ushort type{ *reinterpret_cast<unsigned short *>(address + dwOffset + 2) };
	ushort size{ *reinterpret_cast<unsigned short *>(address + dwOffset + 4) };

	// Swap endianness
	setting = ((setting & 0xFF) << 8) | (setting >> 8);
	type = ((type & 0xFF) << 8) | (type >> 8);
	size = ((size & 0xFF) << 8) | (size >> 8);

	std::string name{ names.count(setting) ? names.at(setting) : ("Unknown Setting " + std::to_string(setting)) };
	if(type == 0x0001){ // TYPE_SHORT
		ushort data{ *reinterpret_cast<ushort *>(address + dwOffset + 6) };
		data = ((data & 0xFF) << 8) | (data >> 8);
		if(enums.count(setting) && enums.at(setting).count(data)){
			return name + ": " + enums.at(setting).at(data);
		}
		return name + ": " + std::to_string(data);
	} else if(type == 0x0002){ // TYPE_INT
		unsigned int data{ *reinterpret_cast<uint *>(address + dwOffset + 6) };
		data = ((data & 0xFF) << 24) | ((data & 0xFF00) << 8) | ((data & 0xFF0000) >> 8) | (data >> 24);
		if(enums.count(setting) && enums.at(setting).count(data)){
			return name + ": " + enums.at(setting).at(data);
		}
		return name + ": " + std::to_string(data);
	} else if(type == 0x0003){
		unsigned char * data{ reinterpret_cast<unsigned char *>(address + dwOffset + 6) };
		bool binary = false;
		bool ended = false;
		for(ushort idx = 0; idx < size && !ended && !binary; idx++){
			if(data[idx] == '\0'){
				ended = true;
			} else if(data[idx] > 0x7F || data[idx] < 0x20){
				binary = true;
			}
		}

		if(!binary){
			return name + ": " + std::string{ reinterpret_cast<char*>(data) };
		} else{
			return name + ": " + PrintHex(data, size);
		}
	} else{
		return "";
	}
}

bool DumpBeaconInformation(const MemoryWrapper<>& memory){
	LOG_VERBOSE(3, "Preparing to try to dump cobalt strike configuration");
	auto info{ FindBeaconInfo(memory) };
	if(info){
		// Beacon found; create file to dump to
		time_t t = time(NULL);
		struct tm * time = localtime(&t);
		auto pid{ memory.process };
		auto wFileName{ "bluespawn-" + ToStringPad(time->tm_mon) + "-" + ToStringPad(time->tm_mday) + "-" + ToStringPad(time->tm_year, 4) + "-"
			+ ToStringPad(time->tm_hour) + ToStringPad(time->tm_min) + "-" + ToStringPad(time->tm_sec) + "-PID-" + ToStringPad(pid, 5) };
		auto num = 0ul;
		while(FileSystem::CheckFileExists(wFileName + "-" + std::to_string(num))){
			num++;
		}

		FileSystem::File file{ wFileName + "-" + std::to_string(num) };
		file.Create();

		std::string output{};
		char* memory{ info->GetAsPointer<char>() };
		unsigned int dwOffset{ 0 };
		while(dwOffset < info->GetSize()){
			output += PrettyPrintConfiguration(reinterpret_cast<unsigned char*>(memory), dwOffset) + "\n";
			auto size{ *reinterpret_cast<ushort*>(memory + dwOffset + 4) };
			size = ((size & 0xFF) << 8) | (size >> 8);
			dwOffset += size + 6;
		}
		file.Write(const_cast<char*>(output.c_str()), 0, output.length());

		return true;
	} else return false;
}