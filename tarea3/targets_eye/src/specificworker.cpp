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
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
    try
    {
        this->differentialrobotmulti_proxy->setSpeedBase(idGiraff, 0, 0);
    }
    catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; };
    
    

}

/**
 * @brief Obtienes los parámetros del config
 * 
 * @param params 
 * @return true 
 * @return false 
 */
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

/**
 * @brief Arranca el hilo compute y establece los valores iniciales a las variables
 * 
 * @param period 
 */
void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;
	this->Period = period;
    velGiroOld = 0;
    velAdvOld = 0;
    //timer que incrementará una variable para aplicar aleatoriedad al barrido
    tCounter = new QTimer(this);
	connect(tCounter, SIGNAL(timeout()), this, SLOT(timer_counter()));
    tCounter->start(5000);

	if(this->startup_check_flag)
		this->startup_check();
	else
		timer.start(Period);

}

/**
 * @brief Funcion para incremetar la variable de aleatoridad
 * 
 */
void SpecificWorker::timer_counter()
{
    qInfo() << "Modificación K, valor actual: " << this->k;
    if (this->k > 0.8)
        this->k = 0.65;
    else
        this->k+=0.05;
}

/**
 * @brief Devuelva la velocidad de avance y de giro para aplicar una repulsion al entorno
 * 
 * @return std::pair<float,float>  repulsion_data.first --> Velocidad de avance, repulsion_data.second --> Velocidad de giro
 */
std::pair<float,float> SpecificWorker::repulsion()
{
    RoboCompLaserMulti::TLaserData right;
    RoboCompLaserMulti::TLaserData left;
    RoboCompLaserMulti::TLaserData laser;
    RoboCompLaserMulti::TLaserData velLaser;
    RoboCompLaserMulti::TLaserData ldata;
    std::pair<float,float> repulsion_data;
    
    //Obtenemos LiDAR
    try
    {
        ldata = lasermulti_proxy->getLaserData(idGiraff);
    }
    catch (const Ice::Exception &e) 
    { 
        std::cout << e.what() << std::endl; 
        repulsion_data.first = 0;
        repulsion_data.second = 0;
        return repulsion_data;
    };
    

    //obtenemos un tercio del laser en sentido al frontal del giraff
    velLaser.assign(ldata.begin()+ldata.size()/3, ldata.end()-ldata.size()/3);
    std::ranges::sort(velLaser, {}, &RoboCompLaserMulti::TData::dist);
    
    //VELOCIDAD POR FUNCIÓN SIGMOIDE 
    //const = 200 rápido pero con colisiones si el objeto esta en movimiento
    //const = 430 menos rápido pero sin colisiones
    repulsion_data.first = ((-2/(1+ exp((velLaser.front().dist-UMBRAL)/200)))+1) * 1500;
    
    //Comprobar que estén bien los intervalos
    right.assign(ldata.begin(), ldata.begin()+ldata.size()/2);
    left.assign(ldata.begin()+ldata.size()/2, ldata.end());

    //NORMALIZAMOS ÁNGULOS
    auto maxAng = *max_element(right.begin(), right.end());
    auto minAng = *min_element(left.begin(), left.end());
    
    for (auto &n:right)
        n.angle = 1 - (n.angle / maxAng.angle);

    for (auto &n:left)
        n.angle = ((n.angle - minAng.angle)/ minAng.angle);
    
    //UNIMOS LOS ÁNGULOS
    laser.assign(right.begin(), right.end());
    laser.insert(laser.end(), left.begin(), left.end());
    repulsion_data.second = 0;
    
    
    for (auto &n:laser)
    {
        //APLICAMOS REBOSE DE LA DISTANCIA, ASI CERRAMOS LA INTERACCIÓN CON OBJETOS LEJANOS
        if(n.dist > UMBRAL_REPULSION)
            n.dist = UMBRAL_REPULSION;
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
        //Obtenemos las velocidades
        vel = repulsion();
        float velAdv = vel.first;
        float velGiro = vel.second * k;

        //Para no saturar enviamos en caso de ser diferentes
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
// From the RoboCompCameraRGBDSimple you can call this methods:
// this->camerargbdsimple_proxy->getAll(...)
// this->camerargbdsimple_proxy->getDepth(...)
// this->camerargbdsimple_proxy->getImage(...)
// this->camerargbdsimple_proxy->getPoints(...)

/**************************************/
// From the RoboCompCameraRGBDSimple you can use this types:
// RoboCompCameraRGBDSimple::Point3D
// RoboCompCameraRGBDSimple::TPoints
// RoboCompCameraRGBDSimple::TImage
// RoboCompCameraRGBDSimple::TDepth
// RoboCompCameraRGBDSimple::TRGBD

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

/**************************************/
// From the RoboCompYoloObjects you can call this methods:
// this->yoloobjects_proxy->getImage(...)
// this->yoloobjects_proxy->getYoloJointData(...)
// this->yoloobjects_proxy->getYoloObjectNames(...)
// this->yoloobjects_proxy->getYoloObjects(...)

/**************************************/
// From the RoboCompYoloObjects you can use this types:
// RoboCompYoloObjects::TBox
// RoboCompYoloObjects::TKeyPoint
// RoboCompYoloObjects::TPerson
// RoboCompYoloObjects::TConnection
// RoboCompYoloObjects::TJointData
// RoboCompYoloObjects::TData

