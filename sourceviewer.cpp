#include "sourceviewer.h"
#include "ui_sourceviewer.h"
#include "highlighter.h"

SourceViewer::SourceViewer(const QString &text, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SourceViewer)
{
    m_ui->setupUi(this);

    m_ui->textEdit->setText(text);

    highlighter = new Highlighter(m_ui->textEdit->document());
}

SourceViewer::~SourceViewer()
{
    delete m_ui;
}

void SourceViewer::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
