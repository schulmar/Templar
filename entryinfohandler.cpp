#include "entryinfohandler.h"
#include "traceentry.h"
#include "entryinfo.h"

namespace Templar {

void EntryInfoHandler::handleEvent(const TraceEntry &entry)
{
    showEntry(entry);

    undoStack.push_back(entry);
}

void EntryInfoHandler::undoEvent()
{
    if (undoStack.empty())
        return;

    undoStack.pop_back();

    if (undoStack.empty())
        return;

    TraceEntry entry = undoStack.back();
    showEntry(entry);
}

void EntryInfoHandler::inspect(const TraceEntry& entry)
{
    showEntry(entry);
}

void EntryInfoHandler::showEntry(const TraceEntry &/*entry*/)
{
 /*   entryInfo->setEventType((entry.isBegin)?"Begin":"End");
    entryInfo->setKind(entry.kind);
    entryInfo->setName(entry.context);
    entryInfo->setFilePosition(entry.position);*/
}

void EntryInfoHandler::reset()
{
    undoStack.clear();

 /*   entryInfo->setEventType("");
    entryInfo->setKind("");
    entryInfo->setName("");
    entryInfo->setFilePosition("");*/
}

void EntryInfoHandler::forward(const std::vector<TraceEntry> &vec)
{
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        undoStack.push_back(vec[i]);
    }

    if (!undoStack.empty())
        showEntry(undoStack.back());
}

void EntryInfoHandler::rewind(unsigned int count)
{
    undoStack.erase(undoStack.end() - count, undoStack.end());

    if (!undoStack.empty())
        showEntry(undoStack.back());
}

} // namespace Templar
