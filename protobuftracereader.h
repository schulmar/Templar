#ifndef PROTOBUFTRACEREADER_H_
#define PROTOBUFTRACEREADER_H_

#include "tracereader.h"
#include "templight_messages.pb.h"

namespace Templar {

struct ProtobufTraceReader : TraceReader {
    ProtobufTraceReader(TraceEntry &target) : TraceReader{target} {}

    BuildReturn build(QString fileName) override;

  private:
    void buildFromTrace(TemplightTrace const &trace,
                        UsedSourceFileModel &model);
    void begin(TemplightEntry_Begin const &, UsedSourceFileModel &model);
    void end(TemplightEntry_End const &);
};

} // namespace Templar

#endif /* PROTOBUFTRACEREADER_H_ */
