#ifndef GRAPHVIZVISITOR_H_
#define GRAPHVIZVISITOR_H_


#include <graphviz/gvc.h>
#include <QSharedPointer>
#include "traceentry.h"

namespace Templar {

struct GraphvizVisitor
{
    typedef node_t* node_data;

    GraphvizVisitor(QSharedPointer<GVC_t> &gvc);

    QSharedPointer<graph_t> actualGraph;
    node_t * visit(node_t *, const TraceEntry & parent, const TraceEntry &current);
    QSharedPointer<GVC_t> gvc;
};
} // namespace Templar

#endif /* GRAPHVIZVISITOR_H_ */
