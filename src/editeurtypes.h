#ifndef EDITEURTYPES_H
#define EDITEURTYPES_H

#include "ui_editeurtypes.h"
#include <QSqlDatabase>

class EditeurTypes : public QDialog, private Ui::EditeurTypes
{
    Q_OBJECT
    
public:
    explicit EditeurTypes(QWidget *parent = 0);
    void refresh();

private slots:
    void on_liste_itemClicked(QListWidgetItem *item);
    void on_ajouter_clicked();
    void on_modifier_clicked();
    void on_supprimer_clicked();
    void on_immediat_toggled(bool checked);

private:
    QSqlDatabase db;
    void reInit();
    void displayItem(QString name);
};

#endif // EDITEURTYPES_H
