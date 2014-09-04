#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include "editeurtypes.h"
#include "editeurcategories.h"
#include "editeurclients.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    
private slots:
    void dateUp();
    void on_actionA_Propos_de_Qt_triggered();
    void on_actionA_Propos_triggered();
    void on_actionQuitter_triggered();
    void on_actionTypes_triggered();
    void on_actionCat_gories_triggered();
    void on_actionAjouter_un_cr_dit_triggered();
    void on_annee_valueChanged(int arg1);
    void on_mois_activated(int index);
    void on_actionSupprimer_triggered();
    void on_actionModifier_triggered();
    void on_table_cellDoubleClicked(int row, int column);
    void on_actionAjouter_un_d_bit_triggered();
    void on_precedent_clicked();
    void on_suivant_clicked();
    void on_today_clicked();
    void on_recherche_textEdited(const QString &arg1);
    void on_toolButton_clicked();
    void on_actionClients_triggered();
    void on_actionAfficher_tout_triggered(bool checked);

    void on_actionImporter_triggered();

    void on_actionExporter_triggered();

    void on_actionSupprimer_toutes_les_donn_es_triggered();

private:
    QSqlDatabase db;
    EditeurTypes *editeurTypes;
    EditeurCategories *editeurCategories;
    EditeurClients *editeurClients;
    QColor couleurPositif;
    QColor couleurNegatif;
    QColor couleurFutur;
    void refresh();

};

#endif // MAINWINDOW_H
