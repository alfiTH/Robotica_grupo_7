#ifndef ejemplo1_H
#define ejemplo1_H

#include <QtGui>
#include <QTimer>
#include "ui_counterDlg.h"

class QTimer;

class ejemplo1 : public QWidget, public Ui_Counter
{
    private:

        int counter;
        QTimer *tUpdate; 
        QTimer *tCounter;
        bool runTimer;
    Q_OBJECT
    public:
        ejemplo1();

    public slots:
        void timer_loop();
        void timer_counter();
        void slicer_period();
        void slicer_counter();
        void run_timer();
        void decrement();
        void increment();
};

#endif // ejemplo1_H
