#include "oldxmltracereader.h"
#include <cassert>
#include "make_unique.hpp"

namespace Templar {

namespace {
const char* TemplateBegin = "TemplateBegin";
const char* TemplateEnd = "TemplateEnd";
const char* Position = "PointOfInstantiation";
const char* Context = "Context";
const char* Kind = "Kind";
const char* TimeStamp = "TimeStamp";
const char* Memory = "MemoryUsage";

#if USE_QT5
QStringRef attributeString(QXmlStreamReader &xml, const char *attributeName) {
	return xml.attributes().value(attributeName);
}
#else
QString attributeString(QXmlStreamReader &xml, const char *attributeName) {
	return xml.attributes().value(attributeName).toString();
}
#endif

double attributeAsDouble(QXmlStreamReader &xml, const char *attributeName) {
	return attributeString(xml, attributeName).toDouble();
}

qlonglong attributeAsLongLong(QXmlStreamReader &xml, const char *attributeName) {
	return attributeString(xml, attributeName).toLongLong();
}
}

SourceFileLocation OldXMLTraceReader::locationFromXML(QXmlStreamReader &xml) {
  assert(xml.name().toString() == Position);
  auto components = xml.readElementText().split('|');
  return SourceFileLocation{components[0], components[1].toUInt(),
                            components[2].toUInt()};
}

SourceFileIdAccumulator::SourceFiles
OldXMLTraceReader::readSourceFiles(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException();
    }
    QXmlStreamReader xml(&file);
    SourceFileIdAccumulator sourceFilesAccumulator(
        QFileInfo(fileName).dir().path());

    while (!xml.atEnd()) {
        xml.readNext();
        QString entryType = xml.name().toString();
        if ((entryType == TemplateBegin) &&
            xml.isStartElement()) {
            do {
                xml.readNext();
                if (xml.name().toString() == Position && xml.isStartElement()) {
                    sourceFilesAccumulator.addLocation(locationFromXML(xml));
                }
            } while (xml.name().toString() != entryType);
        }
    }
    return sourceFilesAccumulator.sourceFiles;
}

OldXMLTraceReader::BuildReturn OldXMLTraceReader::build(QString fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException();
    }
    QXmlStreamReader xml(&file);

    SourceFileIdAccumulator sourceFilesAccumulator(getDirPath());

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
                            sourceFilesAccumulator.addLocation(
                                locationFromXML(xml));
                        newEntry->instantiationEnd =
                            newEntry->instantiationBegin =
                                newEntry->instantiation;
                        newEntry->sourceFileId = newEntry->instantiation.fileId;
                        /* todo:
                        newEntry->declarationBegin;
                        newEntry->declarationEnd;
                        */
                    } else if (xml.name().toString() == Kind &&
                               xml.isStartElement()) {
                        newEntry->kind =
                            entryKindFromString(xml.readElementText());
                    } else if (xml.name().toString() == TimeStamp &&
                               xml.isStartElement()) {
                        // store start for later difference calculation
                        newEntry->duration = attributeAsDouble(xml, "time");
                    }
                } while (xml.name().toString() != entryType);

                beginEntry(newEntry);
            } else if (entryType == TemplateEnd) {
                TraceEntry &lastEntry = endEntry();
                do {
                    xml.readNext();
                    if (xml.name().toString() == Memory &&
                        xml.isStartElement()) {
                        lastEntry.memoryUsage =
                            attributeAsLongLong(xml, "bytes");
                    } else if (xml.name().toString() == TimeStamp &&
                               xml.isStartElement()) {
                        double endTimeStamp = attributeAsDouble(xml, "time");
                        lastEntry.duration = endTimeStamp - lastEntry.duration;
                    }
                } while (xml.name().toString() != entryType);
            }
        }
    }
    return make_unique<UsedSourceFileModel>(readSourceFiles(fileName));
}

}
