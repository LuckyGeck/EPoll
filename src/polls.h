#pragma once

#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <string>


namespace NEPoll {

class TPolls {
public:
    TPolls();

    void ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:
    void ListPolls(fastcgi::Request *request, fastcgi::HandlerContext *context);
    void PollCreate(fastcgi::Request *request, fastcgi::HandlerContext *context);
    void PollInfo(fastcgi::Request *request, fastcgi::HandlerContext *context,
                  const std::string& pollId);
    void PollVote(fastcgi::Request *request, fastcgi::HandlerContext *context,
                  const std::string& pollId, const std::string& voteId);
};

} // namespace NEPoll
