#ifndef DEVISEDITEUR_H
#define DEVISEDITEUR_H

#include "ui_devisediteur.h"

class DevisEditeur : public QMainWindow, private Ui::DevisEditeur
{
    Q_OBJECT
    
public:
    explicit DevisEditeur(QWidget *parent = 0);
};

#endif // DEVISEDITEUR_H
