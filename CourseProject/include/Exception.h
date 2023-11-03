#pragma once

#include <iostream>

class Exception : public std::exception
{
	std::string message;

public:
	Exception(std::string msg);

	std::string what();
};