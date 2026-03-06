#ifndef NETWORKLOGGING_H
#define NETWORKLOGGING_H

#include <httplib.h>

#include "utility/logfile.h"

namespace network
{

#ifdef NDEBUG
    #define ENABLE_LOGGING 0
#else
    #define ENABLE_LOGGING 1
#endif

#if ENABLE_LOGGING
    inline void AttachLoggers(httplib::Client& client, std::shared_ptr<utility::LogFile> lf, const std::string& prefix)
    {
        if (!lf)
        {
            return;
        }

        client.set_logger([lf, prefix](const httplib::Request& req, const httplib::Response& res)
        {
            if (lf->file && lf->file.is_open())
            {
                const std::chrono::time_point now{std::chrono::system_clock::now()};
                auto log = std::format("[INFO][{}] {}\tSTATUS: {} PATH: {}\n",
                        prefix,
                        std::chrono::current_zone()->to_local(now),
                        res.reason,
                        req.path);

                std::lock_guard lck(lf->logmutex);
                lf->file.write(log.c_str(), log.size());
            }
        });

        client.set_error_logger([lf, prefix](const httplib::Error& err, const httplib::Request* req)
        {
            if (lf->file && lf->file.is_open() && req)
            {
                const std::chrono::time_point now{std::chrono::system_clock::now()};
                auto log = std::format("[ERROR][{}] {}\tERROR: {} PATH: {}\n",
                        prefix,
                        std::chrono::current_zone()->to_local(now),
                        httplib::to_string(err),
                        req->path);

                std::lock_guard lck(lf->logmutex);
                lf->file.write(log.c_str(), log.size());
            }
        });
    }
#else
    inline void AttachLoggers(httplib::Client& client, std::shared_ptr<utility::LogFile> lf, const std::string& prefix)
    {}
#endif
}// namespace network
#endif
