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
class UsedSourceFileModel;

class DebugManager : public QObject
{
    Q_OBJECT
public:
    DebugManager(QObject *parent = 0);

    void addEventHandler(TemplateEventHandler* handler);
    int getEventCount() const;

    void inspect(const TraceEntry& entry);
    void selectRoot(const TraceEntry& entry);

    void setBreakpoints(const QList<QRegExp>& breakpoints) {
        this->breakpoints = breakpoints;
    }
    TraceEntry &getEntryTarget() { return traceEntryTarget; }
    const TraceEntry *getCurrentEntry();
    void gotoFile(size_t fileId);
    void setUsedFileModel(UsedSourceFileModel *usedSourceFiles);
    /**
     * @brief Get the entry with the given id
     * @return nullptr if there is no entry with the given id
     */
    TraceEntry const* getEntryById(TraceEntry::Id_t id);
public slots:
    void next();
    void prev();
    void forward();
    void rewind();
    void reset();
    void selectParentRoot();

private:
    bool hasBreakpoint(const QString& str) const;
    void letHandlersHandleEvent(TraceEntry const &entry);

  private:
    QList<TemplateEventHandler*> eventHandlers;
    std::vector<const TraceEntry *> navigationHistory;
    std::vector<const TraceEntry *> rootHistory;

    TraceEntry::iterator entryIterator;
    UsedSourceFileModel *usedFiles;
    TraceEntry traceEntryTarget;
    unsigned int historyPos;

    QList<QRegExp> breakpoints;
};

} // namespace Templar

#endif // DEBUGMANAGER_H
