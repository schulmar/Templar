#ifndef TRACEREADER_H
#define TRACEREADER_H

#include <QString>
#include <QRegExp>
#include <QList>
#include <stack>
#include "builder.h"

namespace Templar {

class TraceReader
{
public:
    TraceReader(TraceEntry &target)
        : target(target)/*todo: unused, entryCounter(0)*/ {}

    void build(QString fileName);


    void setDirPath(const QString& path) {
        dirPath = path;
    }

    QString getDirPath() const {
        return dirPath;
    }

private:
    TraceEntry &target;
    std::stack<TraceEntry> traceEntryStack;

    QString dirPath;

    //todo: unused unsigned int entryCounter;
};

class FileException{};

} // namespace Templar

#endif // TRACEREADER_H
