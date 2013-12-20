#include <QFile>
#include <QXmlStreamReader>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "tracereader.h"
#include "traceentry.h"

namespace Templar {

const QString TemplateBegin = "TemplateBegin";
const QString TemplateEnd = "TemplateEnd";
const QString Position = "PointOfInstantiation";
const QString Context = "Context";
const QString Kind = "Kind";
const QString TimeStamp = "TimeStamp";

void TraceReader::build(QString fileName)
{
    qDebug() << "TraceReader::build fileName=" << fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        throw FileException();
    }
    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();
        TraceEntry entry;
        QString entryType = xml.name().toString();

        if ((entryType == TemplateBegin || entryType == TemplateEnd)
                && xml.isStartElement() )
        {
            entry.isBegin = (entryType == TemplateBegin);
            do {
                // read the informations
                xml.readNext();
                if(xml.name().toString() == Context && xml.isStartElement()){
                    QString str = xml.attributes().value("context").toString();
                    qDebug() << "context=" << str;
                    entry.context = str;//xml.attributes().value("context").toString();
                }
                else if(xml.name().toString() == Position && xml.isStartElement()){
                    entry.position = xml.readElementText();
                }
                else if(xml.name().toString() == Kind && xml.isStartElement()){
                    entry.kind = xml.readElementText();
                }
                else if(xml.name().toString() == TimeStamp && xml.isStartElement()){
                    entry.time = xml.attributes().value("time").toString().toDouble();
                }
            } while( xml.name().toString() != entryType);

            if (entry.kind != "TemplateInstantiation" &&
                entry.kind != "Memoization")
                continue;

            completeEntry(entry);

            if (isIgnored(entry.context))
                continue;


            for (int i = 0; i < builders.size(); ++i)
            {
                builders[i]->traceEntry(entry);
            }
        }
    }
}

void TraceReader::completeEntry(TraceEntry &entry)
{
    if (entry.isBegin)
    {
        entry.id = entryCounter++;
        QStringList fileLoc = entry.position.split("|");
        if (QDir::isRelativePath(fileLoc[0])) {
            entry.position.prepend(dirPath);
        }

        traceEntryStack.push(entry);
    }
    else
    {
        TraceEntry associatedBegin = traceEntryStack.top();
        traceEntryStack.pop();

        entry.id = associatedBegin.id;
        entry.context = associatedBegin.context;
        entry.position = associatedBegin.position;
    }
}

void TraceReader::addBuilder(Builder *builder)
{
    builders.append(builder);
}

bool TraceReader::isIgnored(QString text)
{
    for (int i = 0; i < ignoreList.count(); ++i)
    {
        if (ignoreList[i].exactMatch(text))
            return true;
    }

    return false;
}

void TraceReader::setIgnoreList(const QList<QRegExp>& lst)
{
    ignoreList = lst;
}

} // namespace Templar
