#include "protobuftracereader.h"
#include <fstream>

namespace Templar {

ProtobufTraceReader::BuildReturn ProtobufTraceReader::build(QString fileName) {
    std::ifstream input(fileName.toStdString());
}

} // namespace Templar
