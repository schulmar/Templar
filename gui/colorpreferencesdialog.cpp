#include "../gui/colorpreferencesdialog.h"

#include "ui_colorpreferencesdialog.h"
#include "common.h"
#include <QColorDialog>

ColorPreferencesDialog::ColorPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorPreferencesDialog)
{
    ui->setupUi(this);
    ui->colorTableWidget->setColumnCount(2);
    ui->colorTableWidget->setRowCount(10);
    for(int i=0;i<10;i++)
    {
        QTableWidgetItem *pNewItem = new QTableWidgetItem(common::colors::colorNames[i]);
        ui->colorTableWidget->setItem(i,0,pNewItem);

        QTableWidgetItem *pColorItem = new QTableWidgetItem("...");
        pColorItem->setBackgroundColor(common::colors::nodeColors[i]);
        ui->colorTableWidget->setItem(i,1,pColorItem);
    }
}

ColorPreferencesDialog::~ColorPreferencesDialog()
{
    delete ui;
}

void ColorPreferencesDialog::on_colorTableWidget_doubleClicked(const QModelIndex &index)
{
    QColor currentColor = ui->colorTableWidget->item(index.row(),1)->backgroundColor();
    currentColor = QColorDialog::getColor(currentColor,this);
    ui->colorTableWidget->item(index.row(),1)->setBackgroundColor(currentColor);

}

void ColorPreferencesDialog::on_buttonBox_accepted()
{
    for(int i=0;i<10;i++)
    {
        common::colors::nodeColors[i]=ui->colorTableWidget->item(i,1)->backgroundColor();
    }
}
