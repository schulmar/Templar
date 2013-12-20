#ifndef ENTRYVECTORBUILDER_H
#define ENTRYVECTORBUILDER_H

#include "builder.h"
#include <vector>

namespace Templar {

class EntryVectorBuilder : public Builder
{
public:
    void traceEntry(const TraceEntry &entry);

    std::vector<TraceEntry> getTraceEntryVector() {
        return traceEntryVec;
    }

private:
    std::vector<TraceEntry> traceEntryVec;
};

} // namespace Templar

#endif // ENTRYVECTORBUILDER_H
