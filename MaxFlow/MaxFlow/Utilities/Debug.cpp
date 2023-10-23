#include <iostream>
#include "Debug.h"

DebugImpl Debug;

	DebugImpl&  DebugImpl::operator<<(char* text)
	{
		//#if TARGET_NAME == Debug
		#if SHOW_DEBUG_MESSAGES == 1 && TARGET_NUMBER == 0
				std::cout << text;
		#endif
		return *this;
	}

	DebugImpl&  DebugImpl::operator<<(uint32_t text)
	{
		//#if TARGET_NAME == Debug
		#if SHOW_DEBUG_MESSAGES == 1 && TARGET_NUMBER == 0
				std::cout << text;
		#endif
		return *this;
	}