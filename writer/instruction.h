#pragma once
#include <unordered_map>
#include <string>
#include <Windows.h>





namespace APICMD
{
#define CBDEF void Callback()
	using Arg_t = size_t;

	
	inline std::vector<Arg_t> GetArgs(std::string cmd)
	{
		std::vector<Arg_t> args;
		std::string delimiter = ",";

		size_t pos = 0;
		std::string token;
		while ((pos = cmd.find(delimiter)) != std::string::npos) {
			token = cmd.substr(0, pos);
			args.push_back(strtoul(token.c_str(), nullptr, 16));
			cmd.erase(0, pos + delimiter.length());
		}
		return args;
	}
	class CMD
	{
	public:
		std::vector<Arg_t> args;
		
		virtual void Callback() = 0;
		inline virtual void Call(std::string cmd_str)
		{
			args = GetArgs(cmd_str);
			Callback();
		}
	};
	//This declares a dword, reference with $
	//i.e: DeclVar ecksdee 0x42069666
	//WriteProcessMemory $ecksdee, ...., ....

	class CMDDeclVar : public CMD
	{
		CBDEF;

	};
	

	class CMDWriteProcessMemory : public CMD
	{
		CBDEF;
		
		
		//void PrintDef();

	};
	class CMDVirtualAllocEx : public CMD
	{
		CBDEF;
	};
	class CMDOpenProcess : public CMD
	{
		CBDEF;
	};
	//not gonna try figuring out an undocumented function sorry
	class CMDCreateRemoteThreadEx : public CMD
	{
		
		CBDEF;
	};

	class CMDCreateBuffer : public CMD
	{
		CBDEF;
	};

	


	inline std::unordered_map<std::string, CMD*> cmd_table = {
		{"WriteProcessMemory", new CMDWriteProcessMemory},
		{"CreateBuffer", new CMDCreateBuffer},
		{"OpenProcess", new CMDOpenProcess},
		{"VirtualAllocEx", new CMDVirtualAllocEx},
		{"CreateRemoteThreadEx", new CMDCreateRemoteThreadEx}

	};
}
