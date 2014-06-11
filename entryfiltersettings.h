#ifndef ENTRYFILTERSETTINGS_H
#define ENTRYFILTERSETTINGS_H

#include <QWidget>

namespace Ui {
class EntryFilterSettings;
}

class EntryFilterSettings : public QWidget
{
    Q_OBJECT

public:
    explicit EntryFilterSettings(QWidget *parent = 0);
    ~EntryFilterSettings();

private slots:
    void on_maxDepthSpinBox_valueChanged(int arg1);

    void on_maxDepthSlider_valueChanged(int value);

    void on_enableCriticalPathFilterCheckBox_stateChanged(int arg1);

private:
    Ui::EntryFilterSettings *ui;
};

#endif // ENTRYFILTERSETTINGS_H
