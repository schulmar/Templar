#ifndef SOURCEVIEWER_H
#define SOURCEVIEWER_H

#include <QDialog>

namespace Ui {
    class SourceViewer;
}

class Highlighter;

class SourceViewer : public QDialog {
    Q_OBJECT
public:
    SourceViewer(const QString &text, QWidget *parent = 0);
    ~SourceViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SourceViewer *m_ui;
    Highlighter *highlighter;
};

#endif // SOURCEVIEWER_H
