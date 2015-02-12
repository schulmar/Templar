#include "../gui/entryfiltersettings.h"

#include "ui_entryfiltersettings.h"

EntryFilterSettings::EntryFilterSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryFilterSettings)
{
    ui->setupUi(this);
    connect(ui->maxDepthSpinBox,SIGNAL(valueChanged(int)),ui->maxDepthSlider,SLOT(setValue(int)));
    connect(ui->maxDepthSlider,SIGNAL(valueChanged(int)), ui->maxDepthSpinBox,SLOT(setValue(int)));
    connect(ui->criticalPathThresholdHorizontalSlider,SIGNAL(valueChanged(int)), ui->criticalPathThresoldSpinBox, SLOT(setValue(int)));
    connect(ui->criticalPathThresoldSpinBox,SIGNAL(valueChanged(int)), ui->criticalPathThresholdHorizontalSlider, SLOT(setValue(int)));
}

EntryFilterSettings::~EntryFilterSettings()
{
    delete ui;
}
