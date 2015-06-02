#ifndef REDIS_EXCEPTION_H
#define REDIS_EXCEPTION_H

#include <exception>
#include <string>

class RedisException : public std::exception
{
    public:
        explicit RedisException(const char* what);
        explicit RedisException(const std::string& what);
        virtual ~RedisException() throw();
        virtual const char* what() const throw();

    private:
        std::string message_;
};

#endif  // REDIS_EXCEPTION_H
