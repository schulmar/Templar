#include "tracereader.h"

#include "make_unique.hpp"
#include <QDebug>
#include "oldxmltracereader.h"
#include "yamltracereader.h"
#include "binarytracereader.h"
#include "protobuftracereader.h"

namespace Templar {

TraceReader::BuildReturn TraceReader::build(QString fileName, TraceEntry &entry,
                                            QString dirPath) {
    qDebug() << "TraceReader::build(" << fileName << ")";

    std::unique_ptr<TraceReader> reader;
    if(fileName.endsWith("pbf")) {
      reader = make_unique<ProtobufTraceReader>(entry);
    } else if (fileName.endsWith("xml")) {
      reader = make_unique<OldXMLTraceReader>(entry);
#if YAML_TRACEFILE_SUPPORT
    } else if (fileName.endsWith("yaml")) {
        reader = make_unique<YAMLTraceReader>(entry);
#endif
    } else {
        reader = make_unique<BinaryTraceReader>(entry);
    }
    reader->setDirPath(dirPath);
    return reader->build(fileName);
}

namespace {

/**
 * @brief Remove an extension from a path string
 * @param extensionsToRemove list of extensions (without dots!)
 *
 * The dots are removed as well!
 */
QString removeExtension(QString path, QStringList extensionsToRemove) {
    for (const auto &extension : extensionsToRemove) {
        if (path.endsWith(extension)) {
            // -1 to remove the dot as well
            return path.left(path.lastIndexOf(extension) - 1);
        }
    }
    return path;
}
}

QString sourceFileNameFromTraceFileName(QString traceFileName) {
    QRegExp regex(R"raw((\.memory)?\.trace\.(xml|yaml|pbf))raw");
    return removeExtension(
        removeExtension(removeExtension(traceFileName, {"xml", "yaml", "pbf"}),
                        {"trace"}),
        {"memory"});
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

void TraceReader::beginEntry(traceEntryPtr entry) {
	entry->id = entryCounter;
    ++entryCounter;
    entry->parent = childVectorStack.top();
    childVectorStack.top()->children.push_back(entry);
    childVectorStack.push(childVectorStack.top()->children.last().data());
}

TraceEntry & TraceReader::endEntry() {
  TraceEntry &lastEntry = *childVectorStack.top();
  childVectorStack.pop();
  return lastEntry;
}

} // namespace Templar
