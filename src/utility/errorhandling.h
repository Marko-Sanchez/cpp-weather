#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <string>

#include "iresult.h"

namespace utility
{
struct ParsingError: public IResult
{
    std::string type;
    std::string message;

    ParsingError(std::string t, std::string m):
    type(t),
    message(m)
    {}

    std::string GetType() const override
    {
        return type;
    }

    std::string GetMessage() const override
    {
        return message;
    }
};

struct NetworkError: public IResult
{
    std::string type;
    std::string message;

    NetworkError(std::string t, std::string m):
    type(t),
    message(m)
    {}

    std::string GetType() const override
    {
        return type;
    }

    std::string GetMessage() const override
    {
        return message;
    }
};
}// namespace utility
#endif
