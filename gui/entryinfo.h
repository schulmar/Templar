#ifndef ENTRYINFO_H
#define ENTRYINFO_H

#include <QWidget>

namespace Ui {
class EntryInfo;
}

class EntryInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit EntryInfo(QWidget *parent = 0);
    ~EntryInfo();

    void setEventType(const QString& event);
    void setKind(const QString& kind);
    void setName(const QString& name);
    void setFilePosition(const QString& filePos);
    
private:
    Ui::EntryInfo *ui;
};

#endif // ENTRYINFO_H
