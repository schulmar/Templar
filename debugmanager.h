#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H


#include <string>
#include <vector>

#include <QObject>
#include <QColor>
#include <QList>
#include <QRegExp>

#include "traceentry.h"

class QRegExp;

namespace Templar {

class TemplateEventHandler;

class DebugManager : public QObject
{
    Q_OBJECT
public:
    DebugManager(QObject *parent = 0);

    void addEventHandler(TemplateEventHandler* handler);
    int getEventCount() const;

    void inspect(const TraceEntry& entry);

    void setBreakpoints(const QList<QRegExp>& breakpoints) {
        this->breakpoints = breakpoints;
    }

public slots:
    void next();
    void prev();
    void forward();
    void rewind();
    void reset(const std::vector<TraceEntry>& instHistory);

private:
    bool hasBreakpoint(const QString& str) const;

private:

    QList<TemplateEventHandler*> eventHandlers;

    std::vector<TraceEntry> instantiationHistory;
    unsigned int historyPos;

    QList<QRegExp> breakpoints;
};

} // namespace Templar

#endif // DEBUGMANAGER_H
