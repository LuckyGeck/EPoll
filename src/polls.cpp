#include "polls.h"

#include <fastcgi2/stream.h>

#include <iostream>

namespace {

const size_t POLL_PREFIX_SIZE = sizeof("/polls/") - 1;

bool IsListPolls(const std::string& path, const std::string& method) {
    return path == "/polls" && method == "GET";
}

bool IsPollCreate(const std::string& path, const std::string& method) {
    return path == "/polls" && method == "POST";
}

bool IsPollInfo(const std::string& path, const std::string& method, std::string* pollId) {
    if (method == "GET" &&
        path.find("/polls/") == 0 && path.find('/', POLL_PREFIX_SIZE) == std::string::npos)
    {
        *pollId = path.substr(POLL_PREFIX_SIZE);
        return true;
    }
    return false;
}

bool IsPollVote(const std::string& path, const std::string& method,
                std::string* pollId, std::string* voteId)
{
    if (method == "POST" && path.find("/polls/") == 0) {
        auto slashLeftPos = path.find('/', POLL_PREFIX_SIZE);
        auto slashRightPos = path.rfind('/');
        if (slashRightPos == slashLeftPos && slashLeftPos > POLL_PREFIX_SIZE) {
            *pollId = path.substr(POLL_PREFIX_SIZE, slashLeftPos - POLL_PREFIX_SIZE);
            *voteId = path.substr(slashLeftPos + 1);
            return true;
        }
    }
    return false;
}

} // anonymous namespace

namespace NEPoll {

TPolls::TPolls() {

}

void TPolls::ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
    auto& path = request->getScriptName();
    auto& method = request->getRequestMethod();
    request->setContentType("application/json");
    std::string pollId;
    std::string voteId;
    if (IsListPolls(path, method)) {
        ListPolls(request, context);
    } else if (IsPollCreate(path, method)) {
        PollCreate(request, context);
    } else if (IsPollInfo(path, method, &pollId)) {
        PollInfo(request, context, pollId);
    } else if (IsPollVote(path, method, &pollId, &voteId)) {
        PollVote(request, context, pollId, voteId);
    } else {
        request->setStatus(404);
    }
}

void TPolls::ListPolls(fastcgi::Request *request, fastcgi::HandlerContext *context) {
    fastcgi::RequestStream stream(request);
    stream << "[\"All polls\"]";
}

void TPolls::PollCreate(fastcgi::Request *request, fastcgi::HandlerContext *context) {
    fastcgi::RequestStream stream(request);
    stream << "[\"Poll create\"]";
}

void TPolls::PollInfo(fastcgi::Request *request, fastcgi::HandlerContext *context,
                      const std::string& pollId)
{
    fastcgi::RequestStream stream(request);
    stream << "[\"Poll info for " << pollId << "\"]";
}

void TPolls::PollVote(fastcgi::Request *request, fastcgi::HandlerContext *context,
                      const std::string& pollId, const std::string& voteId)
{
    fastcgi::RequestStream stream(request);
    stream << "[\"Poll vote " << pollId << " for " << voteId << "\"]";
}

} // namespace NEPoll
