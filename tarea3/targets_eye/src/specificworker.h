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

#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <innermodel/innermodel.h>
#include <ranges>

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(TuplePrx tprx, bool startup_check);
	~SpecificWorker();
    //Función de repulsion
    std::pair<float,float> repulsion();
	bool setParams(RoboCompCommonBehavior::ParameterList params);

public slots:
	void compute();
	int startup_check();
	void initialize(int period);
    void timer_counter();
    
private:
	std::shared_ptr < InnerModel > innerModel;
	bool startup_check_flag;

    //couter de aleatoridad
    QTimer *tCounter;

    //velocidades anteriores
    float velGiroOld;
    float velAdvOld;

    //Parametros del config
    int UMBRAL;
    int UMBRAL_REPULSION;
    int idGiraff;
    float k;
};

#endif
