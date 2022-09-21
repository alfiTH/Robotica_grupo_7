#include "ejemplo1.h"

Contenedores::Contenedores()
{
	//test_vector();
	//test_vector_omp();
	//test_list();
	//test_map();
	//test_tuple();
	test_graph();

}

void Contenedores::test_vector(){
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,1000000); // distribution in range [1, 6]
	qInfo() << "Empezamos prueba de vector";
	vec.resize(1000000000);
	auto t1 = std::chrono::high_resolution_clock::now();
	for(auto &n:vec)
		n=dist6(rng);
	auto t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	t1 = std::chrono::high_resolution_clock::now();
	std::ranges::sort(vec);
    t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Sort de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	
	t1 = std::chrono::high_resolution_clock::now();
	auto max = std::ranges::max(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Max de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<<"siendo"<<max;

	t1 = std::chrono::high_resolution_clock::now();
	auto min = std::ranges::min(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Min de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<< "siendo"<<min;

	t1 = std::chrono::high_resolution_clock::now();
	std::ranges::shuffle(vec, rng);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Shuffle de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

}


void Contenedores::test_vector_omp(){
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-1000000,1000000); // distribution in range [1, 6]
	qInfo() << "Empezamos prueba de vector con OMP";
	vec.resize(1000000000);

	auto t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for
		for(auto &n:vec)
			n=dist6(rng);
	
	auto t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel
		std::ranges::sort(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Sort de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	
	t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel
		auto max = std::ranges::max(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Max de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<<"siendo"<<max;

	t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel
		auto min = std::ranges::min(vec);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Min de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<< "siendo"<<min;

	t1 = std::chrono::high_resolution_clock::now();
	#pragma omp parallel
		std::ranges::shuffle(vec, rng);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Shuffle de vector" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
}



void Contenedores::test_list()
{
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-100,100); // distribution in range [1, 6]
	qInfo() << "Empezamos prueba lista";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 25; i++)
		lis.push_front(dist6(rng));
	auto t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de lista" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	
	for (auto &n:lis){
		qInfo()<<n;
	}

	t1 = std::chrono::high_resolution_clock::now();
	lis.sort();
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Sort de lista" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	qInfo()<<lis;

	t1 = std::chrono::high_resolution_clock::now();
	auto min = std::ranges::min(lis);
	t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Min de lista" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<< "siendo"<<min;
}

void Contenedores::test_map()
{
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-100,100); // distribution in range [1, 6]
	qInfo() << "Empezamos prueba mapa";
	auto t1 = std::chrono::high_resolution_clock::now();
	std::map<std::pair<int,int> , int>::iterator it = mymap.begin();
	for (int i = 0; i < 25; i++)
	{
		mymap.insert ({ std::make_pair(dist6(rng),dist6(rng)), i });
	}
	
	auto t2 = std::chrono::high_resolution_clock::now();
	qInfo() << "Inicializacion de lista" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	// showing contents:

	for (it=mymap.begin(); it!=mymap.end(); ++it)
    	qInfo() << it->first << " => " << it->second;
	

}


void Contenedores::test_tuple()
{
	mytuple = std::make_tuple(10, "posicion", 9.0, 2.5);

	const auto& [a, b, c, d] = mytuple;
	std::cout << "The value of t is "  << "("<< a << ", " << b << ", "<< c << ", " << d <<")\n";
	std::cout << "The value of t is "  << "("<< std::get<0>(mytuple) << ", " << std::get<1>(mytuple) << ", "<< std::get<2>(mytuple) << ", " << std::get<3>(mytuple)<<")\n";
}

void Contenedores::test_graph()
{
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,24); // distribution in range [1, 6]
	uint number;
	for (uint i = 0; i < 25; i++)
	{
		Contenedores::Node nodo;
		nodo.id = i;
		auto inter = dist6(rng);
		for (uint x = 0; x < inter; x++)
		{
			do
			{
			number = dist6(rng);
			}while(std::find(nodo.links.begin(), nodo.links.end(), number) != nodo.links.end());
			nodo.links.push_back(number);
		}
		std::ranges::sort(nodo.links);
		graph.insert({i, nodo});
	}
	for(auto &g:graph)
	{
		qInfo()<<"Id del nodo"<<g.first<<"interior del nodo:";
		qInfo()<<"ID:"<<g.second.id;
		qInfo()<<"Enlaces:";
		for (auto &link:g.second.links)
			std::cout<<link<<" || ";
		std::cout<<"\n";
	}
	
	
}