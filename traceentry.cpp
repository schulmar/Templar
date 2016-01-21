#include "traceentry.h"

namespace Templar
{

const std::vector<const char *> TraceEntry::InstantiationKindNames = {
    "TemplateInstantiation",
    "DefaultTemplateArgumentInstantiation",
    "DefaultFunctionArgumentInstantiation",
    "ExplicitTemplateArgumentSubstitution",
    "DeducedTemplateArgumentSubstitution",
    "PriorTemplateArgumentSubstitution",
    "DefaultTemplateArgumentChecking",
    "ExceptionSpecInstantiation",
    "Memoization",
    "Unknown"};

TraceEntry::iterator::iterator(const TraceEntry *first)
    :currentEntry(first)
    ,root(first)
{

}

TraceEntry const *TraceEntry::iterator::moveToNextSibling(TraceEntry const *entry) const
{
    if(entry->parent!=nullptr)
    {
        QVector<traceEntryPtr>::iterator position = std::find_if(entry->parent->children.begin(),entry->parent->children.end(),
                                                              [&](traceEntryPtr &e){ return e.data() == entry;});
        position++;
        if(position == entry->parent->children.end())
        {
            if(entry == root)
                return nullptr;
            else
               return moveToNextSibling(entry->parent);
        }
        else
            return position->data();
    }
    return nullptr;
}

void TraceEntry::iterator::increment()
{
    if(currentEntry) {
        currentEntry = currentEntry->children.empty()
                           ? moveToNextSibling(currentEntry)
                           : currentEntry->children.front().data();
    }
}

Templar::TraceEntry::iterator TraceEntry::end() { return iterator(); }

Templar::TraceEntry::iterator TraceEntry::begin() { return iterator(this); }

}
