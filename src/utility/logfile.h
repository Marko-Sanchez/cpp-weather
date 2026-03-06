#ifndef LOGFILE_H
#define LOGFILE_H

#include <fstream>
#include <mutex>

namespace utility
{
struct LogFile
{
    std::ofstream file;
    std::mutex logmutex;

    explicit LogFile(const std::string& path):
    file(path, std::ofstream::out | std::ofstream::app)
    {}
};
}// namespace utility
#endif
