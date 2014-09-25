#include "credit.h"
#include "dufsqlquery.h"
#include <QtDebug>
#include <QDate>

Credit::Credit(bool nouv, QString iD, QDate dateBase, QWidget *parent) :
    QDialog(parent)
{
    db = QSqlDatabase::database();
    id = iD;
    nouveau = nouv;

    setupUi(this);

    //remplir les combobox
    QString q = "SELECT nom FROM Types ORDER BY nom;";
    DufSqlQuery queryTypes(q,db);
    while(queryTypes.next())
    {
        type->addItem(queryTypes.value(0).toString());
    }
    q = "SELECT nom FROM Categories ORDER BY nom;";
    DufSqlQuery queryCategories(q,db);
    while(queryCategories.next())
    {
        categorie->addItem(queryCategories.value(0).toString());
    }
    q = "SELECT nom FROM Clients ORDER BY nom;";
    DufSqlQuery queryClients(q,db);
    while(queryClients.next())
    {
        client->addItem(queryClients.value(0).toString());
    }

    dateF->setDate(dateBase);
    dateP->setDate(dateBase);
    dateC->setDate(dateBase);

    if (nouveau) return;

    //                 0           1      2     3        4                     5               5           7     8    9       10
    q = "SELECT Types.nom,Credits.nom,montant,paye,dateFacturation,Historique.date,Categories.nom,Clients.nom,heures,net,dateCotisations FROM Credits ";
    q += " LEFT JOIN Types ON Credits.type = Types.id ";
    q += " LEFT JOIN Categories ON Credits.categorie = Categories.id ";
    q += " LEFT JOIN Clients ON Credits.client = Clients.id ";
    q += " LEFT JOIN Historique ON Credits.id = Historique.credit ";
    q += " WHERE Credits.id = " + id + ";";
    DufSqlQuery query(q,db);
    query.first();
    nom->setText(query.value(1).toString());
    for (int i = 1; i< type->count();i++)
    {
        if (type->itemText(i) == query.value(0).toString())
        {
            type->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 1; i< categorie->count();i++)
    {
        if (categorie->itemText(i) == query.value(6).toString())
        {
            categorie->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 1; i< client->count();i++)
    {
        if (client->itemText(i) == query.value(7).toString())
        {
            client->setCurrentIndex(i);
            break;
        }
    }
    heures->setValue(query.value(8).toInt());
    brut->setValue(query.value(2).toDouble());
    net->setValue(query.value(9).toDouble());
    dateF->setDate(QDate::fromString(query.value(4).toString(),"yyyyMMdd"));
    paye->setChecked(query.value(3).toBool());
    dateP->setDate(QDate::fromString(query.value(5).toString(),"yyyyMMdd"));
    dateC->setDate(QDate::fromString(query.value(10).toString(),"yyyyMMdd"));

}

void Credit::on_buttonBox_accepted()
{
    QString q = "UPDATE Credits SET";
    if (type->currentIndex() != 0) q += " type = (SELECT id FROM Types WHERE nom = '" + type->currentText().replace("'","''") + "'),";
    q += " nom = '" + nom->text() + "',";
    q += " montant = " + QString::number(brut->value()) + ",";
    if (paye->isChecked()) q += " paye = 1,";
    else q += " paye = 0,";
    q += " dateFacturation = " + dateF->date().toString("yyyyMMdd") + ",";
    if (categorie->currentIndex() != 0) q += " categorie = (SELECT id FROM Categories WHERE nom = '" + categorie->currentText().replace("'","''") + "'),";
    if (client->currentIndex() != 0) q += " client = (SELECT id FROM Clients WHERE nom = '" + client->currentText().replace("'","''") + "'),";
    q += " heures = " + QString::number(heures->value()) + ",";
    q += " net = " + QString::number(net->value()) + ",";
    q += " dateCotisations = " + dateC->date().toString("yyyyMMdd");
    q += " WHERE Credits.id = " + id + ";";
    DufSqlQuery(q,db).verif();
    q = "UPDATE Historique SET date = " + dateP->date().toString("yyyyMMdd") + " WHERE credit = " + id + ";";
    DufSqlQuery(q,db).verif();
}

void Credit::on_calcul_clicked()
{
    if (type->currentIndex() == 0)
    {
        net->setValue(0);
        return;
    }
    QString q = "SELECT tauxCotisations,decalageCotisations,jourCotisations FROM Types WHERE nom = '" + type->currentText().replace("'","''") + "';";
    DufSqlQuery query(q,db);
    q = "SELECT tauxHoraire FROM Clients WHERE nom = '" + client->currentText() + "';";
    DufSqlQuery queryH(q,db);
    query.verif();
    queryH.verif();
    query.first();
    queryH.first();
    if (queryH.value(0).toDouble() != 0)
    {
        brut->setValue(heures->value()*queryH.value(0).toDouble());
    }
    net->setValue(brut->value() - brut->value() * query.value(0).toDouble() / 100);
    if (query.value(1).toInt() == 0) dateC->setDate(dateP->date());
    else if (query.value(1).toInt() == 1) dateC->setDate(QDate(dateP->date().year(),dateP->date().month()+1,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 1 && dateP->date().month() <= 3) dateC->setDate(QDate(dateP->date().year(),4,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 4 && dateP->date().month() <= 6) dateC->setDate(QDate(dateP->date().year(),7,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 7 && dateP->date().month() <= 9) dateC->setDate(QDate(dateP->date().year(),10,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 10 && dateP->date().month() <= 12) dateC->setDate(QDate(dateP->date().year()+1,1,query.value(2).toInt()));
}

void Credit::on_dateF_editingFinished()
{
    if (dateP->date() < dateF->date()) dateP->setDate(dateF->date());
}

void Credit::on_client_activated(const QString &arg1)
{
    QString q = "SELECT Categories.nom,Types.nom FROM Clients";
    q += " JOIN Categories ON Categories.id = Clients.categorie";
    q += " JOIN Types ON Types.id = Clients.type";
    q += " WHERE Clients.nom = '" + arg1 + "';";
    DufSqlQuery query(q,db);
    query.verif();
    query.first();
    for (int i = 1; i< type->count();i++)
    {
        if (type->itemText(i) == query.value(1).toString())
        {
            type->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 1; i< categorie->count();i++)
    {
        if (categorie->itemText(i) == query.value(0).toString())
        {
            categorie->setCurrentIndex(i);
            break;
        }
    }
    if(nom->text() == "") nom->setText(arg1);
}

void Credit::on_type_currentIndexChanged(const QString &arg1)
{
    QString q = "SELECT tauxCotisations,decalageCotisations,jourCotisations FROM Types WHERE nom = '" + type->currentText().replace("'","''") + "';";
    DufSqlQuery query(q,db);
    query.verif();
    query.first();

    if (query.value(1).isValid() && query.value(1).toInt() == 0)
    {
        dateC->setEnabled(false);
    }
    else
    {
        dateC->setEnabled(true);
    }
    if (query.value(1).toInt() == 0) dateC->setDate(dateP->date());
    else if (query.value(1).toInt() == 1) dateC->setDate(QDate(dateP->date().year(),dateP->date().month()+1,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 1 && dateP->date().month() <= 3) dateC->setDate(QDate(dateP->date().year(),4,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 4 && dateP->date().month() <= 6) dateC->setDate(QDate(dateP->date().year(),7,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 7 && dateP->date().month() <= 9) dateC->setDate(QDate(dateP->date().year(),10,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 10 && dateP->date().month() <= 12) dateC->setDate(QDate(dateP->date().year()+1,1,query.value(2).toInt()));
}

void Credit::on_dateP_dateChanged(const QDate &date)
{
    QString q = "SELECT tauxCotisations,decalageCotisations,jourCotisations FROM Types WHERE nom = '" + type->currentText().replace("'","''") + "';";
    DufSqlQuery query(q,db);
    query.verif();
    query.first();
    if (query.value(1).toInt() == 0) dateC->setDate(dateP->date());
    else if (query.value(1).toInt() == 1) dateC->setDate(QDate(dateP->date().year(),dateP->date().month()+1,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 1 && dateP->date().month() <= 3) dateC->setDate(QDate(dateP->date().year(),4,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 4 && dateP->date().month() <= 6) dateC->setDate(QDate(dateP->date().year(),7,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 7 && dateP->date().month() <= 9) dateC->setDate(QDate(dateP->date().year(),10,query.value(2).toInt()));
    else if (query.value(1).toInt() == 2 && dateP->date().month() >= 10 && dateP->date().month() <= 12) dateC->setDate(QDate(dateP->date().year()+1,1,query.value(2).toInt()));
}
