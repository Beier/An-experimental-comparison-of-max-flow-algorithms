#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <stdint.h>


#define SHOW_DEBUG_MESSAGES 0
class DebugImpl
{
public:
	DebugImpl& operator<<(char* text);

	DebugImpl& operator<<(uint32_t text);
};

extern DebugImpl Debug;

#endif // !DEBUG_H

//namespace Debug
//{
//	inline void log(char* text);
//}

	//std::ostream& operator<<(std::ostream& text)
	//{
	//	#ifdef _DEBUG
	//			std::cout << text;
	//	#endif
	//	return text;
	//}	

	//Debug& operator<<(void* newLine)
	//{
	//	#ifdef _DEBUG
	//			std::cout << newLine;
	//	#endif
	//	return *this;
	//}