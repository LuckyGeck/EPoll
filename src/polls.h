#pragma once

#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

namespace NEPoll {

class TPolls {
public:
    TPolls();

    void ListPolls(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:
};

} // namespace NEPoll
