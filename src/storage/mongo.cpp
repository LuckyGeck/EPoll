#include "mongo.h"
#include <mongo/client/dbclient.h>
#include <sstream>

namespace {

NEPoll::TPollInfo ConvertToPollInfo(const mongo::BSONObj& obj) {
    NEPoll::TPollInfo result;
    result.PollId = obj.getStringField("id");
    result.Description = obj.getStringField("description");
    result.Date = static_cast<time_t>(obj["created-at"].numberLong());
    for (const auto& option : obj["options"].Array()) {
        result.Options.push_back({option["text"].str(), (size_t)option["votes"].numberLong()});
    }
    return result;
}

mongo::BSONObj ConvertToBSONObj(const NEPoll::TPollInfo& pollInfo) {
    mongo::BSONObjBuilder result(64);
    result << "id" << pollInfo.PollId.c_str() <<
        "description" << pollInfo.Description.c_str() <<
        "created-at" << (long long)pollInfo.Date;
    mongo::BSONArrayBuilder options;
    for (const auto& option : pollInfo.Options) {
        options << BSON("text" << option.Text.c_str() << "votes" << (long long)option.Votes.load());
    }
    result << "options" << options.arr();
    return result.obj();
}

} // anonymous namespace

namespace NEPoll {

TMongoStorage::TMongoStorage(const char* host)
    : Conn(new mongo::DBClientConnection())
{
    Conn->connect(host);
}

TMongoStorage::~TMongoStorage() = default;

std::vector<TPollInfo> TMongoStorage::ListPolls() {
    std::vector<TPollInfo> result;
    Conn->query([&result](const mongo::BSONObj& obj) {
        result.push_back(ConvertToPollInfo(obj));
    }, "epoll.polls", mongo::Query());
    return result;
}

void TMongoStorage::CreatePoll(const TPollInfo& pollInfo) {
    Conn->insert("epoll.polls", ConvertToBSONObj(pollInfo));
}

TPollInfo TMongoStorage::FindPoll(const TPollId& pollId) const {
    auto poll = Conn->findOne("epoll.polls", BSON("id" << pollId.c_str()));
    if (poll.isEmpty() || !poll.isValid()) {
        throw TNotFoundPollException();
    }
    return ConvertToPollInfo(poll);
}

void TMongoStorage::VotePoll(const TPollId& pollId, size_t voteId) {
    std::stringstream field;
    field << "options." << voteId;
    auto filter = BSON(
        "id" << pollId.c_str() <<
        field.str().c_str() << BSON("$exists" << true)
    );
    field << ".votes";
    auto operation = BSON("$inc" << BSON(field.str().c_str() << 1));
    Conn->update("epoll.polls", filter, operation);
    if (Conn->getLastErrorDetailed().getIntField("n") == 0) {
        throw TNotFoundOptionInPollException();
    }
}

} // namespace NEPoll
