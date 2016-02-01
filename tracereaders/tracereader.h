#ifndef TRACEREADER_H
#define TRACEREADER_H

#include <QString>
#include <QRegExp>
#include <QList>
#include <stack>
#include <memory>
#include "usedsourcefilemodel.h"
#include "traceentry.h"

namespace Templar {

class TraceReader
{
public:
    virtual ~TraceReader() = default;

    using BuildReturn = std::unique_ptr<UsedSourceFileModel>;
    static BuildReturn build(QString fileName, TraceEntry &target, QString dirPath);

    virtual BuildReturn build(QString fileName) = 0;


    void setDirPath(const QString& path) {
        dirPath = path;
    }

    QString getDirPath() const {
        return dirPath;
    }

  protected:
    TraceReader(TraceEntry &target)
    : entryCounter(0) {
        childVectorStack.push(&target);
    }

    void beginEntry(traceEntryPtr);

    TraceEntry &endEntry();

    TraceEntry::Duration durationFrom(double);

private:
    std::stack<TraceEntry*> childVectorStack;

    QString dirPath;

    unsigned int entryCounter;
};

QString sourceFileNameFromTraceFileName(QString traceFileName);

TraceEntry::InstantiationKind entryKindFromString(QString name);

class FileException{};

} // namespace Templar

#endif // TRACEREADER_H
