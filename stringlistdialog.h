#ifndef STRINGLISTDIALOG_H
#define STRINGLISTDIALOG_H

#include <QDialog>

namespace Ui {
class StringListDialog;
}

class QStringList;

class StringListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StringListDialog(QString elemType, QWidget *parent = 0);
    ~StringListDialog();

    QStringList getStringList() const;

    void setDefaultValue(const QString& str) {
        defaultValue = str;
    }

    QString getDefaultValue() const {
        return defaultValue;
    }

		//silence clang warning
		// -> "unhide" this function (because of done()) 
    using QDialog::done;
public slots:
    void addItem();
    void deleteItem();
    void deleteAllItem();
    void done();
    
private:
    Ui::StringListDialog *ui;
    QString elemType;
    QString defaultValue;
};

#endif // STRINGLISTDIALOG_H
