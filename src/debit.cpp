#include "debit.h"
#include "dufsqlquery.h"
#include <QtDebug>

Debit::Debit(bool nouv, QString iD, QDate dateBase, QWidget *parent) :
    QDialog(parent)
{
    db = QSqlDatabase::database();
    id = iD;
    nouveau = nouv;

    setupUi(this);

    //remplir les combobox
    QString q = "SELECT nom FROM Categories ORDER BY nom;";
    DufSqlQuery queryCategories(q,db);
    while(queryCategories.next())
    {
        categorie->addItem(queryCategories.value(0).toString());
    }

    dateP->setDate(dateBase);

    if (nouveau) return;

    q = "SELECT Debits.nom,montant,paye,Categories.nom,Historique.date FROM Debits ";
    q += " LEFT JOIN Categories ON Debits.categorie = Categories.id ";
    q += " LEFT JOIN Historique ON Debits.id = Historique.debit ";
    q += " WHERE Debits.id = '" + id + "';";
    DufSqlQuery query(q,db);
    query.verif();
    query.first();
    nom->setText(query.value(0).toString());
    brut->setValue(- query.value(1).toDouble());
    paye->setChecked(query.value(2).toBool());
    for (int i = 1; i< categorie->count();i++)
    {
        if (categorie->itemText(i) == query.value(3).toString())
        {
            categorie->setCurrentIndex(i);
            break;
        }
    }
    dateP->setDate(QDate::fromString(query.value(4).toString(),"yyyyMMdd"));
}

void Debit::on_buttonBox_accepted()
{
    QString q = "UPDATE Debits SET";
    q += " nom = '" + nom->text() + "',";
    q += " montant = -" + QString::number(brut->value()) + ",";
    if (paye->isChecked()) q += " paye = 1,";
    else q += " paye = 0,";
    if (categorie->currentIndex() != 0) q += " categorie = (SELECT id FROM Categories WHERE nom = '" + categorie->currentText().replace("'","''") + "')";
    q += " WHERE Debits.id = " + id + ";";
    DufSqlQuery(q,db).verif();
    q = "UPDATE Historique SET date = " + dateP->date().toString("yyyyMMdd") + " WHERE debit = " + id + ";";
    DufSqlQuery(q,db).verif();
    accept();
    close();
}

void Debit::on_buttonBox_rejected()
{
    close();
}
