#include "entryfiltersettings.h"
#include "ui_entryfiltersettings.h"

EntryFilterSettings::EntryFilterSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryFilterSettings)
{
    ui->setupUi(this);
    connect(ui->maxDepthSpinBox,SIGNAL(valueChanged(int)),ui->maxDepthSlider,SLOT(setValue(int)));
    connect(ui->maxDepthSlider,SIGNAL(valueChanged(int)), ui->maxDepthSpinBox,SLOT(setValue(int)));
}

EntryFilterSettings::~EntryFilterSettings()
{
    delete ui;
}

void EntryFilterSettings::on_maxDepthSpinBox_valueChanged(int arg1)
{

}

void EntryFilterSettings::on_maxDepthSlider_valueChanged(int value)
{

}

void EntryFilterSettings::on_enableCriticalPathFilterCheckBox_stateChanged(int arg1)
{
    ui->criticalPathThresholdHorizontalSlider->setEnabled(arg1>0);
    ui->criticalPathThresoldDoubleSpinBox->setEnabled(arg1>0);
}
