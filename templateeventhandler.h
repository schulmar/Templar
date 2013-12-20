#ifndef TEMPLATEEVENTHANDLER_H
#define TEMPLATEEVENTHANDLER_H

#include <vector>

namespace Templar {

class TraceEntry;

class TemplateEventHandler
{
public:
    virtual void handleEvent(const TraceEntry&) {}
    virtual void undoEvent() {}
    virtual void reset() {}
    virtual void inspect(const TraceEntry&) {}

    virtual void forward(const std::vector<TraceEntry>&) {}
    virtual void rewind(unsigned int ) {}
};

} // namespace Templar

#endif // TEMPLATEEVENTHANDLER_H
