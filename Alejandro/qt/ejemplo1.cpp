#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	tUpdate = new QTimer(this);
	tCounter = new QTimer(this);
	counter = 0;
	setupUi(this);
	show();
	connect(Increment, SIGNAL(clicked()), this, SLOT(increment()) );
	connect(Decrement, SIGNAL(clicked()), this, SLOT(decrement()) );
	connect(StartStop, SIGNAL(clicked()), this, SLOT(run_timer()) );
	connect(Reset, SIGNAL(clicked()), this, SLOT(reset()) );
	//connect(tUpdate, SIGNAL(timeout()), this, SLOT(timer_loop()));
	connect(tCounter, SIGNAL(timeout()), this, SLOT(timer_counter()));
	connect(sPeriod, SIGNAL(valueChanged(int)), this, SLOT(slicer_period()));
	connect(sCounter, SIGNAL(valueChanged(int)), this, SLOT(slicer_counter()));
	//tUpdate->start(10);
	std::thread th(&ejemplo1::timer_thread, this);
	threadUpdate  = std::move(th);
	//th.swap(threadUpdate);
}

void ejemplo1::timer_thread()
{
	while (true)
	{
		lcdNumber->display(counter);
std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
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
	if (tCounter->isActive())
		tCounter->start(sPeriod-> sliderPosition());
}

void ejemplo1::slicer_counter(){
	lcdCounter->display(sCounter-> sliderPosition());
}

void ejemplo1::run_timer()
{
	qInfo()<<tCounter->isActive();
	if (tCounter->isActive()){
		tCounter->stop();
		StartStop->setText("Start");
		widgetManual->show();
	}
		
	else{
		tCounter->start(sPeriod->sliderPosition());
		StartStop->setText("Stop");
		widgetManual->hide();
	}
}

void ejemplo1::decrement()
{
	counter--;
}

void ejemplo1::increment()
{
	counter++;
}

void ejemplo1::reset()
{
	counter = 0;
}




