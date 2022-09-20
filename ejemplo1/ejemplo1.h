#ifndef ejemplo1_H
#define ejemplo1_H

#include <QtGui>
#include <QTimer>
#include "ui_counterDlg.h"
#include <thread>   

class QTimer;

class ejemplo1 : public QWidget, public Ui_Counter
{
    private:

        int counter;
        QTimer *tUpdate; 
        QTimer *tCounter;
        std::thread threadUpdate;

    public:
        void timer_thread();

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
        void reset();
};

#endif // ejemplo1_H
