#include <QFile>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QXmlStreamReader>
#include <cassert>

#include "tracereader.h"
#include "traceentry.h"

namespace Templar {
QDataStream &operator>>(QDataStream &input, SourceLocation &location)
{
    qint64 fileId;
    input>>fileId;
    location.fileId = static_cast<size_t>(fileId);
    input>>location.line;
    input>>location.col;
    return input;
}

void TraceReader::build(QString fileName) {
    if (fileName.endsWith("xml")) {
        buildFromXML(fileName);
    } else {
        buildFromBinary(fileName);
    }
}

static const char* TemplateBegin = "TemplateBegin";
static const char* TemplateEnd = "TemplateEnd";
static const char* Position = "PointOfInstantiation";
static const char* Context = "Context";
static const char* Kind = "Kind";
//static const char* TimeStamp = "TimeStamp";
static const char* Memory = "MemoryUsage";

struct SourceFileManagerAccumulator {

    SourceFileManagerAccumulator(QString dirPath) : dirPath(dirPath) {}

    SourceLocation parsePosition(QXmlStreamReader &xml) {
        assert(xml.name().toString() == Position);
        auto components = xml.readElementText().split('|');
        QFileInfo fileInfo(components[0]);
        if(fileInfo.isRelative()) {
          fileInfo = QDir(dirPath).filePath(components[0]);
        }
        QString sourceFile = fileInfo.canonicalFilePath();
        int line = components[1].toInt();
        int col = components[2].toInt();
        auto iterator = sourceFileIds.find(sourceFile);
        if (iterator == sourceFileIds.end()) {
            iterator =
                sourceFileIds.insert(std::make_pair(sourceFile,
                                                    sourceFiles.size())).first;
            sourceFiles.push_back(sourceFile);
        }
        return SourceLocation{iterator->second, line, col};
    }

    TraceReader::SourceFiles sourceFiles;
    std::map<QString, size_t> sourceFileIds;
    QString dirPath;
};

TraceReader::SourceFiles
TraceReader::readSourceFilesFromXML(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException();
    }
    QXmlStreamReader xml(&file);
    SourceFileManagerAccumulator sourceFilesAccumulator(
        QFileInfo(fileName).dir().path());

    while (!xml.atEnd()) {
        xml.readNext();
        QString entryType = xml.name().toString();
        if ((entryType == TemplateBegin) &&
            xml.isStartElement()) {
            do {
                xml.readNext();
                if (xml.name().toString() == Position && xml.isStartElement()) {
                    sourceFilesAccumulator.parsePosition(xml);
                }
            } while (xml.name().toString() != entryType);
        }
    }
    return sourceFilesAccumulator.sourceFiles;
}

TraceEntry::InstantiationKind entryKindFromString(QString name) {
#define MAP_ENTRY(NAME) {#NAME, TraceEntry::NAME}
    std::map<QString, TraceEntry::InstantiationKind> map = {
        MAP_ENTRY(TemplateInstantiation),
        MAP_ENTRY(DefaultFunctionArgumentInstantiation),
        MAP_ENTRY(ExplicitTemplateArgumentSubstitution),
        MAP_ENTRY(DeducedTemplateArgumentSubstitution),
        MAP_ENTRY(PriorTemplateArgumentSubstitution),
        MAP_ENTRY(DefaultTemplateArgumentChecking),
        MAP_ENTRY(ExceptionSpecInstantiation),
        MAP_ENTRY(Memoization),
    };
#undef MAP_ENTRY
    auto nameIter = map.find(name);
    return nameIter != map.end() ? nameIter->second : TraceEntry::Unknown;
}

void TraceReader::buildFromXML(QString fileName) {
    qDebug() << "TraceReader::build fileName=" << fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException();
    }
    QXmlStreamReader xml(&file);

    SourceFileManagerAccumulator sourceFilesAccumulator(dirPath);

    std::stack<TraceEntry *> childVectorStack;
    childVectorStack.push(&target);
    int counter = 0;

    while (!xml.atEnd()) {
        xml.readNext();
        QString entryType = xml.name().toString();

        if (xml.isStartElement()) {
            if (entryType == TemplateBegin) {
                traceEntryPtr newEntry(new TraceEntry{});
                newEntry->id = counter++;
                do {
                    xml.readNext();
                    if (xml.name().toString() == Context &&
                        xml.isStartElement()) {
                        newEntry->context =
                            xml.attributes().value("context").toString();
                    } else if (xml.name().toString() == Position &&
                               xml.isStartElement()) {
                        newEntry->instantiation =
                            sourceFilesAccumulator.parsePosition(xml);
                        newEntry->instantiationEnd =
                            newEntry->instantiationBegin =
                                newEntry->instantiation;
                        /* todo:
                        newEntry->declarationBegin;
                        newEntry->declarationEnd;
                        */
                    } else if (xml.name().toString() == Kind &&
                               xml.isStartElement()) {
                        newEntry->kind =
                            entryKindFromString(xml.readElementText());
                    }
                } while (xml.name().toString() != entryType);

                newEntry->parent = childVectorStack.top();
                childVectorStack.top()->children.push_back(newEntry);
                childVectorStack.push(
                    childVectorStack.top()->children.last().data());
            } else if (entryType == TemplateEnd) {
                TraceEntry &lastEntry = *childVectorStack.top();
                do {
                    xml.readNext();
                    if (xml.name().toString() == Memory) {
                        lastEntry.memoryUsage =
                            xml.attributes().value("context").toLongLong();
                    }
                } while (xml.name().toString() != entryType);
                childVectorStack.pop();
            }
        }
    }
}

void TraceReader::buildFromBinary(QString fileName) {
    qDebug() << "TraceReader::build fileName=" << fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        throw FileException();
    }
    QDataStream input(&file);
    input.setByteOrder(QDataStream::LittleEndian);

    std::stack<TraceEntry*> childVectorStack;
    childVectorStack.push(&target);
    int counter=0;
    while (!input.atEnd()) {
        char entryHeader;
        input.readRawData(&entryHeader,1);
        if(entryHeader==1)
        {
           traceEntryPtr newEntry ( new TraceEntry{} );
           newEntry->id = counter++;
           input.readRawData((char*)&newEntry->kind,4);
           qint32 length;
           input>>length;
           QByteArray buffer(length, Qt::Uninitialized);
           input.readRawData(buffer.data(),length);
           newEntry->context = QString(buffer);

           input>>newEntry->instantiation;
           input>>newEntry->instantiationBegin;
           input>>newEntry->instantiationEnd;
           input>>newEntry->declarationBegin;
           input>>newEntry->declarationEnd;

           newEntry->parent = childVectorStack.top();
           childVectorStack.top()->children.push_back(newEntry);
           childVectorStack.push(childVectorStack.top()->children.last().data());
        }
        else if(entryHeader==0)
        {
            TraceEntry &lastEntry = *childVectorStack.top();
            input.readRawData((char*)&lastEntry.memoryUsage,sizeof(size_t));
            childVectorStack.pop();
        }
        else
        {
            throw FileException();
        }
    }
}

} // namespace Templar
