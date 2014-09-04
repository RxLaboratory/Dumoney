#include "editeurcategories.h"
#include "dufsqlquery.h"
#include <QMessageBox>

EditeurCategories::EditeurCategories(QWidget *parent) :
    QDialog(parent)
{
    db = QSqlDatabase::database();

    setupUi(this);

    reInit();
}

void EditeurCategories::refresh()
{
    //vider la liste
    liste->clear();

    QString q = "SELECT nom FROM Categories ORDER BY nom;";
    DufSqlQuery query(q,db);
    query.verif();
    while (query.next())
    {
        liste->addItem(query.value(0).toString());
    }

}

void EditeurCategories::reInit()
{
    boutonsValid->setEnabled(false);
    nom->setEnabled(false);
    nom->setText("");
    id->setText("");
}

void EditeurCategories::displayItem(QString name)
{
    reInit();

    QString q = "SELECT id FROM Categories WHERE ";
    q += "nom = '" + name.replace("'","''") + "'";
    DufSqlQuery query(q,db);
    query.verif();

    query.first();

    nom->setText(name);
    id->setText(query.value(0).toString());

    boutonsValid->setEnabled(true);
    nom->setEnabled(true);
}

void EditeurCategories::on_liste_itemClicked(QListWidgetItem *item)
{
    displayItem(item->text());
}

void EditeurCategories::on_ajouter_clicked()
{
    QString q = "INSERT INTO Categories(nom) VALUES ('Nouvelle');";
    DufSqlQuery(q,db).verif();
    refresh();
    displayItem("Nouvelle");
}

void EditeurCategories::on_modifier_clicked()
{
    QString q = "UPDATE Categories SET ";
    q += "nom = '" + nom->text() + "'";
    q += " WHERE id = " + id->text() + " ;";
    DufSqlQuery(q,db).verif();
    refresh();
    reInit();
}

void EditeurCategories::on_supprimer_clicked()
{
    if (QMessageBox::warning(this, "Suppression d'une catégorie", "Voulez vous vraiment supprimer cette catégorie ?\n\n" + nom->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString q = "DELETE FROM Categories WHERE id = " + id->text() + ";";
        DufSqlQuery(q,db).verif();
        refresh();
        reInit();
    }

}
