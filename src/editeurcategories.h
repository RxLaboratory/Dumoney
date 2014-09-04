#ifndef EDITEURCATEGORIES_H
#define EDITEURCATEGORIES_H

#include "ui_editeurcategories.h"
#include <QSqlDatabase>

class EditeurCategories : public QDialog, private Ui::EditeurCategories
{
    Q_OBJECT
    
public:
    explicit EditeurCategories(QWidget *parent = 0);
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

#endif // EDITEURCATEGORIES_H
