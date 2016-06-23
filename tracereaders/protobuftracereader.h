#ifndef PROTOBUFTRACEREADER_H_
#define PROTOBUFTRACEREADER_H_

#include "tracereader.h"
#include "templight-tools/include/templight/ProtobufReader.h"

namespace Templar {

struct ProtobufTraceReader : TraceReader {
    ProtobufTraceReader(TraceEntry &target) : TraceReader{target} {}

    BuildReturn build(QString fileName) override;

  private:
    void begin(::templight::PrintableEntryBegin, UsedSourceFileModel&);
    void end(::templight::PrintableEntryEnd);
};

} // namespace Templar

#endif /* PROTOBUFTRACEREADER_H_ */
