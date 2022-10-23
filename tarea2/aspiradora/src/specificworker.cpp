/*
 *    Copyright (C) 2022 by Alejandro Torrejón Harto and María Segovia Ferreira
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

/**
	\brief
	@author Alejandro Torrejón Harto and María Segovia Ferreira
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
    this->differentialrobotmulti_proxy->setSpeedBase(idGiraff, 0, 0);

}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
    try
	{
        UMBRAL=std::stoi(params.at("UMBRAL").value);
        k = std::stof(params.at("k").value);
        UMBRAL_REPULSION = std::stoi(params.at("UMBRAL_REPULSION").value);
        idGiraff = std::stoi(params.at("id_giraff").value);
	}
	catch(const std::exception &e) { qFatal("Error reading config params"); }

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
    contador = 0;

	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		timer.start(Period);
	}

}
//    repulsion_data.first --> Velocidad de avance
//    repulsion_data.second --> Velocidad de giro
std::pair<float,float> SpecificWorker::repulsion()
{
    const auto ldata = lasermulti_proxy->getLaserData(idGiraff);

    RoboCompLaserMulti::TLaserData right;
    RoboCompLaserMulti::TLaserData left;
    RoboCompLaserMulti::TLaserData laser;
    RoboCompLaserMulti::TLaserData velLaser;
    std::pair<float,float> repulsion_data;

    velLaser.assign(ldata.begin()+ldata.size()/3, ldata.end()-ldata.size()/3);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    
    //VELOCIDAD POR FUNCION SIGMOIDE
    repulsion_data.first = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/200)))+1) * 1500;
   
    //Comprobar que estén bien los intervalos
    right.assign(ldata.begin(), ldata.begin()+ldata.size()/2);
    left.assign(ldata.begin()+ldata.size()/2, ldata.end());

    //NORMALIZAMOS ANGULOS
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
    
    //UNIMOS LOS ANGULOS
    laser.assign(right.begin(), right.end());
    laser.insert(laser.end(), left.begin(), left.end());
    repulsion_data.second = 0;
    
    
    for (auto &n:laser)
    {
        //APLICAMOS REBOSE DE LA DISTANCIA
        if(n.dist > UMBRAL_REPULSION)
        {
            n.dist = UMBRAL_REPULSION;
        }
        //REALIZAMOS FORMULA
        repulsion_data.second += n.angle * abs(n.angle)  * -20/n.dist;
    }

    //anti bloqueo de esquinas
    if (repulsion_data.first < 200 and repulsion_data.first > -200 and repulsion_data.second <0.05 and repulsion_data.second > -0.05){
        repulsion_data.second = abs(repulsion_data.second)/repulsion_data.second * 1.2;
        try{
            this->differentialrobotmulti_proxy->setSpeedBase(idGiraff, -100, repulsion_data.second);
            sleep(3);
        }
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
    }
    //qInfo() << "repulsion: " << repulsion_data.first << " " << repulsion_data.second;
    return repulsion_data;
}

void SpecificWorker::compute()
{
    //El robot siente,luego piensa, luego actua, luego existe
    try
    {
        std::pair<float, float> vel;
        vel = repulsion();
        if(contador < 500)
        { 
            velAdv = vel.first;
            velGiro = vel.second * k;
            contador++;
        }
        else
        {
            if (k > 0.8)
                k = 0.65;
            else
                k+=0.05;
            contador = 0;
        }

        if (velGiro != velGiroOld or velAdv != velAdvOld)
        {
            this->differentialrobotmulti_proxy->setSpeedBase(idGiraff, velAdv, velGiro);
            qInfo() <<" Lineal de "<<velAdv<<"Giro de "<<velGiro;
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

