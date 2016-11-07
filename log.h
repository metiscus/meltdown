#pragma once
#include <stdint.h>
#include <string>

namespace Log
{
	enum Group
	{
		None    = 0,
		Core    = 0x1,

		Last
	};

	enum Priority
	{
		Debug,
		Warn,
		Error
	};

	void SetGroupMask(uint32_t mask);
	void SetPriorityMask(uint32_t mask);

	void DEBUG(Group group, const char* str, ...);
	void DEBUG(Group group, const std::string& str, ...);

	void WARN(Group group, const char* str, ...);
	void WARN(Group group, const std::string& str, ...);

	void ERROR(Group group, const char* str, ...);
	void ERROR(Group group, const std::string& str, ...);
}