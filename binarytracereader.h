#ifndef BINARYTRACEREADER_H_
#define BINARYTRACEREADER_H_

#include "tracereader.h"

namespace Templar {

struct BinaryTraceReader : TraceReader {
    BinaryTraceReader(TraceEntry &target) : TraceReader(target) {}

    BuildReturn build(QString fileName) override;
private:
    BuildReturn readSourceFileModelFromFile(QString fileName);
};

} // namespace Templar

#endif /* BINARYTRACEREADER_H_ */
