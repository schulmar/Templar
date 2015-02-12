#ifndef PROTOBUFTRACEREADER_H_
#define PROTOBUFTRACEREADER_H_

#include "tracereader.h"

// forward-declare the protoc-generated classes:
class TemplightTrace;
class TemplightEntry_Begin;
class TemplightEntry_End;

namespace Templar {

struct ProtobufTraceReader : TraceReader {
    ProtobufTraceReader(TraceEntry &target) : TraceReader{target} {}

    BuildReturn build(QString fileName) override;

  private:
    void buildFromTrace(TemplightTrace const &trace,
                        UsedSourceFileModel &model);
    void begin(TemplightEntry_Begin const &, TemplightTrace const &, UsedSourceFileModel &);
    void end(TemplightEntry_End const &);
};

} // namespace Templar

#endif /* PROTOBUFTRACEREADER_H_ */
