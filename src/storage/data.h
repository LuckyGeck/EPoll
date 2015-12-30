#pragma once

#include <atomic>
#include <ctime>
#include <string>
#include <vector>


namespace NEPoll {

using TPollId = std::string;

struct TPollOption {
    TPollOption(const TPollOption& rhs) {
        Text = rhs.Text;
        Votes.store(rhs.Votes.load());
    }

    TPollOption(TPollOption&& rhs) {
        Text = std::move(rhs.Text);
        Votes.store(rhs.Votes.load());
    }

    std::string Text;
    std::atomic<size_t> Votes;
};

struct TPollInfo {
    TPollId PollId;
    std::string Description;
    std::time_t Date;
    std::vector<TPollOption> Options;
};

} // namespace NEPoll
