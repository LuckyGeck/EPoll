#pragma once

#include "interface.h"

#include <list>
#include <string>
#include <vector>
#include <memory>

namespace mongo {
class DBClientConnection;
} // namespace mongo

namespace NEPoll {

class TMongoStorage: public IStorage {
public:
    TMongoStorage(const char* host);
    ~TMongoStorage();

    std::vector<TPollInfo> ListPolls() override;
    void CreatePoll(const TPollInfo& pollInfo) override;
    TPollInfo FindPoll(const TPollId& pollId) const override;
    void VotePoll(const TPollId& pollId, size_t voteId) override;

private:
    std::unique_ptr<mongo::DBClientConnection> Conn;
};

} // namespace NEPoll
