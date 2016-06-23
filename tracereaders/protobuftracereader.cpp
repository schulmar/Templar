#include "protobuftracereader.h"
#include "templight-tools/include/templight/ProtobufReader.h"

#include <fstream>
#include "make_unique.hpp"
#include <QFileInfo>

#include <vector>

namespace Templar {

ProtobufTraceReader::BuildReturn ProtobufTraceReader::build(QString fileName) {
    std::ifstream input(fileName.toStdString());
    ::templight::ProtobufReader pbfReader;
    pbfReader.startOnBuffer(input);
    BuildReturn model = make_unique<UsedSourceFileModel>();
    while ( pbfReader.LastChunk != ::templight::ProtobufReader::EndOfFile ) {
        switch ( pbfReader.LastChunk ) {
          case ::templight::ProtobufReader::EndOfFile:
            break;
          case ::templight::ProtobufReader::Header:
            pbfReader.next();
            break;
          case ::templight::ProtobufReader::BeginEntry:
            begin(pbfReader.LastBeginEntry, *model);
            pbfReader.next();
            break;
          case ::templight::ProtobufReader::EndEntry:
            end(pbfReader.LastEndEntry);
            pbfReader.next();
            break;
          case ::templight::ProtobufReader::Other:
          default:
            pbfReader.next();
            break;
        }
    }
    return model;
}

namespace {
/**
 * @brief Converts the instantiation kind stored in the file into the one used
 * 		  in this application
 *
 *  This indirection might become necessary when the enums diverge
 */
TraceEntry::InstantiationKind
instantiationKindFromFile(int kindInFile) {
    return static_cast<TraceEntry::InstantiationKind>(kindInFile);
}
}

void ProtobufTraceReader::begin(::templight::PrintableEntryBegin begin, UsedSourceFileModel &model) {
    traceEntryPtr entry(new TraceEntry());
    entry->kind = instantiationKindFromFile(begin.InstantiationKind);
    entry->context = begin.Name.c_str();
    
    size_t fileId = -1U;
    if (!begin.FileName.empty()) {
        QFileInfo fileInfo(begin.FileName.c_str());
        fileId = model.Add(fileInfo.isRelative() ? fileInfo.filePath()
                                        : fileInfo.canonicalFilePath());
    }
    entry->instantiation = SourceLocation{fileId, static_cast<unsigned>(begin.Line),
                                            static_cast<unsigned>(begin.Column)};
    entry->instantiationBegin = entry->instantiation;
    entry->instantiationEnd = entry->instantiation;
    entry->declarationBegin = entry->instantiation;
    entry->declarationEnd = entry->instantiation;
    entry->beginTimeStamp = begin.TimeStamp;
    entry->memoryUsage = begin.MemoryUsage;
    beginEntry(entry);
}

void ProtobufTraceReader::end(::templight::PrintableEntryEnd end) {
    auto &entry = endEntry();
    entry.endTimeStamp = end.TimeStamp;
}

} // namespace Templar
