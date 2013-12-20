#include "entryvectorbuilder.h"

namespace Templar {

void EntryVectorBuilder::traceEntry(const TraceEntry &entry)
{
    traceEntryVec.push_back(entry);
}

}
