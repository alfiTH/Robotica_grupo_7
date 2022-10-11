/*
 *    Copyright (C) 2022 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
	this->startup_check_flag = startup_check;
	// Uncomment if there's too many debug messages
	// but it removes the possibility to see the messages
	// shown in the console with qDebug()
//	QLoggingCategory::setFilterRules("*.debug=false\n");
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
    this->differentialrobotmulti_proxy->setSpeedBase(2, 0, 0);

}

// Driver function to sort the vector elements
// by second element of pairs
bool sortbysec(const auto &a,
               const auto &b)
{
    return (a.second < b.second);
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
    UMBRAL=1000;

//	THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = std::make_shared(innermodel_path);
//	}
//	catch(const std::exception &e) { qFatal("Error reading config params"); }

	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;
	this->Period = period;

    velGiroOld = 0;
    velAdvOld = 0;
    velGiro = 0;
    velAdv = 0;
    std::mt19937 rngAux(dev());
    rng  = std::move(rngAux);
    std::uniform_int_distribution<std::mt19937::result_type> distAux(0,M_PI*2); // distribution in range [1, 6]
    dist6  = std::move(distAux);


	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		timer.start(Period);
	}

}

void SpecificWorker::go_straight()
{
    std::cout << "Go straight" << std::endl;
    const auto ldata = lasermulti_proxy->getLaserData(2);

    RoboCompLaserMulti::TLaserData right;
    RoboCompLaserMulti::TLaserData left;
    RoboCompLaserMulti::TLaserData laser;
    RoboCompLaserMulti::TLaserData velLaser;


    velLaser.assign(ldata.begin()+ldata.size()/3, ldata.end()-ldata.size()/3);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    qInfo() << "Velocidad de giro" << velLaser.front().dist;
    //VELOCIDAD POR FUNCION SIGMOIDE
    velAdv = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/435)))+1) * 1250;

    //FALTAN LAS OCLUSION DE ESQUINAS, SI ES IGUAL SECCIONDE VECTOR LEF Y RIGH
    if (abs(velGiro) < 0.1){
        velGiro = abs(velGiro)/velGiro * 1.2;
    }
    else{

        //Comprobar que estén bien los intervalos
        right.assign(ldata.begin(), ldata.begin()+ldata.size()/2);
        left.assign(ldata.begin()+ldata.size()/2, ldata.end());

        auto maxAng = *max_element(right.begin(), right.end());
        auto minAng = *min_element(left.begin(), left.end());
        for (auto &n:right)
        {
            n.angle = 1 - (n.angle / maxAng.angle);
        }
        for (auto &n:left)
        {
            n.angle = ((n.angle - minAng.angle)/ minAng.angle);
        }
        laser.assign(right.begin(), right.end());
        laser.insert(laser.end(), left.begin(), left.end());
        velGiro= 0;
        for (auto &n:laser)
        {
            velGiro += n.angle * abs(n.angle)  * -20/n.dist;
        }
    }



    qInfo() << "Velocidad de giro" << velGiro;



}
void SpecificWorker::follow_wall()
{

}
void SpecificWorker::spiral()
{

}
void SpecificWorker::balance()
{

}


void SpecificWorker::compute()
{
    // el robot siente
    try
    {
        go_straight();

        if (velGiro != velGiroOld or velAdv != velAdvOld)
        {
            this->differentialrobotmulti_proxy->setSpeedBase(2, velAdv, velGiro);
            qInfo() <<"Giro de "<<velGiro<<" Lineal de "<<velAdv;
            velGiroOld = velGiro;
            velAdvOld = velAdv;
        }
    }
    catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
}

int SpecificWorker::startup_check()
{
	std::cout << "Startup check" << std::endl;
	QTimer::singleShot(200, qApp, SLOT(quit()));
	return 0;
}






/**************************************/
// From the RoboCompDifferentialRobotMulti you can call this methods:
// this->differentialrobotmulti_proxy->getBaseState(...)
// this->differentialrobotmulti_proxy->setSpeedBase(...)

/**************************************/
// From the RoboCompDifferentialRobotMulti you can use this types:
// RoboCompDifferentialRobotMulti::TMechParams

/**************************************/
// From the RoboCompLaserMulti you can call this methods:
// this->lasermulti_proxy->getLaserConfData(...)
// this->lasermulti_proxy->getLaserData(...)

/**************************************/
// From the RoboCompLaserMulti you can use this types:
// RoboCompLaserMulti::LaserConfData
// RoboCompLaserMulti::TData

