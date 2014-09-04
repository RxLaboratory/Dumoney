#ifndef EDITEURCLIENTS_H
#define EDITEURCLIENTS_H

#include "ui_editeurclients.h"
#include <QSqlDatabase>

class EditeurClients : public QDialog, private Ui::EditeurClients
{
    Q_OBJECT
    
public:
    explicit EditeurClients(QWidget *parent = 0);
    void refresh();

private slots:
    void on_liste_itemClicked(QListWidgetItem *item);
    void on_ajouter_clicked();
    void on_modifier_clicked();
    void on_supprimer_clicked();

private:
    QSqlDatabase db;
    void reInit();
    void displayItem(QString name);
};

#endif // EDITEURCLIENTS_H
