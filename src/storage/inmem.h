#pragma once

#include "interface.h"

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>


namespace NEPoll {

class TInMemoryStorage: public IStorage {
public:
    std::vector<TPollInfo> ListPolls() override;
    void CreatePoll(const TPollInfo& pollInfo) override;
    TPollInfo FindPoll(const TPollId& pollId) const override;
    void VotePoll(const TPollId& pollId, size_t voteId) override;

private:
    using TPollInfoList = std::list<TPollInfo>;

    TPollInfoList::iterator FindPollIter(const TPollId& pollId) const;

private:
    mutable std::mutex PollsListModifyMutex;
    TPollInfoList PollsList;

    mutable std::mutex Id2PollInfoModifyMutex;
    std::unordered_map<TPollId, TPollInfoList::iterator> Id2PollInfo;
};

} // namespace NEPoll
