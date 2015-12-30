#pragma once

#include "data.h"

#include <exception>
#include <vector>


namespace NEPoll {

struct TNotFoundPollException: public std::exception { };
struct TNotFoundOptionInPollException: public std::exception { };

struct IStorage {
    virtual ~IStorage() {}
    virtual std::vector<TPollInfo> ListPolls() = 0;
    virtual void CreatePoll(const TPollInfo& pollInfo) = 0;
    virtual TPollInfo FindPoll(const TPollId& pollId) const = 0;
    virtual void VotePoll(const TPollId& pollId, size_t voteId) = 0;
};

} // namespace NEPoll
