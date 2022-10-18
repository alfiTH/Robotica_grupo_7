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
    UMBRAL=750;
    UMBRAL_WALL=250;

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
    initSpiral = false;
    mode = 5;
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

std::pair<float,float> SpecificWorker::repulsion()
{
    const auto ldata = lasermulti_proxy->getLaserData(2);

    RoboCompLaserMulti::TLaserData right;
    RoboCompLaserMulti::TLaserData left;
    RoboCompLaserMulti::TLaserData laser;
    RoboCompLaserMulti::TLaserData velLaser;
    std::pair<float,float> repulsion_data;


    velLaser.assign(ldata.begin()+ldata.size()/3, ldata.end()-ldata.size()/3);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    
    //VELOCIDAD POR FUNCION SIGMOIDE
    repulsion_data.first = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/500)))+1) * 1250;

    //FALTAN LAS OCLUSION DE ESQUINAS, SI ES IGUAL SECCIONDE VECTOR LEFT Y RIGHT
    
    
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
    repulsion_data.second= 0;
    //qInfo() << "max/min: " << maxAng.angle << " " << minAng.angle;
    for (auto &n:laser)
    {
        repulsion_data.second += n.angle * abs(n.angle)  * -20/n.dist;
    }
    if (repulsion_data.first < 200 and repulsion_data.first > -200 and repulsion_data.second <0.05 and repulsion_data.second > -0.05){
        repulsion_data.second = abs(repulsion_data.second)/repulsion_data.second * 1.2;
        try{
            this->differentialrobotmulti_proxy->setSpeedBase(2, -250, repulsion_data.second);
            sleep(3);
        }
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
    }
    qInfo() << "repulsion: " << repulsion_data.first << " " << repulsion_data.second;
    return repulsion_data;
}

int SpecificWorker::go_straight()
{
    std::cout << "Go straight" << std::endl;
    const auto ldata = lasermulti_proxy->getLaserData(2);

    RoboCompLaserMulti::TLaserData right;
    RoboCompLaserMulti::TLaserData left;
    RoboCompLaserMulti::TLaserData laser;
    RoboCompLaserMulti::TLaserData velLaser;


    velLaser.assign(ldata.begin()+ldata.size()/3, ldata.end()-ldata.size()/3);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    
    //VELOCIDAD POR FUNCION SIGMOIDE
    velAdv = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/400)))+1) * 1250;

    //FALTAN LAS OCLUSION DE ESQUINAS, SI ES IGUAL SECCIONDE VECTOR LEF Y RIGH
    
    

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
    qInfo() << "max/min: " << maxAng.angle << " " << minAng.angle;
    for (auto &n:laser)
    {
        velGiro += n.angle * abs(n.angle)  * -20/n.dist;
    }
    if (velAdv < 100 and velAdv > -100 and velGiro <0.05 and velGiro > -0.05){
        //velGiro = abs(velGiro)/velGiro * 1.2;
        try{
            this->differentialrobotmulti_proxy->setSpeedBase(2, -1250, -1.25);
            sleep(3);
        }
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
    }
    
    return 1;
}


int SpecificWorker::follow_wall()
{

    std::cout << "follow wall" << std::endl;

    //dividiremos el LiDar en 6 sectores usando los laterales para el equilibrio del la pared
    RoboCompLaserMulti::TLaserData sideUp;
    RoboCompLaserMulti::TLaserData sideDown;    
    RoboCompLaserMulti::TLaserData frontRight;
    RoboCompLaserMulti::TLaserData frontLeft;
    RoboCompLaserMulti::TData right;
    RoboCompLaserMulti::TLaserData velLaser;
    const auto ldata = lasermulti_proxy->getLaserData(2);
    const int part = ldata.size()/6;
    //direction = 0 --> izquierda
    //direction = 1 --> derecha

    velLaser.assign(ldata.begin() + 3 * part -part/ 2, ldata.begin() + 3 * part + part/ 2);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);

    //VELOCIDAD POR FUNCION SIGMOIDE
    double sinAdv = (-2/(1+ exp((velLaser.front().dist-UMBRAL)/435)))+1;
    velAdv = sinAdv * 800;
    
    frontRight.assign(ldata.begin() + part * 2, ldata.begin()+ part * 3);
    frontLeft.assign(ldata.begin() + part * 3, ldata.begin() + part * 4);


     //   sideUp.assign(ldata.begin() , ldata.begin()+ part );
     //   sideDown.assign(ldata.begin() + part, ldata.begin() + part * 2);
        right = ldata.at(106);
    /*
    difWall = 0;
    for ( int i = 0; i < sideUp.size();i++)
    {
        difWall += sideUp[i].dist - sideDown[i].dist;
    }*/
    if( velAdv< 5 and velAdv > -5)
    {
        try{
            this->differentialrobotmulti_proxy->setSpeedBase(2, velAdv, -1.25);
            sleep(3);
        }
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
        
    }
    else{
        velGiro =((right.dist - UMBRAL)/1000) * 1.25 ;
        if (velGiro > 3.5)
            return 1;
        if (right.dist < UMBRAL + 100)
        {
            qInfo() << "REDUCCION";
            velGiro = velGiro * (1-sinAdv);
        }
        qInfo() << "dist" << right.dist ;
        if(velGiro < -1.25)
            velGiro = -1.25;
        else if(velGiro > 1.25)
            velGiro = 1.25;
        return 2;
    }

    //velGiro = ((-2/(1+ exp((right.dist - UMBRAL)/500)))+1) * 1.25;
    //563 (izquierda) y 106 (derecha)
}
int SpecificWorker::spiral()
{
    std::cout << "spiral" << std::endl;
    if (initSpiral ==false)
    {
        initSpiral = true;
        velAdv = 0;
        velGiro = 1.25;
    }

    if (velAdv < 1250)
        velAdv +=5;
    if (velGiro > 0)
        velGiro -=0.0005;
    return 3;
}

