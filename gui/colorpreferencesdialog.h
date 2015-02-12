#ifndef COLORPREFERENCESDIALOG_H
#define COLORPREFERENCESDIALOG_H

#include <QDialog>

class QModelIndex;
namespace Ui {
class ColorPreferencesDialog;
}

class ColorPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorPreferencesDialog(QWidget *parent = 0);
    ~ColorPreferencesDialog();

private slots:

    void on_colorTableWidget_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

private:
    Ui::ColorPreferencesDialog *ui;
};

#endif // COLORPREFERENCESDIALOG_H
