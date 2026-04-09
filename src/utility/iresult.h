#ifndef IRESULT_H
#define IRESULT_H

#include <string>

namespace utility
{
struct IResult
{
    virtual ~IResult() = default;

    virtual std::string GetType() const = 0;
    virtual std::string GetMessage() const = 0;
};
}
#endif