std::pair<float,float> SpecificWorker::spiral_wall()
{
        std::pair<float,float> repulsion_data;
        repulsion_data.first = 0.0;
        repulsion_data.second = 0.0;
    
    //dividiremos el LiDar en 6 sectores usando los laterales para el equilibrio del la pared
    //RoboCompLaserMulti::TLaserData sideUp;
    //RoboCompLaserMulti::TLaserData sideDown;

    RoboCompLaserMulti::TLaserData frontRight;
    RoboCompLaserMulti::TLaserData frontLeft;
    RoboCompLaserMulti::TData right;
    RoboCompLaserMulti::TLaserData velLaser;
    const auto ldata = lasermulti_proxy->getLaserData(2);
    const int part = ldata.size()/6;
    int direction;
    //direction = 0 --> izquierda
    //direction = 1 --> derecha
    if (ldata.at(106).dist < ldata.at(563).dist)
    {
        right =ldata.at(106);
        direction = 1;
    }
    else
    {
        right =ldata.at(563);
        direction = -1;
    }

    //Obtenemos la secion frontal 
    velLaser.assign(ldata.begin() + 3 * part -part/ 2, ldata.begin() + 3 * part + part/ 2);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    if(velLaser.front().dist-UMBRAL < 150)
    {
        if(UMBRAL_WALL > 3500)
        {
            UMBRAL_WALL = 600;
        }
        else
        {
            qInfo() << "Aumento umbral" << UMBRAL_WALL;
            UMBRAL_WALL += 10;
        }

    }

    //VELOCIDAD POR FUNCION GAUSSIANA POR EL ERROR DE DESVIACION DEL SEGUIMIENTO
    //y = e^-(x²/(2*k)²)
    double sinAdv = exp(-(pow(right.dist-UMBRAL_WALL,2)/10000000));
    //double sinAdv = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/500)))+1);
    repulsion_data.first = sinAdv * 1250;
    
    //frontRight.assign(ldata.begin() + part * 2, ldata.begin()+ part * 3);
    //frontLeft.assign(ldata.begin() + part * 3, ldata.begin() + part * 4);


     //   sideUp.assign(ldata.begin() , ldata.begin()+ part );
     //   sideDown.assign(ldata.begin() + part, ldata.begin() + part * 2);

    //difWall = 0;
    //for ( int i = 0; i < sideUp.size();i++)
    //{
    //    difWall += sideUp[i].dist - sideDown[i].dist;
    //}
   /*  if( repulsion_data.first < 5 and repulsion_data.first > -5)
    {
        try{
            this->differentialrobotmulti_proxy->setSpeedBase(2, repulsion_data.first, -1.25);
            sleep(2);
            UMBRAL_WALL += 50;
            qInfo() <<"umbral"<<UMBRAL_WALL;
            return repulsion_data;
        }
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
        
    }
     */
    //else{
        repulsion_data.second = direction * ((right.dist - UMBRAL_WALL)/1000) * 1.25 ;
        //if (velGiro > 3.5)
        //   return 4;
        if (right.dist < UMBRAL_WALL + 100)
        {
            repulsion_data.second = repulsion_data.second * (1-sinAdv);
        }
        qInfo() << "dist" << right.dist ;
        if(repulsion_data.second < -1.25)
            repulsion_data.second = -1.25;
        else if(repulsion_data.second > 1.25)
            repulsion_data.second = 1.25;
    qInfo() << "Umbral" << UMBRAL_WALL;
        qInfo() << "espiral: " << repulsion_data.first << " " << repulsion_data.second;
        return repulsion_data;
    //}

    //velGiro = ((-2/(1+ exp((right.dist - UMBRAL)/500)))+1) * 1.25;
    //563 (izquierda) y 106 (derecha)   

}


void SpecificWorker::compute()
{
    // el robot siente
    try
    {
        switch (mode)
        {
        //Go_straight
        case 1:
            mode = go_straight();
            break;
        //follow_wall
        case 2:
            mode = follow_wall();
            break;
        //Spiral
        case 3:
            mode = spiral();
            break;
        case 4:
        {
            std::pair<float,float> vel;
            vel = spiral_wall();
            velAdv = vel.first;
            velGiro = vel.second;
            break;
        }
        case 5:
        {
            std::pair<float,float> vel;
            vel = repulsion();
            velAdv = vel.first * 0.75;
            velGiro = vel.second * 0.8;
            vel = spiral_wall();
            velAdv += vel.first * 0.25;
            velGiro += vel.second * 0.2;
            break;
        }
        default:
            break;
        }



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

