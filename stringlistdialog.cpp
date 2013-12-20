#include "stringlistdialog.h"
#include "ui_stringlistdialog.h"
#include <QStringList>
#include <QInputDialog>
#include <QLineEdit>

StringListDialog::StringListDialog(QString elemType, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StringListDialog),
    elemType(elemType)
{
    ui->setupUi(this);

    QObject::connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addItem()));
    QObject::connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
    QObject::connect(ui->doneButton, SIGNAL(clicked()), this, SLOT(done()));
    QObject::connect(ui->deleteAllButton, SIGNAL(clicked()), this, SLOT(deleteAllItem()));
}

void StringListDialog::addItem()
{
    QString text = QInputDialog::getText(this, tr("Add Item"),
        "Enter " + elemType + ": ",
        QLineEdit::Normal, defaultValue);

    ui->listWidget->addItem(new QListWidgetItem(text));
}

void StringListDialog::deleteItem()
{
    QSharedPointer<QListWidgetItem> item(ui->listWidget->currentItem());
    ui->listWidget->removeItemWidget(item.data());
}

void StringListDialog::deleteAllItem()
{
    ui->listWidget->clear();
}

void StringListDialog::done()
{
    this->close();
}

QStringList StringListDialog::getStringList() const
{
    QStringList ret;
    for (int i = 0; i < ui->listWidget->count(); ++i)
        ret << ui->listWidget->item(i)->text();

    return ret;
}

StringListDialog::~StringListDialog()
{
    delete ui;
}
