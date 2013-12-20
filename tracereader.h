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
    TraceReader() : entryCounter(0) {}

    void addBuilder(Builder* builder);

    void build(QString fileName);

    void completeEntry(TraceEntry &entry);

    void clearIgnoreList() { ignoreList.clear(); }
    void setIgnoreList(const QList<QRegExp>& list);

    bool isIgnored(QString text);

    void setDirPath(const QString& path) {
        dirPath = path;
    }

    QString getDirPath() const {
        return dirPath;
    }

private:
    QList<Builder*> builders;
    std::stack<TraceEntry> traceEntryStack;

    QString dirPath;

    QList<QRegExp> ignoreList;

    unsigned int entryCounter;
};

class FileException{};

} // namespace Templar

#endif // TRACEREADER_H
