#ifndef GRAPHVIZBUILDER_H
#define GRAPHVIZBUILDER_H

#include <iostream>
#include <stack>

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include <graphviz/gvc.h>

#include "builder.h"

namespace Templar {

class GraphvizBuilder : public Builder
{
public:
    typedef int NodeId;
    typedef QMap<NodeId, QSharedPointer<graph_t> > NodeIdToGraph;

    GraphvizBuilder();

    virtual void traceEntry(const TraceEntry& entry);

    NodeIdToGraph getNodeGraphMap() { return nodeToGraph; }
    QSharedPointer<GVC_t> getGvc() { return gvc; }

private:
    void initGraph();

    void templateBegin(const TraceEntry& entry);
    void templateEnd();

private:
    QSharedPointer<GVC_t> gvc;

    NodeIdToGraph nodeToGraph;

    QSharedPointer<graph_t> actualGraph;
    //TODO unused variable: unsigned int counter;

    std::stack<node_t*> nodeStack;
};

} // namespace Templar

#endif // GRAPHVIZBUILDER_H
