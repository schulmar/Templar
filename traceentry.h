#ifndef TRACEENTRY_H
#define TRACEENTRY_H

#include <QString>
#include <QMetaType>

namespace Templar{

struct TraceEntry{
    bool isBegin;
    QString context;
    QString position;
    QString kind;
    double time;
    unsigned int id;
};

} // namespace Templar

Q_DECLARE_METATYPE(Templar::TraceEntry) // for storing in QVariant

#endif // TRACEENTRY_H
