#include "protobuftracereader.h"
#include "templight_messages.pb.h"

#include <fstream>
#include "make_unique.hpp"
#include <QFileInfo>

#include <vector>

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
            begin(entry.begin(), trace, model);
        } else if (entry.has_end()) {
            end(entry.end());
        }
    }
}

namespace {
/**
 * @brief Converts the instantiation kind stored in the file into the one used
 * 		  in this application
 *
 *  This indirection might become necessary when the enums diverge
 */
TraceEntry::InstantiationKind
instantiationKindFromFile(::TemplightEntry_InstantiationKind kindInFile) {
    return static_cast<TraceEntry::InstantiationKind>(int(kindInFile));
}

struct dict_expansion_task {
  DictionaryEntry const * p_entry;
  std::size_t char_id;
  std::size_t mark_id;
};
}

void ProtobufTraceReader::begin(TemplightEntry_Begin const &begin,
                                TemplightTrace const &trace,
                                UsedSourceFileModel &model) {
    traceEntryPtr entry(new TraceEntry());
    entry->kind = instantiationKindFromFile(begin.kind());
    
    if (begin.name().has_name()) {
        entry->context = begin.name().name().c_str();
    } else if (begin.name().has_dict_id()) {
        // TODO Build a template-name dictionary that does the string expansion on-demand (and maybe produce reduced expansion levels)
        std::string expanded_name;
        std::vector< dict_expansion_task > tasks;
        tasks.push_back(dict_expansion_task{&trace.names(begin.name().dict_id()), 0, 0});
        while( !tasks.empty() ) {
            const std::string& dict_name = tasks.back().p_entry->marked_name();
            std::size_t new_char_id = 
              std::find(dict_name.begin() + tasks.back().char_id, 
                        dict_name.end(), '\0') - dict_name.begin();
            expanded_name.append(dict_name.begin() + tasks.back().char_id, 
                                 dict_name.begin() + new_char_id);
            if( new_char_id < dict_name.size() ) {
                tasks.back().char_id = new_char_id + 1;
                dict_expansion_task next_task{&trace.names(tasks.back().p_entry->marker_ids(tasks.back().mark_id)), 0, 0};
                tasks.back().mark_id += 1;
                tasks.push_back(next_task);
            } else {
                tasks.pop_back();
            }
        }
        entry->context = expanded_name.c_str();
    } else {
      // TODO Should report this as an error (unsupported format).
    }
    
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
    entry->beginTimeStamp = durationFrom(begin.time_stamp());
    entry->memoryUsage = begin.memory_usage();
    beginEntry(entry);
}

void ProtobufTraceReader::end(TemplightEntry_End const &end) {
    auto &entry = endEntry();
    entry.endTimeStamp = durationFrom(end.time_stamp());
}

} // namespace Templar
