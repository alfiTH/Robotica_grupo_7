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

void SpecificWorker::compute()
{
    // el robot siente
    try
    {
        const auto ldata = laser_proxy->getLaserData();
        const int part = 3;

        RoboCompLaser::TLaserData copy;
        copy.assign(ldata.begin()+ldata.size()/part, ldata.end()-ldata.size()/part);
        std::ranges::sort(copy, {}, &RoboCompLaser::TData::dist);

        if (velGiro == 0)
            velGiro= dist6(rng) - M_PI; //velocidad de giro aleatoria

        if(copy.front().dist < UMBRAL)
        {
            velAdv = 0;
        }
        else if(copy.front().dist < UMBRAL*1.5)
        {
            velAdv = 500;
        }
        else
        {
            velAdv = 1000;
            velGiro = 0;
        }

        if (velGiro != velGiroOld or velAdv != velAdvOld)
        {
            qInfo() << copy.front().dist;
            this->differentialrobot_proxy->setSpeedBase(velAdv, velGiro);
            qInfo() <<"Giro de"<<velGiro<<"Lineal de"<<velAdv;
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
// From the RoboCompDifferentialRobot you can call this methods:
// this->differentialrobot_proxy->correctOdometer(...)
// this->differentialrobot_proxy->getBasePose(...)
// this->differentialrobot_proxy->getBaseState(...)
// this->differentialrobot_proxy->resetOdometer(...)
// this->differentialrobot_proxy->setOdometer(...)
// this->differentialrobot_proxy->setOdometerPose(...)
// this->differentialrobot_proxy->setSpeedBase(...)
// this->differentialrobot_proxy->stopBase(...)

/**************************************/
// From the RoboCompDifferentialRobot you can use this types:
// RoboCompDifferentialRobot::TMechParams

/**************************************/
// From the RoboCompLaser you can call this methods:
// this->laser_proxy->getLaserAndBStateData(...)
// this->laser_proxy->getLaserConfData(...)
// this->laser_proxy->getLaserData(...)

/**************************************/
// From the RoboCompLaser you can use this types:
// RoboCompLaser::LaserConfData
// RoboCompLaser::TData

