#include "graphvizbuilder.h"
#include "common.h"
#include "qt_version_switch.h"
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
    Agraph_t *graphPtr = agopen("Templight", Agstrictdirected, &AgDefaultDisc);
    agattr(graphPtr, AGNODE, "shape", Q_TO_C_STRING(common::attrs::POLYSHAPE));
    agattr(graphPtr, AGNODE, "shape", Q_TO_C_STRING(common::attrs::POLYSHAPE));
    agattr(graphPtr, AGNODE, "sides", Q_TO_C_STRING(common::attrs::SIDES));
    agattr(graphPtr, AGNODE, "fillcolor", Q_TO_C_STRING(common::colors::DEFAULT));
    agattr(graphPtr, AGNODE, "style", Q_TO_C_STRING(common::attrs::STYLE));
    agattr(graphPtr, AGNODE, "instantiationpos", "");
#pragma GCC diagnostic pop
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
                          const_cast<char*>(idStr.c_str()),
                          TRUE);

    char* label = Q_TO_C_STRING(breakString(entry.context));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
    agset(node, "label", label);

    if (entry.kind == "Memoization") {
        agset(node, "shape", Q_TO_C_STRING(common::attrs::ELLIPSESHAPE));
    }

    // set parent
    if (!nodeStack.empty()){
        agedge(actualGraph.data(), nodeStack.top(), node, "", TRUE);
    }
#pragma GCC diagnostic pop

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

