#include "debugmanager.h"
#include "common.h"
#include "templateeventhandlers/templateeventhandler.h"
#include "usedsourcefilemodel.h"
#include <QDir>
#include <QXmlStreamReader>
#include <QString>
#include <QColor>

#include <stack>
#include <exception>

namespace Templar{

DebugManager::DebugManager(QObject *parent)
    : QObject(parent), usedFiles(nullptr), historyPos(0)
{
}

void DebugManager::setUsedFileModel(UsedSourceFileModel *usedSourceFiles)
{
    usedFiles = usedSourceFiles;
    for(int i=0;i<this->eventHandlers.size();++i)
    {
        this->eventHandlers[i]->SetUsedFileModel(usedSourceFiles);
    }
}

const TraceEntry *DebugManager::getCurrentEntry() {
  return entryIterator != TraceEntry::iterator{} ? &*entryIterator : nullptr;
}

void DebugManager::gotoFile(size_t fileId)
{
    if(usedFiles==nullptr)
        return;
    QString path;
    try {
    	path = usedFiles->getAbsolutePathOf(fileId);
    } catch (...) {
    	return;
    }

    for(int i=0;i<this->eventHandlers.size();++i)
        eventHandlers[i]->gotoFile(path);
}

TraceEntry const* DebugManager::getEntryById(TraceEntry::Id_t id) {
    auto result =
        std::find_if(traceEntryTarget.begin(), traceEntryTarget.end(),
                     [&id](const TraceEntry &child) { return child.id == id; });
    return result == traceEntryTarget.end() ? nullptr : &*result;
}

void DebugManager::next()
{
    if(!navigationHistory.empty() && historyPos<navigationHistory.size()-1)
    {
        historyPos++;
    }
    else
    {
        ++entryIterator;
        if(entryIterator!=TraceEntry::end())
        {
            navigationHistory.push_back(&*entryIterator);
            historyPos = navigationHistory.size()-1;
        }
    }
    if(navigationHistory.empty() || navigationHistory[historyPos] == nullptr)
        return; // if history still empty, exit this function.
    letHandlersHandleEvent(*navigationHistory[historyPos]);
}

void DebugManager::letHandlersHandleEvent(TraceEntry const &entry) {
  for (int i = 0; i < this->eventHandlers.size(); ++i)
    eventHandlers[i]->handleEvent(entry);
}

void DebugManager::prev() {
    if (historyPos == 0)
        return;

    --historyPos;
    entryIterator = TraceEntry::iterator(navigationHistory[historyPos]);
    letHandlersHandleEvent(*entryIterator);
}

void DebugManager::reset()
{
    navigationHistory.clear();
    historyPos = 0;

    for (int i = 0; i < this->eventHandlers.size(); ++i)
    {
        eventHandlers[i]->reset(traceEntryTarget);
     //   eventHandlers[i]->handleEvent(traceEntryTarget);
    }
}

void DebugManager::selectParentRoot() {
  if (entryIterator->parent) {
    selectRoot(*entryIterator->parent);
  }
}

int DebugManager::getEventCount() const
{
    return navigationHistory.size();
}

void DebugManager::addEventHandler(TemplateEventHandler *handler)
{
    eventHandlers.append(handler);
}

void DebugManager::inspect(const TraceEntry &entry)
{
    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->inspect(entry);
}

void DebugManager::selectRoot(const TraceEntry&entry)
{
    navigationHistory.clear();
    rootHistory.push_back(&entry);
    entryIterator = TraceEntry::iterator(&entry);

    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->selectRoot(entry);
}
void DebugManager::forward()
{
    std::vector<TraceEntry> entryVec;

    while (historyPos < navigationHistory.size())
    {
        const TraceEntry& entry = *navigationHistory[historyPos++];

        entryVec.push_back(entry);
        if (hasBreakpoint(entry.context)) {
            break;
        }
    }

    if (!entryVec.empty())
    {
        for (int i = 0; i < this->eventHandlers.size(); ++i)
            eventHandlers[i]->forward(entryVec);
    }
}

void DebugManager::rewind()
{
    int count = 0;
    bool breakPointActivated = false;

    while (historyPos > 0)
    {
        ++count;

        const TraceEntry& entry = *navigationHistory[--historyPos];
        if (hasBreakpoint(entry.context) && count > 1) {
            breakPointActivated = true;
            break;
        }
    }

    if (count > 0)
    {
        for (int i = 0; i < this->eventHandlers.size(); ++i)
            eventHandlers[i]->rewind(count);

        if (breakPointActivated && count > 1) {
            next();
        }
    }
}

bool DebugManager::hasBreakpoint(const QString &str) const
{
    for (int i = 0; i < breakpoints.count(); ++i)
    {
        if (breakpoints[i].exactMatch(str))
            return true;
    }
    return false;
}


} // namespace Templar


