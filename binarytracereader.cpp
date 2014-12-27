#include "binarytracereader.h"
#include <QFile>
#include "make_unique.hpp"

namespace Templar {

QDataStream &operator>>(QDataStream &input, SourceLocation &location)
{
    qint64 fileId;
    input>>fileId;
    location.fileId = static_cast<size_t>(fileId);
    input>>location.line;
    input>>location.col;
    return input;
}

BinaryTraceReader::BuildReturn BinaryTraceReader::build(QString fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        throw FileException();
    }
    QDataStream input(&file);
    input.setByteOrder(QDataStream::LittleEndian);

    int counter=0;
    while (!input.atEnd()) {
        char entryHeader;
        input.readRawData(&entryHeader,1);
        if(entryHeader==1)
        {
           traceEntryPtr newEntry ( new TraceEntry{} );
           newEntry->id = counter++;
           input.readRawData(reinterpret_cast<char*>(&newEntry->kind),4);
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

           beginEntry(newEntry);
        }
        else if(entryHeader==0)
        {
          auto &entry = endEntry();
          input.readRawData(reinterpret_cast<char*>(&entry.memoryUsage),sizeof(size_t));
        }
        else
        {
            throw FileException();
        }
    }

    const char *fileListExtension = ".filelist.trace";
    QString fileListFilename = sourceFileNameFromTraceFileName(fileName) + fileListExtension;
    return readSourceFileModelFromFile(fileListFilename);
}

BinaryTraceReader::BuildReturn
BinaryTraceReader::readSourceFileModelFromFile(QString fileName) {
    auto model = make_unique<UsedSourceFileModel>();
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QDataStream input(&file);
    input.setByteOrder(QDataStream::LittleEndian);
    while (!input.atEnd()) {
        quint64 fileId;
        input >> fileId;
        qint32 length;
        input >> length;
        QByteArray buffer(length, Qt::Uninitialized);
        input.readRawData(buffer.data(), length);
        QString path(buffer);
        model->Add(path, fileId);
    }
    return model;
}

}  // namespace Templar
