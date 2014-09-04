#ifndef DEBIT_H
#define DEBIT_H

#include "ui_debit.h"
#include <QSqlDatabase>

class Debit : public QDialog, private Ui::Debit
{
    Q_OBJECT
    
public:
    explicit Debit(bool nouv,QString iD = 0,QDate dateBase = QDate::currentDate(),QWidget *parent = 0);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    QSqlDatabase db;
    QString id;
    bool nouveau;
};

#endif // DEBIT_H
