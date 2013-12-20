#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include "traceentry.h"
#include <QRegExp>

namespace Templar {

class Builder
{
public:
    virtual void traceEntry(const TraceEntry&) {}

    virtual ~Builder() {}

protected:
    Builder() {}
};

} // namespace Templar

#endif // GRAPHBUILDER_H
