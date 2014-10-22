#include "protobuftracereader.h"
#include <fstream>
#include "make_unique.hpp"
#include <QFileInfo>

namespace Templar {

ProtobufTraceReader::BuildReturn ProtobufTraceReader::build(QString fileName) {
    std::ifstream input(fileName.toStdString());
    TemplightTraceCollection collection;
    collection.ParseFromIstream(&input);
    BuildReturn model = make_unique<UsedSourceFileModel>();
    for (int traceIndex = 0; traceIndex < collection.traces_size();
         ++traceIndex) {
        buildFromTrace(collection.traces(traceIndex), *model);
    }
    return model;
}

void ProtobufTraceReader::buildFromTrace(TemplightTrace const &trace,
                                         UsedSourceFileModel &model) {
    for (int entryIndex = 0; entryIndex < trace.entries_size(); ++entryIndex) {
        auto &entry = trace.entries(entryIndex);
        if (entry.has_begin()) {
            begin(entry.begin(), model);
        } else if (entry.has_end()) {
            end(entry.end());
        }
    }
}

void ProtobufTraceReader::begin(TemplightEntry_Begin const &begin,
                                UsedSourceFileModel &model) {
    traceEntryPtr entry(new TraceEntry());
    entry->kind = (TraceEntry::InstantiationKind)begin.kind();
    entry->context = begin.name().name().c_str();
    if (begin.location().has_file_name()) {
        QFileInfo fileInfo(begin.location().file_name().c_str());
        model.Add(fileInfo.isRelative() ? fileInfo.filePath()
                                        : fileInfo.canonicalFilePath(),
                  begin.location().file_id());
    }
    entry->instantiation =
        SourceLocation{begin.location().file_id(), begin.location().line(),
                       begin.location().column()};
    entry->instantiationBegin = entry->instantiation;
    entry->instantiationEnd = entry->instantiation;
    entry->declarationBegin = entry->instantiation;
    entry->declarationEnd = entry->instantiation;
    entry->duration = begin.time_stamp();
    entry->memoryUsage = begin.memory_usage();
    beginEntry(entry);
}

void ProtobufTraceReader::end(TemplightEntry_End const &/*end*/) {
    /*auto &entry = */endEntry();
}

} // namespace Templar
