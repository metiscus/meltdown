#include "log.h"

#include <cstdarg>
#include <cstdio>

namespace Log
{
	namespace {
		uint32_t group_mask    = -1;
		uint32_t priority_mask = -1;

		FILE *log_file = stderr;
	}

	void SetGroupMask(uint32_t mask)
	{
		group_mask = mask;
	}

	void SetPriorityMask(uint32_t mask)
	{
		priority_mask = mask;
	}

	void DEBUG(Group group, const char* str, ...)
	{
		if(((group & group_mask) > 0) && ((Debug & priority_mask) > 0))
		{
			std::string format = "[Debug] : ";
			format += str;
			va_list l;
			va_start(l, str);
			vfprintf(log_file, format.c_str(), l);
			va_end(l);
		}
	}

	void DEBUG(Group group, const std::string& str, ...)
	{
		if(((group & group_mask) > 0) && ((Debug & priority_mask) > 0))
		{
			std::string format = "[Debug] : ";
			format += str;
			va_list l;
			va_start(l, str);
			vfprintf(log_file, format.c_str(), l);
			va_end(l);
		}
	}

	void WARN(Group group, const char* str, ...);
	void WARN(Group group, const std::string& str, ...);

	void ERROR(Group group, const char* str, ...);
	void ERROR(Group group, const std::string& str, ...);

}