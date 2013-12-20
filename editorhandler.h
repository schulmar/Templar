#ifndef EDITORHANDLER_H
#define EDITORHANDLER_H

#include "templateeventhandler.h"
#include <vector>
#include <utility>

class CodeEditor;
class QString;
class QColor;

namespace Templar {

class EditorHandler : public TemplateEventHandler
{
public:
    EditorHandler(CodeEditor *editor);

    void handleEvent(const TraceEntry& entry);
    void undoEvent();
    void reset();

    void inspect(const TraceEntry& entry);
    void forward(const std::vector<TraceEntry> &entryVec);
    void rewind(unsigned int count);

private:
    void highlightPos(const QString& fileLoc, const QColor& color);

private:
    CodeEditor *editor;

    typedef std::pair<QString, QColor> UndoInfo;
    std::vector<UndoInfo> undoList;
};

} // namespace Templar

#endif // EDITORHANDLER_H
