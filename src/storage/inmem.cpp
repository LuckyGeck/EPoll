#include "inmem.h"

#include <assert.h>


namespace NEPoll {

std::vector<TPollInfo> TInMemoryStorage::ListPolls() {
    std::lock_guard<std::mutex> guard(PollsListModifyMutex);
    return std::vector<TPollInfo>(PollsList.begin(), PollsList.end());
}

void TInMemoryStorage::CreatePoll(const TPollInfo& pollInfo) {
    TPollInfoList::iterator insertedIter;
    {
        std::lock_guard<std::mutex> guard(PollsListModifyMutex);
        PollsList.push_back(pollInfo);
        insertedIter = PollsList.end();
        --insertedIter;
    }
    {
        std::lock_guard<std::mutex> guard(Id2PollInfoModifyMutex);
        Id2PollInfo.insert({pollInfo.PollId, insertedIter});
    }
}

TPollInfo TInMemoryStorage::FindPoll(const TPollId& pollId) const {
    return *FindPollIter(pollId);
}

void TInMemoryStorage::VotePoll(const TPollId& pollId, size_t voteId) {
    auto& pollInfo = *FindPollIter(pollId);
    if (pollInfo.Options.size() > voteId) {
        ++pollInfo.Options[voteId].Votes;
    } else {
        throw TNotFoundOptionInPollException();
    }
}

TInMemoryStorage::TPollInfoList::iterator
TInMemoryStorage::FindPollIter(const TPollId &pollId) const {
    std::lock_guard<std::mutex> guard(Id2PollInfoModifyMutex);
    auto it = Id2PollInfo.find(pollId);
    if (it != Id2PollInfo.end()) {
        return it->second;
    } else {
        throw TNotFoundPollException();
    }
}

} // namespace NEPoll
