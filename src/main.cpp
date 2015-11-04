#include "polls.h"

#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>


class EPoll : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    EPoll(fastcgi::ComponentContext *context)
        : fastcgi::Component(context)
    {}

    virtual void onLoad() {}
    virtual void onUnload() {}

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        Polls.ScheduleRequest(request, context);
    }

private:
    NEPoll::TPolls Polls;
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("EPollFactory", EPoll)
FCGIDAEMON_REGISTER_FACTORIES_END()