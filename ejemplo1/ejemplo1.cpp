#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	tUpdate = new QTimer(this);
	tCounter = new QTimer(this);
	counter = 0;
	runTimer = false;
	setupUi(this);
	show();
	connect(Increment, SIGNAL(clicked()), this, SLOT(increment()) );
	connect(Decrement, SIGNAL(clicked()), this, SLOT(decrement()) );
	connect(StartStop, SIGNAL(clicked()), this, SLOT(run_timer()) );
	connect(tUpdate, SIGNAL(timeout()), this, SLOT(timer_loop()));
	connect(tCounter, SIGNAL(timeout()), this, SLOT(timer_counter()));
	connect(sPeriod, SIGNAL(valueChanged(int)), this, SLOT(slicer_period()));
	connect(sCounter, SIGNAL(valueChanged(int)), this, SLOT(slicer_counter()));
	tUpdate->start(10);
}

void ejemplo1::timer_loop()
{
	lcdNumber->display(counter);
}

void ejemplo1::timer_counter()
{
	counter += sCounter->value();
}

void ejemplo1::slicer_period(){
	lcdPeriod->display(sPeriod-> sliderPosition());
}

void ejemplo1::slicer_counter(){
	lcdCounter->display(sCounter-> sliderPosition());
}

void ejemplo1::run_timer()
{
	if (runTimer){
		tCounter->stop();
		StartStop->setText("Start");
		widgetManual->show();
	}
		
	else{
		tCounter->start(sPeriod->value());
		StartStop->setText("Stop");
		widgetManual->hide();
	}
	
	runTimer = !runTimer;
	qInfo()<<"Timer a "<<runTimer;
}

void ejemplo1::decrement()
{
	counter--;
}

void ejemplo1::increment()
{
	counter++;
}




