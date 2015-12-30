#pragma once

#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <string>
#include <memory>

namespace NEPoll {

struct IStorage;

class TPolls {
public:
    TPolls();
    ~TPolls();

    void ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:
    void ListPolls(fastcgi::Request *request, fastcgi::HandlerContext *context);
    void PollCreate(fastcgi::Request *request, fastcgi::HandlerContext *context);
    void PollInfo(fastcgi::Request *request, fastcgi::HandlerContext *context,
                  const std::string& pollId);
    void PollVote(fastcgi::Request *request, fastcgi::HandlerContext *context,
                  const std::string& pollId, const std::string& voteId);

private:
    std::unique_ptr<IStorage> Storage;
};

} // namespace NEPoll
