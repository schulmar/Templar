#ifndef ENTRYINFOHANDLER_H
#define ENTRYINFOHANDLER_H

#include "templateeventhandler.h"
#include <utility>
#include <vector>

class EntryInfo;

namespace Templar {

class EntryInfoHandler : public TemplateEventHandler
{
public:
    EntryInfoHandler(EntryInfo */*entryInfo*/) /*todo: unused : entryInfo(entryInfo)*/ {}
    virtual ~EntryInfoHandler() {}

    void handleEvent(const TraceEntry &entry) override;
    void undoEvent() override;
    void reset(const TraceEntry& entry) override;
    void inspect(const TraceEntry& entry) override;
    void forward(const std::vector<TraceEntry> &vec) override;
    void rewind(unsigned int) override;

private:
    void showEntry(const TraceEntry &entry);

private:
    //todo: unused EntryInfo *entryInfo;

    std::vector<TraceEntry> undoStack;
};

} // namespace Templar

#endif // ENTRYINFOHANDLER_H
