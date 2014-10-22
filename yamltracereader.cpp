#include "yamltracereader.h"
#include <yaml-cpp/yaml.h>
#include <QDebug>
#include "make_unique.hpp"

namespace Templar {

SourceFileIdAccumulator::SourceFiles
YAMLTraceReader::readSourceFiles(const QString &fileName) {
    auto document = YAML::LoadFile(fileName.toStdString());
    SourceFileIdAccumulator sourceFilesAccumulator(
        QFileInfo(fileName).dir().path());

    for (auto const &node : document) {
        if (node.Type() != YAML::NodeType::Map)
            throw std::invalid_argument("Unexpected format of node");
        if (node["IsBegin"].as<bool>()) {
            sourceFilesAccumulator.addLocation(
                {node["FileName"].as<std::string>().c_str(),
                 node["Line"].as<int>(), node["Column"].as<int>()});
        }
    }
    return sourceFilesAccumulator.sourceFiles;
}

YAMLTraceReader::BuildReturn YAMLTraceReader::build(QString fileName) {
    auto document = YAML::LoadFile(fileName.toStdString());
    SourceFileIdAccumulator sourceFilesAccumulator(getDirPath());

    int counter = 0;

    try {
        for (auto const &node : document) {
            if (node.Type() != YAML::NodeType::Map)
                throw std::invalid_argument("Unexpected format of node");
            if (node["IsBegin"].as<bool>()) {
                traceEntryPtr newEntry(new TraceEntry{});
                newEntry->id = counter++;
                newEntry->context = node["Name"].as<std::string>().c_str();
                SourceFileLocation location{
                    node["FileName"].as<std::string>().c_str(),
                    node["Line"].as<int>(), node["Column"].as<int>()};
                newEntry->instantiation =
                    sourceFilesAccumulator.addLocation(location);
                newEntry->instantiationEnd = newEntry->instantiationBegin =
                    newEntry->instantiation;
                newEntry->sourceFileId = newEntry->instantiation.fileId;
                // store the start for later difference calculation
                newEntry->duration = node["TimeStamp"].as<double>();
                /* todo:
                newEntry->declarationBegin;
                newEntry->declarationEnd;
                */
                newEntry->kind =
                    entryKindFromString(node["Kind"].as<std::string>().c_str());
                beginEntry(newEntry);
            } else {
                auto &lastEntry = endEntry();
                // calculate the difference to start
                lastEntry.duration = node["TimeStamp"].as<double>() - lastEntry.duration;
                lastEntry.memoryUsage = node["MemoryUsage"].as<long long>();
            }
        }
    } catch (...) {
        qDebug() << "Error during YAML parsing";
    }
    return make_unique<UsedSourceFileModel>(readSourceFiles(fileName));
}

}  // namespace Templar

