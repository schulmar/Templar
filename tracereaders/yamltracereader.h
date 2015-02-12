#ifndef YAMLTRACEREADER_H_
#define YAMLTRACEREADER_H_

#include "tracereader.h"
#include "sourcefileidaccumulator.h"

namespace Templar {

struct YAMLTraceReader : TraceReader {
    YAMLTraceReader(TraceEntry &target) : TraceReader{target} {}

    static SourceFileIdAccumulator::SourceFiles
    readSourceFiles(const QString &fileName);

    BuildReturn build(QString fileName) override;
};

} // namespace Templar

#endif /* YAMLTRACEREADER_H_ */
