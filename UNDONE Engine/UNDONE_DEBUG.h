#pragma once

#ifndef _UNDONE_DEBUG_H
#define _UNDONE_DEBUG_H
#include <string>
#include <iostream>
#include "UNDONE_Engine_declr.h"
using namespace std;

#define UNDONE_DEBUG_OUTPUT

inline void _coutput(std::string message) {
#ifdef UNDONE_DEBUG_OUTPUT
	std::cout<<message;
#endif
};

#define coutput(message) _coutput(std::string("")+message)

string UNDONE_API operator +(string lhs, int rhs);
string UNDONE_API operator +(string lhs, float rhs);
#endif