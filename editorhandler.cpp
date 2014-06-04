#include "editorhandler.h"
#include "traceentry.h"
#include "codeeditor.h"
#include "highlighter.h"

#include <QString>
#include <QFile>
#include "common.h"

namespace Templar {

EditorHandler::EditorHandler(CodeEditor *editor)
    : editor(editor)
{
}

void EditorHandler::handleEvent(const TraceEntry &entry)
{
    QColor color = (entry.isBegin)?
                QColor(common::colors::BEGIN) :
                QColor(common::colors::END);

    highlightPos(entry, color);

//    undoList.push_back(std::make_pair(entry.position, color));
}

void EditorHandler::undoEvent()
{
    if (undoList.empty())
        return;

    undoList.pop_back();

    if (undoList.empty())
        return;

    //highlightPos(undoList.back().first,
    //             undoList.back().second);
}

void EditorHandler::inspect(const TraceEntry& entry)
{
    highlightPos(entry, Qt::yellow);
}

void EditorHandler::reset(const TraceEntry &)
{
    undoList.clear();
}

void EditorHandler::forward(const std::vector<TraceEntry> &entryVec)
{
    for (int i = 0; i < (int)entryVec.size() - 1; ++i)
    {
        const TraceEntry& entry = entryVec[i];
        QColor color = (entry.isBegin)?
                    QColor(common::colors::BEGIN) :
                    QColor(common::colors::END);

       // undoList.push_back(std::make_pair(entry.position, color));
    }

    if (!entryVec.empty())
    {
        handleEvent(entryVec.back());
    }
}

void EditorHandler::rewind(unsigned int count)
{
    undoList.erase(undoList.end() - count + 1, undoList.end());
    undoEvent();
}
void EditorHandler::gotoFile(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QString src(file.readAll());
    editor->setPlainText(src);
    editor->setDocumentTitle(filename);
}
void EditorHandler::highlightPos(const TraceEntry& entry, const QColor& color) {

    QFile file(entry.sourcefile);

    file.open(QIODevice::ReadOnly);

    QString src(file.readAll());

    editor->setPlainText(src);
    editor->setDocumentTitle(entry.sourcefile);

    editor->highlightLine(entry.declarationBegin.line, color);
}


} // namespace Templar
