#include "ejemplo1.h"

Contenedores::Contenedores()
{
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,1000000); // distribution in range [1, 6]
	vec.resize(1000000000);
	auto t1 = std::chrono::high_resolution_clock::now();
	for(auto &n:vec)
	{
		n=dist6(rng);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();


	vec2.resize(1000000000);
	t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for
	for(auto &n:vec2)
	{
		n=dist6(rng);
	}
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de vector OMP" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	t1 = std::chrono::high_resolution_clock::now();
	std::ranges::sort(vec);
    t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Sort de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	
	t1 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel 
	std::ranges::sort(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Sort OMP de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();


}

void Contenedores::sort_vector(){
	;
}



