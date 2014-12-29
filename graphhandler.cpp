
#include <cstdlib>

#include "graphhandler.h"
#include "qgraph.h"
#include "traceentry.h"
#include "common.h"

namespace Templar {

namespace {

inline QColor colorOfEntry(const TraceEntry& /*entry*/) {
    return QColor(common::colors::BEGIN);
}

} // unnamed namespace

GraphHandler::GraphHandler(QGraph *qGraph)
    : theGraph(qGraph), gvc(gvContext(), gvFreeContext)
{}

void GraphHandler::selectRoot(const TraceEntry &entry)
{
    changeGraph(entry);
    nodeColor.clear();
}
void GraphHandler::handleEvent(const TraceEntry &entry)
{
    theGraph->colorizeUpToNode(entry.id);
}

void GraphHandler::undoEvent()
{
   /* if (undoStack.empty())
        return;

    UndoInfo undo = undoStack.back();
    undoStack.pop_back();

    NodeId nodeId = undo.first;
    QColor color = undo.second;

  //  changeGraph(nodeId);

    nodeColor[nodeId] = color;
    theGraph->colorNode(QString::number(nodeId), color);*/
}

void GraphHandler::reset(const TraceEntry &/*entry*/)
{
    undoStack.clear();
    nodeColor.clear();
    theGraph->clearGraph();
}

void GraphHandler::inspect(const TraceEntry &entry)
{
    QGraphicsItem *item = theGraph->getNodeById(entry.id);
    theGraph->centerOn(item);
}

void GraphHandler::changeGraph(const TraceEntry &entry)
{
    currentRootIterator = TraceEntry::iterator(&entry);
    GraphvizVisitor newVisitor(gvc);
    FullTreeWalker<GraphvizVisitor> graphWalker;
    TraceEntry dummy;
    graphWalker.Apply(newVisitor.visit(nullptr,dummy,entry),entry,newVisitor);
    currentGraph = newVisitor.actualGraph;
//    graph_t* graph = nodeToGraph[nodeName].data();

//    if (theGraph->getGraph() != graph) {
        theGraph->renderGraph(gvc.data(), currentGraph.data(), "dot");
        colorize();
  //  }

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
/*    graph_t *graph = theGraph->getGraph();
    for (node_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node)) {
        NodeId ndId = std::atoi(agnameof(node));

        theGraph->colorNode(agnameof(node), colorOfNode(ndId));
    }*/
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

    //const TraceEntry &entry = entryVec.back();
    //NodeId nodeId = entry.id;

    //removed unused: graph_t* graph = ;
    //changeGraph(nodeId);
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
