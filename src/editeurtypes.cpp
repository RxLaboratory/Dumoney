#include "editeurtypes.h"
#include "dufsqlquery.h"
#include <QtDebug>
#include <QSqlError>
#include <QMessageBox>

EditeurTypes::EditeurTypes(QWidget *parent) :
    QDialog(parent)
{
    db = QSqlDatabase::database();

    setupUi(this);

    reInit();
}

void EditeurTypes::refresh()
{
    //vider la liste
    liste->clear();

    QString q = "SELECT nom FROM Types ORDER BY nom;";
    DufSqlQuery query(q,db);
    query.verif();
    while (query.next())
    {
        liste->addItem(query.value(0).toString());
    }

}

void EditeurTypes::reInit()
{
    boutonsValid->setEnabled(false);
    box->setEnabled(false);
    nom->setText("");
    cotiz->setValue(0);
    immediat->setChecked(true);
    jourCotiz->setEnabled(false);
    id->setText("");
}

void EditeurTypes::displayItem(QString name)
{
    reInit();

    QString q = "SELECT id,tauxCotisations,decalageCotisations,jourCotisations FROM Types WHERE ";
    q += "nom = '" + name.replace("'","''") + "'";
    DufSqlQuery query(q,db);
    query.verif();

    query.first();

    nom->setText(name);
    id->setText(query.value(0).toString());

    if(query.value(0).toInt() == 0)
    {
        return;
    }

    boutonsValid->setEnabled(true);
    box->setEnabled(true);
    jourCotiz->setValue(query.value(3).toInt());
    cotiz->setValue(query.value(1).toDouble());
    immediat->setChecked(query.value(2).toInt() == 0);
    mensuel->setChecked(query.value(2).toInt() == 1);
    trimestriel->setChecked(query.value(2).toInt() == 2);
}

void EditeurTypes::on_liste_itemClicked(QListWidgetItem *item)
{
    displayItem(item->text());
}

void EditeurTypes::on_ajouter_clicked()
{
    QString q = "INSERT INTO Types(nom,tva,tauxCotisations,decalageCotisations,jourCotisations) VALUES ('Nouveau',0,0,0,15);";
    DufSqlQuery query(q,db);
    refresh();
    displayItem("Nouveau");
}

void EditeurTypes::on_modifier_clicked()
{
    QString q = "UPDATE Types SET ";
    q += "nom = '" + nom->text() + "',";
    q += "tva = 0,";
    q += "tauxCotisations = " + QString::number(cotiz->value()) + ",";
    if (immediat->isChecked()) q += "decalageCotisations = 0,";
    else if (mensuel->isChecked()) q += "decalageCotisations = 1,";
    else if (trimestriel->isChecked()) q += "decalageCotisations = 2,";
    q += "jourCotisations = " + QString::number(jourCotiz->value());
    q += " WHERE id = " + id->text() + " ;";
    DufSqlQuery(q,db).verif();
    refresh();
    reInit();
}

void EditeurTypes::on_supprimer_clicked()
{
    if (QMessageBox::warning(this, "Suppression d'un type de credit", "Voulez vous vraiment supprimer ce type de credit ?\n\n" + nom->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString q = "DELETE FROM Types WHERE id = " + id->text() + ";";
        DufSqlQuery(q,db);
        refresh();
        reInit();
    }

}

void EditeurTypes::on_immediat_toggled(bool checked)
{
    jourCotiz->setEnabled(!checked);
}
