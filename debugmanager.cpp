#include "debugmanager.h"
#include "graphvizbuilder.h"
#include "tracereader.h"
#include "common.h"
#include "templateeventhandler.h"
#include "usedsourcefilemodel.h"
#include <QDir>
#include <QXmlStreamReader>
#include <QString>
#include <QColor>

#include <stack>
#include <exception>

namespace Templar{

DebugManager::DebugManager(QObject *parent)
    : QObject(parent), usedFiles(nullptr)
{
}

void DebugManager::setUsedFileModel(UsedSourceFileModel *usedSourceFiles)
{
    usedFiles = usedSourceFiles;
    for(int i=0;i<this->eventHandlers.size();++i)
    {
        this->eventHandlers.at(i)->SetUsedFileModel(usedSourceFiles);
    }
}

void DebugManager::gotoFile(size_t fileId)
{
    if(usedFiles==nullptr)
        return;
    QMap<size_t, SourceFileNode*>::iterator found = usedFiles->nodeIdMap.find(fileId);
    if(found==usedFiles->nodeIdMap.end())
        return;

    for(int i=0;i<this->eventHandlers.size();++i)
        eventHandlers[i]->gotoFile((*found)->fullPath);
}

void DebugManager::next()
{
    ++entryIterator;
    if(entryIterator!=TraceEntry::end())
    {
        navigationHistory.push_back(entryIterator.currentEntry);
        historyPos = navigationHistory.size()-1;
        for (int i = 0; i < this->eventHandlers.size(); ++i)
            eventHandlers[i]->handleEvent(*entryIterator.currentEntry);

    }
}

void DebugManager::prev(){
    if (historyPos == 0)
        return;

    --historyPos;
    entryIterator = TraceEntry::iterator(navigationHistory[historyPos]);
    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->handleEvent(*entryIterator.currentEntry);
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


