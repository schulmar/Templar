#ifndef TEMPLATEEVENTHANDLER_H
#define TEMPLATEEVENTHANDLER_H

#include <QString>
#include <vector>

namespace Templar {

struct TraceEntry;
struct UsedSourceFileModel;

class TemplateEventHandler
{
public:
    TemplateEventHandler() : usedFileModel(nullptr) {}
    void SetUsedFileModel(UsedSourceFileModel *model)
    {
        usedFileModel = model;
    }
    virtual void handleEvent(const TraceEntry&) {}
    virtual void undoEvent() {}
    virtual void reset(const TraceEntry &) =0;
    virtual void inspect(const TraceEntry&) {}
    virtual void gotoFile(const QString &) {}
    virtual void forward(const std::vector<TraceEntry>&) {}
    virtual void rewind(unsigned int ) {}

    UsedSourceFileModel *usedFileModel;
};

} // namespace Templar

#endif // TEMPLATEEVENTHANDLER_H
