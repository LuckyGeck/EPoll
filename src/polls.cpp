#include "polls.h"

#include <iostream>
#include <sstream>

namespace NEPoll {

TPolls::TPolls() {

}

void TPolls::ListPolls(fastcgi::Request *request, fastcgi::HandlerContext *context) {
    request->write("All polls", 10);
}

} // namespace NEPoll
