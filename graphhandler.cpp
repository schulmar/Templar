
#include <cstdlib>

#include "graphhandler.h"
#include "qgraph.h"
#include "traceentry.h"
#include "common.h"

namespace Templar {

namespace {

inline QColor colorOfEntry(const TraceEntry& entry) {
    return (entry.isBegin) ? QColor(common::colors::BEGIN)
                           : QColor(common::colors::END);
}

} // unnamed namespace

void GraphHandler::handleEvent(const TraceEntry &entry)
{
    NodeId nodeId = entry.id;

    changeGraph(nodeId);

    QColor oldColor = colorOfNode(nodeId);
    undoStack.push_back(std::make_pair(nodeId, oldColor));

    QColor newColor = colorOfEntry(entry);
    nodeColor[nodeId] = newColor;

    theGraph->colorNode(QString::number(nodeId), newColor);
}

void GraphHandler::undoEvent()
{
    if (undoStack.empty())
        return;

    UndoInfo undo = undoStack.back();
    undoStack.pop_back();

    NodeId nodeId = undo.first;
    QColor color = undo.second;

    changeGraph(nodeId);

    nodeColor[nodeId] = color;
    theGraph->colorNode(QString::number(nodeId), color);
}

void GraphHandler::reset()
{
    undoStack.clear();
    nodeColor.clear();
    theGraph->getScene()->clear();
}

void GraphHandler::inspect(const TraceEntry &entry)
{
    QGraphicsItem *item = theGraph->getNodeById(QString::number(entry.id));
    theGraph->centerOn(item);
}

graph_t* GraphHandler::changeGraph(const NodeId& nodeName)
{
    graph_t* graph = nodeToGraph[nodeName].data();

    if (theGraph->getGraph() != graph) {  
        theGraph->renderGraph(gvc.data(), graph, "dot");
        colorize();
    }

    return graph;
}

QColor GraphHandler::colorOfNode(const NodeId& nodeName)
{
    if (nodeColor.contains(nodeName)) {
        return nodeColor[nodeName];
    } else {
        return QColor(common::colors::DEFAULT);
    }
}

void GraphHandler::colorize()
{
    graph_t *graph = theGraph->getGraph();
    for (node_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node)) {
        NodeId ndId = std::atoi(node->name);

        theGraph->colorNode(node->name, colorOfNode(ndId));
    }
}

void GraphHandler::forward(const std::vector<TraceEntry> & entryVec)
{
    bool follow = theGraph->follow(false);

    for (unsigned int i = 0; i < entryVec.size() - 1; ++i)
    {
        const TraceEntry &entry = entryVec[i];

        NodeId nodeId = entry.id;
        undoStack.push_back(std::make_pair(nodeId, colorOfNode(nodeId)));

        nodeColor[nodeId] = colorOfEntry(entry);
    }

    theGraph->follow(follow);

    const TraceEntry &entry = entryVec.back();
    NodeId nodeId = entry.id;

    graph_t* graph = changeGraph(nodeId);
    colorize();

    handleEvent(entryVec.back());
}

void GraphHandler::rewind(unsigned int count)
{
    bool follow = theGraph->follow(false);
    while (count-- > 1)
    {
        undoEvent();
    }
    theGraph->follow(follow);
    undoEvent();
}

} // namespace Templar
