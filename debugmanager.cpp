#include "debugmanager.h"
#include "graphvizbuilder.h"
#include "tracereader.h"
#include "common.h"
#include "templateeventhandler.h"

#include <QDir>
#include <QXmlStreamReader>
#include <QString>
#include <QColor>

#include <stack>
#include <exception>

namespace Templar{

DebugManager::DebugManager(QObject *parent) : QObject(parent)
{
}

void DebugManager::next()
{
    if (historyPos >= instantiationHistory.size())
        return;

    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->handleEvent(instantiationHistory[historyPos]);

    ++historyPos;
}

void DebugManager::prev(){
    if (historyPos == 0)
        return;

    --historyPos;
    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->undoEvent();
}

void DebugManager::reset(const std::vector<TraceEntry>& instHistory)
{
    instantiationHistory = instHistory;
    historyPos = 0;

    for (int i = 0; i < this->eventHandlers.size(); ++i)
        eventHandlers[i]->reset();
}

int DebugManager::getEventCount() const
{
    return instantiationHistory.size();
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

void DebugManager::forward()
{
    std::vector<TraceEntry> entryVec;

    while (historyPos < instantiationHistory.size())
    {
        const TraceEntry& entry = instantiationHistory[historyPos++];

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

        const TraceEntry& entry = instantiationHistory[--historyPos];
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


