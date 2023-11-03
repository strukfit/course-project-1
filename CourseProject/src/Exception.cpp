#include "Exception.h"

Exception::Exception(std::string msg):
	message(msg)
{

}

std::string Exception::what()
{
	return message;
}
