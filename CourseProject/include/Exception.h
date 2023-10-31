#pragma once

#include <iostream>

class Exception : public std::exception
{
	const char* message;

public:
	Exception(const char* msg);

	const char* what();
};