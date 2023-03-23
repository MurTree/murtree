// MurTree DLL api

#include <string>

extern "C++" {
	int murtree(std::string path_to_dataset, unsigned int maxdepth, unsigned int maxnumnodes, unsigned int time);
}