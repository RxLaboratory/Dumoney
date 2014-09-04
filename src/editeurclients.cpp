#include "editeurclients.h"
#include "dufsqlquery.h"
#include <QMessageBox>

EditeurClients::EditeurClients(QWidget *parent) :
    QDialog(parent)
{
    db = QSqlDatabase::database();

    setupUi(this);

    reInit();
}

void EditeurClients::reInit()
{
    boutonsValid->setEnabled(false);
    box->setEnabled(false);
    nom->setText("");
    horaire->setValue(0);
    type->setCurrentIndex(0);
    categorie->setCurrentIndex(0);
    id->setText("");
}

void EditeurClients::refresh()
{
    type->clear();
    type->addItem("Type par defaut");
    QString q = "SELECT nom FROM Types ORDER BY nom;";
    DufSqlQuery queryTypes(q,db);
    while(queryTypes.next())
    {
        type->addItem(queryTypes.value(0).toString());
    }
    categorie->clear();
    categorie->addItem("Categorie");
    q = "SELECT nom FROM Categories ORDER BY nom;";
    DufSqlQuery queryCategories(q,db);
    while(queryCategories.next())
    {
        categorie->addItem(queryCategories.value(0).toString());
    }


    //vider la liste
    liste->clear();

    q = "SELECT nom FROM Clients ORDER BY nom;";
    DufSqlQuery query(q,db);
    query.verif();
    while (query.next())
    {
        liste->addItem(query.value(0).toString());
    }

}

void EditeurClients::displayItem(QString name)
{
    reInit();

    QString q = "SELECT Clients.id,Types.nom,tauxHoraire,Categories.nom FROM Clients";
    q += " LEFT JOIN Types ON Types.id = Clients.type";
    q += " LEFT JOIN Categories ON Categories.id = Clients.categorie";
    q += " WHERE Clients.nom = '" + name.replace("'","''") + "';";
    DufSqlQuery query(q,db);
    query.verif();

    query.first();

    nom->setText(name);
    id->setText(query.value(0).toString());

    boutonsValid->setEnabled(true);
    box->setEnabled(true);
    horaire->setValue(query.value(2).toDouble());
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
        if (categorie->itemText(i) == query.value(3).toString())
        {
            categorie->setCurrentIndex(i);
            break;
        }
    }
}

void EditeurClients::on_liste_itemClicked(QListWidgetItem *item)
{
    displayItem(item->text());
}

void EditeurClients::on_ajouter_clicked()
{
    QString q = "INSERT INTO Clients(nom) VALUES ('Nouveau');";
    DufSqlQuery query(q,db);
    refresh();
    displayItem("Nouveau");
}

void EditeurClients::on_modifier_clicked()
{
    QString q = "UPDATE Clients SET ";
    q += "nom = '" + nom->text() + "',";
    if (type->currentIndex() != 0) q += "type = (SELECT id FROM Types WHERE Types.nom = '" + type->currentText() + "'),";
    if (categorie->currentIndex() != 0) q += "categorie = (SELECT id FROM Categories WHERE Categories.nom = '" + categorie->currentText() + "'),";
    q += "tauxHoraire = " + QString::number(horaire->value());
    q += " WHERE id = " + id->text() + " ;";
    DufSqlQuery(q,db).verif();
    refresh();
    reInit();
}

void EditeurClients::on_supprimer_clicked()
{
    if (QMessageBox::warning(this, "Suppression d'un client", "Voulez vous vraiment supprimer ce client ?\n\n" + nom->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString q = "DELETE FROM Clients WHERE id = " + id->text() + ";";
        DufSqlQuery(q,db);
        refresh();
        reInit();
    }

}


