#include "entryinfo.h"
#include "ui_entryinfo.h"

EntryInfo::EntryInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryInfo)
{
    ui->setupUi(this);
}

void EntryInfo::setEventType(const QString &event)
{
    ui->eventTypeEdit->setText(event);
}

void EntryInfo::setKind(const QString &kind)
{
    ui->kindEdit->setText(kind);
}

void EntryInfo::setName(const QString &name)
{
    ui->nameEdit->setText(name);
}

void EntryInfo::setFilePosition(const QString &filePos)
{
    ui->filePositionEdit->setText(filePos);
}

EntryInfo::~EntryInfo()
{
    delete ui;
}
