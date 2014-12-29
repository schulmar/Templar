#ifndef TRACEENTRY_H
#define TRACEENTRY_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QMap>
#include <QMetaType>

namespace Templar{

struct TraceEntry;
struct SourceFileNode;
typedef QMap<size_t,SourceFileNode*> UsedFileMap;

struct AbstractEntryWalker
{
    virtual ~AbstractEntryWalker() {}
    virtual void *visit(const TraceEntry &parent, const TraceEntry &currentNode);
};


struct SourceLocation
{
   size_t fileId;
   unsigned int    line;
   unsigned int    col;
};

typedef QSharedPointer<TraceEntry> traceEntryPtr;

struct TraceEntry {

/// \brief The kind of template instantiation we are performing
    enum InstantiationKind {
      /// We are instantiating a template declaration. The entity is
      /// the declaration we're instantiating (e.g., a CXXRecordDecl).
      TemplateInstantiation,

      /// We are instantiating a default argument for a template
      /// parameter. The Entity is the template, and
      /// TemplateArgs/NumTemplateArguments provides the template
      /// arguments as specified.
      /// FIXME: Use a TemplateArgumentList
      DefaultTemplateArgumentInstantiation,

      /// We are instantiating a default argument for a function.
      /// The Entity is the ParmVarDecl, and TemplateArgs/NumTemplateArgs
      /// provides the template arguments as specified.
      DefaultFunctionArgumentInstantiation,

      /// We are substituting explicit template arguments provided for
      /// a function template. The entity is a FunctionTemplateDecl.
      ExplicitTemplateArgumentSubstitution,

      /// We are substituting template argument determined as part of
      /// template argument deduction for either a class template
      /// partial specialization or a function template. The
      /// Entity is either a ClassTemplatePartialSpecializationDecl or
      /// a FunctionTemplateDecl.
      DeducedTemplateArgumentSubstitution,

      /// We are substituting prior template arguments into a new
      /// template parameter. The template parameter itself is either a
      /// NonTypeTemplateParmDecl or a TemplateTemplateParmDecl.
      PriorTemplateArgumentSubstitution,

      /// We are checking the validity of a default template argument that
      /// has been used when naming a template-id.
      DefaultTemplateArgumentChecking,

      /// We are instantiating the exception specification for a function
      /// template which was deferred until it was needed.
      ExceptionSpecInstantiation,

      // BEGIN TEMPLIGHT
      /// Added for Template debugging (TEMPLIGHT)
      /// We are _not_ instantiating a template because it is already
      /// instantiated.
      Memoization,
      // END TEMPLIGHT
      Unknown

    } kind;

    static const std::vector<const char *> InstantiationKindNames;

    TraceEntry()
    : kind(Unknown), instantiation(), instantiationBegin(), instantiationEnd(),
      declarationBegin(), declarationEnd(), sourceFileId(0), id(-1), 
      memoryUsage(0), duration(0), parent(nullptr) {}

    QString context;
    SourceLocation instantiation;
    SourceLocation instantiationBegin;
    SourceLocation instantiationEnd;
    SourceLocation declarationBegin;
    SourceLocation declarationEnd;
    size_t sourceFileId;
    unsigned int id;
    int64_t memoryUsage;
    double duration;
    TraceEntry *parent;
    QVector<traceEntryPtr> children;

    struct iterator
    {
        iterator(TraceEntry const *first=nullptr);
        TraceEntry const *operator->() { return currentEntry; }

        TraceEntry const *moveToNextSibling(TraceEntry const*entry);
        iterator& operator++();

        bool operator==(const iterator &itr) const
        { return itr.currentEntry == currentEntry; }
        bool operator!=(const iterator &itr) const
        { return !(*this==itr); }

        TraceEntry const *currentEntry;
        TraceEntry const *root;

    };
    static iterator end() { return iterator(); }
    iterator begin() { return iterator(this); }
};

template <typename EntryWalker>
struct FullTreeWalker
{
    typedef typename EntryWalker::node_data node_data;
    node_data Apply(node_data parent_data, const TraceEntry &root, EntryWalker &walker)
    {
        for (int i = 0; i < static_cast<int>(root.children.size()); ++i)
        {
            Apply(walker.visit(parent_data,root,*root.children.at(i)),*root.children.at(i),walker);
        }
        // todo: why not void? the return value is never used
        return node_data();
    }
};
/*
template <typename EntryWalker>
struct FilteringTreeWalker
{
    FilteringTreeWalker()
    {}
    typedef typename EntryWalker::node_data node_data;
    node_data *Apply(node_data *, const TraceEntry &root, EntryWalker &walker)
    {
        for (unsigned int i = 0; i < root.children.size(); ++i)
        {
            Apply(walker.visit(root,root.children.at(i)),root.children.at(i),walker);
        }
    }
};*/
} // namespace Templar

Q_DECLARE_METATYPE(Templar::TraceEntry) // for storing in QVariant

#endif // TRACEENTRY_H
