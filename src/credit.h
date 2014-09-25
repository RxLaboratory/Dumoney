#ifndef CREDIT_H
#define CREDIT_H

#include "ui_credit.h"
#include <QSqlDatabase>

class Credit : public QDialog, private Ui::Credit
{
    Q_OBJECT
    
public:
    explicit Credit(bool nouv,QString iD = 0,QDate dateBase = QDate::currentDate(),QWidget *parent = 0);

private slots:
    void on_buttonBox_accepted();

    void on_calcul_clicked();

    void on_dateF_editingFinished();

    void on_client_activated(const QString &arg1);

    void on_type_currentIndexChanged(const QString &arg1);

    void on_dateP_dateChanged(const QDate &date);

private:
    QSqlDatabase db;
    QString id;
    bool nouveau;
};

#endif // CREDIT_H
