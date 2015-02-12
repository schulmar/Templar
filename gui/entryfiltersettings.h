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

private:
    Ui::EntryFilterSettings *ui;
};

#endif // ENTRYFILTERSETTINGS_H
