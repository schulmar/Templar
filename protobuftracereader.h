#ifndef PROTOBUFTRACEREADER_H_
#define PROTOBUFTRACEREADER_H_

#include "tracereader.h"

namespace Templar {

struct ProtobufTraceReader : TraceReader {
    ProtobufTraceReader(TraceEntry &target) : TraceReader{target} {}

    BuildReturn build(QString fileName) override;
};

} // namespace Templar

#endif /* PROTOBUFTRACEREADER_H_ */
