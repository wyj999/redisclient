#include "RedisException.h"

#include <execinfo.h>
#include <stdlib.h>

RedisException::RedisException(const char* msg)
    : message_(msg)
{
}

RedisException::RedisException(const std::string& msg)
    : message_(msg)
{
}

RedisException::~RedisException() throw ()
{
}

const char* RedisException::what() const throw()
{
    return message_.c_str();
}

