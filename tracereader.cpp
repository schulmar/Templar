#include <QFile>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "tracereader.h"
#include "traceentry.h"

namespace Templar {
QDataStream &operator>>(QDataStream &input, SourceLocation &location)
{
    qint64 fileId;
    input>>fileId;
    location.fileId = static_cast<size_t>(fileId);
    input>>location.line;
    input>>location.col;
}
void TraceReader::build(QString fileName)
{
    qDebug() << "TraceReader::build fileName=" << fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        throw FileException();
    }
    QDataStream input(&file);
    input.setByteOrder(QDataStream::LittleEndian);

    std::stack<TraceEntry*> childVectorStack;
    childVectorStack.push(&target);
    int counter=0;
    while (!input.atEnd()) {
        char entryHeader;
        input.readRawData(&entryHeader,1);
        if(entryHeader==1)
        {
           traceEntryPtr newEntry ( new TraceEntry{} );
           newEntry->id = counter++;
           input.readRawData((char*)&newEntry->kind,4);
           qint32 length;
           input>>length;
           QByteArray buffer(length, Qt::Uninitialized);
           input.readRawData(buffer.data(),length);
           newEntry->context = QString(buffer);

           input>>newEntry->instantiation;
           input>>newEntry->instantiationBegin;
           input>>newEntry->instantiationEnd;
           input>>newEntry->declarationBegin;
           input>>newEntry->declarationEnd;

           newEntry->parent = childVectorStack.top();
           childVectorStack.top()->children.push_back(newEntry);
           childVectorStack.push(childVectorStack.top()->children.last().data());
        }
        else if(entryHeader==0)
        {
            TraceEntry &lastEntry = *childVectorStack.top();
            input.readRawData((char*)&lastEntry.memoryUsage,sizeof(size_t));
            childVectorStack.pop();
        }
        else
        {
            throw FileException();
        }
    }
}

} // namespace Templar
