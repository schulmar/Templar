#ifndef GRAPHHANDLER_H
#define GRAPHHANDLER_H

#include <utility>
#include <vector>

#include <QMap>
#include <QString>
#include <QColor>
#include <QSharedPointer>
#include <QHash>
#include <QColor>

#include <graphviz/gvc.h>

#include "templateeventhandler.h"
#include "graphvizbuilder.h"

class QGraph;

namespace Templar {

class GraphHandler : public TemplateEventHandler
{
public:
    typedef GraphvizBuilder::NodeId NodeId;
    typedef GraphvizBuilder::NodeIdToGraph NodeIdToGraph;

    GraphHandler(QGraph *qGraph);
    virtual ~GraphHandler() {}

    void selectRoot(const TraceEntry &entry);

    void handleEvent(const TraceEntry &entry);

    void undoEvent();

    void reset(const TraceEntry &entry);

    void inspect(const TraceEntry &entry);

    void forward(const std::vector<TraceEntry> & entryVec);
    void rewind(unsigned int count);

    void setGvc(QSharedPointer<GVC_t> gvc) {
        this->gvc = gvc;
    }

    void setNodeGraphMap(const NodeIdToGraph& nodeToGr) {
        nodeToGraph = nodeToGr;
    }


    void setCriticalPathThreshold(double value);
    void enableCriticalPathFilter(bool enable);
    void setMaxDepthValue(int value);

private:
    QColor colorOfNode(const NodeId& nodeName);
    void changeGraph(const TraceEntry &entry);
    void colorize();

private:
    QGraph *theGraph;
    QSharedPointer<GVC_t> gvc;
    QSharedPointer<graph_t> currentGraph;

    TraceEntry::iterator currentRootIterator;
    NodeIdToGraph nodeToGraph;
    QHash<NodeId, QColor> nodeColor;

    typedef std::pair<NodeId, QColor> UndoInfo;
    std::vector<UndoInfo> undoStack;
};

} // namespace Templar

#endif // GRAPHHANDLER_H
