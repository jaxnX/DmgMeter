#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QDialog>

namespace Ui
{
    class Configurator;
}

namespace GW2
{
    class Configurator : public QDialog
    {
        Q_OBJECT

    public:
        explicit Configurator(QWidget *parent = 0);
        ~Configurator();

    private slots:
        void RestoreDefaults();

    private:
        Ui::Configurator *ui;

        friend class MainWindow;
    };
}

#endif // CONFIGURATOR_H
