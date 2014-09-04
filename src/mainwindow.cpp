#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include <QMessageBox>
#include <QtDebug>
#include <QDir>
#include <QFile>
#include "credit.h"
#include "debit.h"
#include "dufsqlquery.h"
#include <QTimer>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //============= BDD ===============
    //connection bdd
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("comptes.s3db");
    db.setHostName("localhost");

    setupUi(this);

    couleurPositif = QColor(110,234,107);
    couleurNegatif = QColor(230,83,83);
    couleurFutur = QColor(83,204,230);

    editeurTypes = new EditeurTypes();
    editeurCategories = new EditeurCategories();
    editeurClients = new EditeurClients();

    //mettre l'année
    annee->setValue(QDate::currentDate().year());
    labelAnnee->setText(QDate::currentDate().toString("yyyy"));
    labelMois->setText(QDate::currentDate().toString("MMMM"));

    //date
    dateActuelle->setDateTime(QDateTime::currentDateTime());

    //lancer le thread de mise à jour de date
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(dateUp()));
    timer->start(1000);

}

void MainWindow::dateUp()
{
    dateActuelle->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::refresh()
{
    QString rech = recherche->text().toLower();

    table->clearContents();
    table->setRowCount(0);

    QDate rangeTop;
    QDate rangeBottom;

    if (mois->currentIndex() == 0)
    {
        rangeTop = QDate(annee->value()+1,1,1);
        rangeBottom = QDate(annee->value(),1,1);
    }
    else
    {
        if (mois->currentIndex()<12)
        {
            rangeTop = QDate(annee->value(),mois->currentIndex()+1,1);
        }
        else
        {
            rangeTop = QDate(annee->value()+1,1,1);
        }
        rangeBottom = QDate(annee->value(),mois->currentIndex(),1);
    }


    //récupérer les crédits et les débits
    //                           0               1                 2              3             1              5                6
    QString q = "SELECT Credits.id,Credits.dateFacturation,Credits.nom,Credits.heures,Credits.montant,Credits.net,Types.decalageCotisations";
    //             7        8           9
    q += ",Debits.id,Debits.nom,Debits.montant";
    //                 10
    q += ",Historique.date";
    //              11                 12                13
    q += ",Credits.paye,Credits.dateCotisations,Debits.paye";
    q += " FROM Historique";
    q += " LEFT JOIN Credits ON Credits.id = Historique.credit";
    q += " LEFT JOIN Debits ON Debits.id = Historique.debit";
    q += " LEFT JOIN Types ON Types.id = Credits.type";
    q += " ORDER BY Historique.date;";
    DufSqlQuery query(q,db);
    query.verif();

    //construire le tableau
    int i = 1;
    int heures = 0;
    double total = 0;
    double totalMois = 0;
    double solde = 0;
    int heuresMois = 0;
    int heuresAnnee = 0;
    double variationAnnee = 0;
    double variationMois = 0;
    double totalAnnee = 0;
    double totalActuel = 0;
    while(query.next())
    {

        QDate dateItem = QDate::fromString(query.value(10).toString(),"yyyyMMdd");
        QDate dateCotiz = QDate::fromString(query.value(12).toString(),"yyyyMMdd");

        QTableWidgetItem *itemId = new QTableWidgetItem("");
        QTableWidgetItem *itemDate = new QTableWidgetItem("");
        QTableWidgetItem *itemDateF = new QTableWidgetItem("");
        QTableWidgetItem *itemNom = new QTableWidgetItem("");
        QTableWidgetItem *itemHeures = new QTableWidgetItem("");
        QTableWidgetItem *itemMontant = new QTableWidgetItem("");

        //si la date correspond
        if ((dateItem >= rangeBottom && dateItem < rangeTop) || actionAfficher_tout->isChecked() )
        {
            itemDate->setText(dateItem.toString("dd/MM/yyyy"));
            //si crédit
            if (query.value(0).toString() != "")
            {
                itemId->setText(query.value(0).toString());
                itemId->setToolTip("Credit");
                itemDateF->setText(QDate::fromString(query.value(1).toString(),"yyyyMMdd").toString("dd/MM/yyyy"));
                itemNom->setText(query.value(2).toString());
                itemHeures->setText(query.value(3).toString());
                heures += query.value(3).toInt();
                if (query.value(6).toInt() == 0)
                {
                    itemMontant->setText(query.value(5).toString());
                    total += query.value(5).toDouble();
                }
                else
                {
                    itemMontant->setText(query.value(4).toString());
                    total += query.value(4).toDouble();
                }
            }
            else
            {
                itemId->setText(query.value(7).toString());
                itemId->setToolTip("Debit");
                itemNom->setText(query.value(8).toString());
                itemMontant->setText(query.value(9).toString());
                total += query.value(9).toDouble();
            }
        }

        //soldes
        if (dateItem < rangeTop || actionAfficher_tout->isChecked())
        {
            if (query.value(0).toString() != "")
            {
                if (dateCotiz < rangeTop || actionAfficher_tout->isChecked())
                {
                    solde += query.value(5).toDouble();
                }
                else
                {
                    solde += query.value(4).toDouble();
                }
            }
            else
            {
                solde += query.value(9).toDouble();
            }
        }

        //couleurs
        if (itemMontant->text().startsWith("-"))
        {
            itemMontant->setBackgroundColor(couleurNegatif);
        }
        else
        {
            itemMontant->setBackgroundColor(couleurPositif);
        }
        if ((!query.value(13).toBool() && query.value(7).toString() != "") || (!query.value(11).toBool() && query.value(0).toString() != "") || QDate::fromString(query.value(10).toString(),"yyyyMMdd") > QDate::currentDate())
        {
            itemId->setBackgroundColor(couleurFutur);
        }

        //mise en page
        itemId->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        itemDate->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        itemDateF->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        itemHeures->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        itemMontant->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        if (itemId->text() != "" && itemNom->text().toLower().contains(rech))
        {
            table->setRowCount(i);
            table->setItem(i-1,0,itemId);
            table->setItem(i-1,1,itemDate);
            table->setItem(i-1,2,itemDateF);
            table->setItem(i-1,3,itemNom);
            table->setItem(i-1,4,itemHeures);
            table->setItem(i-1,5,itemMontant);
            i++;
        }

        //vérifier si ya pas une cotisation
        if ((dateCotiz >= rangeBottom && dateCotiz < rangeTop && query.value(6).toInt() != 0) || actionAfficher_tout->isChecked())
        {
            QTableWidgetItem *itemId2 = new QTableWidgetItem("");
            QTableWidgetItem *itemDate2 = new QTableWidgetItem("");
            QTableWidgetItem *itemDateF2 = new QTableWidgetItem("");
            QTableWidgetItem *itemNom2 = new QTableWidgetItem("");
            QTableWidgetItem *itemHeures2 = new QTableWidgetItem("");
            QTableWidgetItem *itemMontant2 = new QTableWidgetItem("");
            itemDate2->setText(dateCotiz.toString("dd/MM/yyyy"));
            itemId2->setText(query.value(0).toString());
            itemId2->setToolTip("Cotisation");
            itemDateF2->setText(QDate::fromString(query.value(1).toString(),"yyyyMMdd").toString("dd/MM/yyyy"));
            itemNom2->setText("Cotisations de " + query.value(2).toString());
            itemMontant2->setText("-" + QString::number(query.value(4).toDouble()-query.value(5).toDouble()));
            //couleurs
            if (itemMontant2->text().startsWith("-"))
            {
                itemMontant2->setBackgroundColor(couleurNegatif);
            }
            else
            {
                itemMontant2->setBackgroundColor(couleurPositif);
            }
            if (!query.value(11).toBool() && query.value(0).toString() != "" || query.value(11).toDate() > QDate::currentDate())
            {
                itemId2->setBackgroundColor(couleurFutur);
            }

            //mise en page
            itemId2->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            itemDate2->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            itemDateF2->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            itemHeures2->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            itemMontant2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            if (itemId2->text() != "" && itemNom2->text().toLower().contains(rech))
            {
                table->setRowCount(i);
                table->setItem(i-1,0,itemId2);
                table->setItem(i-1,1,itemDate2);
                table->setItem(i-1,2,itemDateF2);
                table->setItem(i-1,3,itemNom2);
                table->setItem(i-1,4,itemHeures2);
                table->setItem(i-1,5,itemMontant2);
                i++;
            }
        }

        //ajouter le compte des totaux
        //solde actuel
        if (dateItem <= QDate::currentDate())
        {
            //si crédit payé
            if ((query.value(0).toString() != "" && query.value(11).toBool()))
            {
                //si cotisations payées, le net
                if (dateCotiz <= QDate::currentDate())
                {
                    totalActuel += query.value(5).toDouble();
                }
                //sinon le brut
                else
                {
                    totalActuel += query.value(4).toDouble();
                }
            }
            //si débit
            else
            {
                totalActuel += query.value(9).toDouble();
            }
        }

        //total du mois
        if (dateItem.month() <= QDate::currentDate().month() && dateItem.year() <= QDate::currentDate().year())
        {
            //si crédit
            if (query.value(0).toString() != "")
            {
                //si cotisations payées
                if (dateCotiz.month() <= QDate::currentDate().month() && dateCotiz.year() <= QDate::currentDate().year())
                {
                    totalMois += query.value(5).toDouble();
                    //variation
                    if (dateItem.month() == QDate::currentDate().month() && dateItem.year() == QDate::currentDate().year())
                    {
                        variationMois += query.value(5).toDouble();
                        heuresMois += query.value(3).toInt();
                    }
                    else if (dateCotiz.month() == QDate::currentDate().month() && dateCotiz.year() == QDate::currentDate().year())
                    {
                        variationMois += -query.value(4).toDouble()+query.value(5).toDouble();
                    }
                }
                //sinon le brut
                else
                {
                    totalMois += query.value(4).toDouble();
                    //variation
                    if (dateItem.month() == QDate::currentDate().month() && dateItem.year() == QDate::currentDate().year())
                    {
                        variationMois += query.value(4).toDouble();
                        heuresMois += query.value(3).toInt();
                    }
                }
            }
            //si débit
            else
            {
                totalMois += query.value(9).toDouble();
                //variation
                if (dateItem.month() == QDate::currentDate().month() && dateItem.year() == QDate::currentDate().year())
                {
                    variationMois += query.value(9).toDouble();
                }
            }
        }

        //total de l'année
        if (dateItem.year() <= QDate::currentDate().year())
        {
            //si crédit
            if (query.value(0).toString() != "")
            {
                //si cotisations payées, le net
                if (dateCotiz.year() <= QDate::currentDate().year())
                {
                    totalAnnee += query.value(5).toDouble();
                    //variation
                    if (dateItem.year() == QDate::currentDate().year())
                    {
                        variationAnnee += query.value(5).toDouble();
                        heuresAnnee += query.value(3).toInt();
                    }
                    else if (dateCotiz.year() == QDate::currentDate().year())
                    {
                        variationAnnee += -query.value(4).toDouble()+query.value(5).toDouble();
                    }
                }
                //sinon le brut
                else
                {
                    totalAnnee += query.value(4).toDouble();
                    //variation
                    if (dateItem.year() == QDate::currentDate().year())
                    {
                        variationAnnee += query.value(4).toDouble();
                        heuresAnnee += query.value(3).toInt();
                    }
                }
            }
            //si débit
            else
            {
                totalAnnee += query.value(9).toDouble();
                //variation
                if (dateItem.year() == QDate::currentDate().year())
                {
                    variationAnnee += query.value(9).toDouble();
                }
            }
        }

    }

    //ajouter totaux
    QTableWidgetItem *itemNom = new QTableWidgetItem("Total");
    QTableWidgetItem *itemHeures = new QTableWidgetItem(QString::number(heures));
    QTableWidgetItem *itemMontant = new QTableWidgetItem(QString::number(total));
    QTableWidgetItem *itemNom2 = new QTableWidgetItem("Nouveau solde");
    QTableWidgetItem *itemMontant2 = new QTableWidgetItem(QString::number(solde));
    //couleurs
    if (itemMontant->text().startsWith("-"))
    {
        itemMontant->setBackgroundColor(couleurNegatif);
    }
    else
    {
        itemMontant->setBackgroundColor(couleurPositif);
    }
    if (itemMontant2->text().startsWith("-"))
    {
        itemMontant2->setBackgroundColor(couleurNegatif);
    }
    else
    {
        itemMontant2->setBackgroundColor(couleurPositif);
    }
    //mise en page
    itemHeures->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    itemMontant->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itemMontant2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    table->setRowCount(i+1);
    table->setItem(i-1,3,itemNom);
    table->setItem(i-1,4,itemHeures);
    table->setItem(i-1,5,itemMontant);
    table->setItem(i,3,itemNom2);
    table->setItem(i,5,itemMontant2);

    //resize colonnes
    table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);


    totActuel->setValue(totalActuel);
    totMois->setValue(totalMois);
    totAnnee->setValue(totalAnnee);
    vMois->setValue(variationMois);
    vAnnee->setValue(variationAnnee);
    hMois->setValue(heuresMois);
    hAnnee->setValue(heuresAnnee);
}

