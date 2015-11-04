#include "polls.h"

#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>
#include <sstream>

class EPoll : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    EPoll(fastcgi::ComponentContext *context)
        : fastcgi::Component(context)
    {}

    virtual void onLoad() {}
    virtual void onUnload() {}

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        request->setContentType("text/html");
        auto& path = request->getScriptName();
        request->write("Called: ", 8);
        request->write(path.c_str(), path.size());
        request->write("</br>", 5);
        request->write("</br>", 5);
        if (path == "/polls") {
            Polls.ListPolls(request, context);
        }
    }

private:
    NEPoll::TPolls Polls;
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("EPollFactory", EPoll)
FCGIDAEMON_REGISTER_FACTORIES_END()