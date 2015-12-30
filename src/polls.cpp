#include "polls.h"
#include "time_util.h"

#include <storage/inmem.h>

#include <fastcgi2/stream.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>


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

enum class EPollInfoPrintMode { SHORT, FULL };

void PrintPollInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,
                   const NEPoll::TPollInfo& pollInfo, EPollInfoPrintMode mode)
{
    writer.StartObject();
    writer.String("id");
    writer.String(pollInfo.PollId.c_str());
    writer.String("description");
    writer.String(pollInfo.Description.c_str());
    writer.String("created-at");
    writer.String(NTime::TimeToString(pollInfo.Date).c_str());
    if (mode == EPollInfoPrintMode::FULL) {
        writer.String("options");
        writer.StartArray();
        for (const auto& option : pollInfo.Options) {
            writer.StartObject();
            writer.String("text");
            writer.String(option.Text.c_str());
            writer.String("votes");
            writer.Int64(option.Votes.load(std::memory_order_relaxed));
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();
}

inline uint32_t Rand4() {
    return rand() & 0xffff;
}

std::string GenerateGUID() {
    char result[64];
    snprintf(result, 64, "%x%x-%x-%x-%x-%x%x%x",
        Rand4(), Rand4(),             // Generates a 64-bit Hex number
        Rand4(),                       // Generates a 32-bit Hex number
        ((Rand4() & 0x0fff) | 0x4000), // Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
        Rand4() % 0x3fff + 0x8000,     // Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
        Rand4(), Rand4(), Rand4());  // Generates a 96-bit Hex number
    return result;
}

} // anonymous namespace

namespace NEPoll {

TPolls::TPolls()
    : Storage(new TInMemoryStorage())
{
}

TPolls::~TPolls() = default;

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
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.String("polls");
    writer.StartArray();
    for (const auto& pollInfo : Storage->ListPolls()) {
        PrintPollInfo(writer, pollInfo, EPollInfoPrintMode::SHORT);
    }
    writer.EndArray();
    writer.EndObject();
    request->write(sb.GetString(), sb.GetSize());
}

void TPolls::PollCreate(fastcgi::Request *request, fastcgi::HandlerContext *context) {
    std::string inputJson;
    request->requestBody().toString(inputJson);
    rapidjson::Document document;
    document.Parse(inputJson.c_str());
    if (!(document.HasMember("description") && document["description"].IsString() &&
          document.HasMember("options") && document["options"].IsArray() &&
          document["options"].Size() > 0))
    {
        request->setStatus(400);
        return;
    }
    TPollInfo pollInfo;
    pollInfo.PollId = GenerateGUID();
    pollInfo.Description = document["description"].GetString();
    pollInfo.Date = NTime::GetCurrectTime();
    const auto& opts = document["options"];
    for (size_t optionIdx = 0; optionIdx < opts.Size(); ++optionIdx) {
        const auto& curOption = opts[optionIdx];
        if (!curOption.IsString()) {
            request->setStatus(400);
            return;
        }
        pollInfo.Options.emplace_back(curOption.GetString());
    }
    Storage->CreatePoll(pollInfo);
    request->setStatus(201);
    request->setHeader("Location", "/polls/" + pollInfo.PollId);
}

void TPolls::PollInfo(fastcgi::Request *request, fastcgi::HandlerContext *context,
                      const std::string& pollId)
{
    TPollInfo pollInfo;
    try {
        pollInfo = std::move(Storage->FindPoll(pollId));
    } catch (const TNotFoundPollException& ex) {
        request->setStatus(404);
        return;
    }
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    PrintPollInfo(writer, pollInfo, EPollInfoPrintMode::FULL);
    request->write(sb.GetString(), sb.GetSize());
}

void TPolls::PollVote(fastcgi::Request *request, fastcgi::HandlerContext *context,
                      const std::string& pollId, const std::string& voteId)
{
    long voteNum = -1;
    try {
        voteNum = std::stoi(voteId);
    } catch (...) { }

    if (voteNum < 0 || std::to_string(voteNum) != voteId) {
        request->setStatus(404);
        return;
    }
    try {
        Storage->VotePoll(pollId, voteNum);
    } catch (const TNotFoundPollException& ex) {
        request->setStatus(404);
    } catch (const TNotFoundOptionInPollException& ex) {
        request->setStatus(404);
    }
}

} // namespace NEPoll