void MainWindow::on_actionA_Propos_de_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionA_Propos_triggered()
{
    About *about = new About();
    about->show();
}

void MainWindow::on_actionQuitter_triggered()
{
    close();
}

void MainWindow::on_actionTypes_triggered()
{
    editeurTypes->refresh();
    editeurTypes->exec();
}

void MainWindow::on_actionCat_gories_triggered()
{
    editeurCategories->refresh();
    editeurCategories->exec();
}

void MainWindow::on_actionAjouter_un_cr_dit_triggered()
{
    QString q = "INSERT INTO Credits(type,nom) VALUES (0,'Nouveau');";
    DufSqlQuery(q,db).verif();
    q = "SELECT last_insert_rowid();";
    DufSqlQuery query(q,db);
    query.first();
    q = "INSERT INTO Historique(credit) VALUES (" + query.value(0).toString() + ");";
    DufSqlQuery(q,db).verif();

    QDate date;
    if (mois->currentIndex() == 0) date = QDate(annee->value(),1,1);
    else date = QDate(annee->value(),mois->currentIndex(),1);

    Credit credit(true,query.value(0).toString(),date);
    if (!credit.exec())
    {
        QString q = "DELETE FROM Credits WHERE id = " + query.value(0).toString() + ";";
        DufSqlQuery(q,db);
        q = "DELETE FROM Historique WHERE credit = " + query.value(0).toString() + ";";
        DufSqlQuery(q,db);
    }
    else
    {
        refresh();
    }

}

