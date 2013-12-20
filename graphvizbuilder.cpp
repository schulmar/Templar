#include "graphvizbuilder.h"
#include "common.h"
#include <QString>
#include <sstream>

namespace Templar{

namespace {

QString breakString(const QString& str)
{
    const int lineLength = 128;
    int i = 0;
    QString ret = str;

    while ( (i += lineLength) < str.length() )
    {
        ret.insert(i, '\n');
    }

    return ret;
}

template <typename To, typename From>
To lexical_cast(From from)
{
    std::stringstream ss;

    ss << from;
    To to;
    ss >> to;

    return to;
}

} // Unnamed namespace

inline bool isTemplateInstantiation(const TraceEntry & entry)
{
    return entry.kind == "TemplateInstantiation";
}

GraphvizBuilder::GraphvizBuilder()
    : gvc(gvContext(), gvFreeContext), counter(0)
{
}

void GraphvizBuilder::initGraph()
{
    graph_t *graphPtr = agopen("Templight", AGDIGRAPHSTRICT);
    agnodeattr(graphPtr, "shape", common::attrs::POLYSHAPE.toAscii().data());
    agnodeattr(graphPtr, "sides", common::attrs::SIDES.toAscii().data());
    agnodeattr(graphPtr, "fillcolor", common::colors::DEFAULT.toAscii().data());
    agnodeattr(graphPtr, "style", common::attrs::STYLE.toAscii().data());
    agnodeattr(graphPtr, "instantiationpos", "");

    actualGraph = QSharedPointer<graph_t> (graphPtr, agclose);
}

void GraphvizBuilder::traceEntry(const TraceEntry &entry)
{
    if (entry.isBegin)
        templateBegin(entry);
    else
        templateEnd();
}

void GraphvizBuilder::templateBegin(const TraceEntry& entry)
{
    if (nodeStack.empty()) {
        initGraph();
    }

    NodeId nodeId = entry.id;
    std::string idStr = lexical_cast<std::string>(nodeId);

    node_t* node = agnode(actualGraph.data(),
                          const_cast<char*>(idStr.c_str()) );

    char* label = breakString(entry.context).toAscii().data();
    agset(node, "label", label);

    if (entry.kind == "Memoization") {
        agset(node, "shape", common::attrs::ELLIPSESHAPE.toAscii().data() );
    }

    // set parent
    if (!nodeStack.empty()){
        agedge(actualGraph.data(), nodeStack.top(), node);
    }

    nodeToGraph[nodeId] = actualGraph;

    nodeStack.push(node);
}

void GraphvizBuilder::templateEnd()
{
    if (nodeStack.empty())
        return;

    nodeStack.pop();
}


} // Templar

