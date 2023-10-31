#include "Exception.h"

Exception::Exception(const char* msg):
	message(msg)
{

}

const char* Exception::what()
{
	return message;
}