void MainWindow::on_annee_valueChanged(int arg1)
{
    refresh();
}

void MainWindow::on_mois_activated(int index)
{
    refresh();
}

void MainWindow::on_actionSupprimer_triggered()
{
    if (table->currentRow() >= table->rowCount()-2) return;

    QString q;

    if (table->item(table->currentRow(),0)->toolTip() == "Debit")
    {
        if (QMessageBox::warning(this, "Suppression d'un debit", "Voulez vous vraiment supprimer ce debit ?\n\n" + table->item(table->currentRow(),3)->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            q = "DELETE FROM Historique WHERE debit = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            q = "DELETE FROM Debits WHERE id = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            refresh();
        }
    }
    else if (table->item(table->currentRow(),0)->toolTip() == "Credit")
    {
        if (QMessageBox::warning(this, "Suppression d'un credit", "Voulez vous vraiment supprimer ce credit ?\n\n" + table->item(table->currentRow(),3)->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            q = "DELETE FROM Historique WHERE credit = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            q = "DELETE FROM Credits WHERE id = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            refresh();
        }
    }
    else if (table->item(table->currentRow(),0)->toolTip() == "Cotisation")
    {
        if (QMessageBox::warning(this, "Suppression d'un credit", "La suppression de cette cotisation supprimera aussi le credit associe, voulez-vous continuer ?\n\n" + table->item(table->currentRow(),3)->text(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            q = "DELETE FROM Historique WHERE credit = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            q = "DELETE FROM Credits WHERE id = " + table->item(table->currentRow(),0)->text() + ";";
            DufSqlQuery(q,db).verif();
            refresh();
        }
    }
}

void MainWindow::on_actionModifier_triggered()
{
    if (table->currentRow() >= table->rowCount()-2) return;

    if (table->item(table->currentRow(),0)->toolTip() == "Credit" || table->item(table->currentRow(),0)->toolTip() == "Cotisation")
    {
        Credit credit(false,table->item(table->currentRow(),0)->text());
        if (credit.exec())
        {
            refresh();
        }
    }
    else if (table->item(table->currentRow(),0)->toolTip() == "Debit")
    {
        Debit debit(false,table->item(table->currentRow(),0)->text());
        if (debit.exec())
        {
            refresh();
        }
    }
}

void MainWindow::on_table_cellDoubleClicked(int row, int column)
{
    on_actionModifier_triggered();
}

void MainWindow::on_actionAjouter_un_d_bit_triggered()
{
    QString q = "INSERT INTO Debits(nom,montant) VALUES ('Nouveau',0);";
    DufSqlQuery(q,db).verif();
    q = "SELECT last_insert_rowid();";
    DufSqlQuery query(q,db);
    query.first();
    q = "INSERT INTO Historique(debit) VALUES (" + query.value(0).toString() + ");";
    DufSqlQuery(q,db).verif();

    QDate date;
    if (mois->currentIndex() == 0) date = QDate(annee->value(),1,1);
    else date = QDate(annee->value(),mois->currentIndex(),1);

    Debit debit(true,query.value(0).toString(),date);
    if (!debit.exec())
    {
        QString q = "DELETE FROM Debits WHERE id = " + query.value(0).toString() + ";";
        DufSqlQuery(q,db);
        q = "DELETE FROM Historique WHERE debit = " + query.value(0).toString() + ";";
        DufSqlQuery(q,db);
    }
    else
    {
        refresh();
    }

}

void MainWindow::on_precedent_clicked()
{
    int index = mois->currentIndex();
    if (index == 0)
    {
        annee->setValue(annee->value()-1);
    }
    else if (index > 1)
    {
        mois->setCurrentIndex(index-1);
    }
    else if (index == 1)
    {
        annee->setValue(annee->value()-1);
        mois->setCurrentIndex(12);
    }

    refresh();
}

void MainWindow::on_suivant_clicked()
{
    int index = mois->currentIndex();
    if (index == 0)
    {
        annee->setValue(annee->value()+1);
    }
    else if (index < 12)
    {
        mois->setCurrentIndex(index+1);
    }
    else if (index == 12)
    {
        annee->setValue(annee->value()+1);
        mois->setCurrentIndex(1);
    }
    refresh();
}

void MainWindow::on_today_clicked()
{
    mois->setCurrentIndex(QDate::currentDate().month());
    annee->setValue(QDate::currentDate().year());
    refresh();
}

void MainWindow::on_recherche_textEdited(const QString &arg1)
{
    refresh();
}

void MainWindow::on_toolButton_clicked()
{
    recherche->setText("");
    refresh();
}

void MainWindow::on_actionClients_triggered()
{
    editeurClients->refresh();
    editeurClients->exec();
}

void MainWindow::on_actionAfficher_tout_triggered(bool checked)
{
    mois->setEnabled(!checked);
    annee->setEnabled(!checked);
    refresh();
}

void MainWindow::on_actionImporter_triggered()
{
    //TODO incorporer plutot que remplacer en option

    //demander où est le fichier à ouvrir
    QString fichier = QFileDialog::getOpenFileName(this,"Importation...", "", "Dumoney (*.dumo);;SQLite (*.s3db);;Tous (*.*)");
    if (fichier == "") return;

    //avertir qu'on perd toutes les données en cours, demander si enregistrement d'abord
    int reponse = QMessageBox::question(this,"Attention","Attention, en important de nouvelles données,\nvous aller perdre les données en cours.\nVoulez-vous d'abord enregistrer les données actuelles ?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,QMessageBox::Cancel);
    if (reponse == QMessageBox::Cancel) return;
    else if (reponse == QMessageBox::Yes) on_actionExporter_triggered();

    //importer : supprimer l'ancienne bdd, remplacer par la nouvelle
    if (!QFile(fichier).exists()) return;
    db.close();
    QFile("comptes.s3db").remove();
    QFile(fichier).copy("comptes.s3db");
    db.open();

    refresh();
}

void MainWindow::on_actionExporter_triggered()
{
    //demander où enregistrer
    QString fichier = QFileDialog::getSaveFileName(this,"Exportation...", "", "Dumoney (*.dumo);;SQLite (*.s3db)");
    if (fichier.trimmed() == "") return;
    //si le fichier existe, demander à l'écraser
    if (QFile(fichier).exists())
    {
        QFile(fichier).remove();
    }
    //faire la copie de la bdd dans le fichier
    QFile("comptes.s3db").copy(fichier);
}

void MainWindow::on_actionSupprimer_toutes_les_donn_es_triggered()
{
    //avertir qu'on perd toutes les données en cours, demander si enregistrement d'abord
    int reponse = QMessageBox::question(this,"Attention","Attention, vous allez supprimer toutes les données.\nVoulez-vous d'abord exporter les données actuelles ?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,QMessageBox::Cancel);
    if (reponse == QMessageBox::Cancel) return;
    else if (reponse == QMessageBox::Yes) on_actionExporter_triggered();

    //importer : supprimer l'ancienne bdd, remplacer par la nouvelle
    db.close();
    QFile("comptes.s3db").remove();
    QFile("new.s3db").copy("comptes.s3db");
    db.open();

    refresh();
}
