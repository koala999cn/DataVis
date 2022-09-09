#include "KvAxisTicker.h"


KvAxisTicker::KvAxisTicker()
{
	tickCount_ = 5;
}


KvAxisTicker::~KvAxisTicker()
{

}


void KvAxisTicker::setTickCount(unsigned count)
{
	tickCount_ = count;
}


std::vector<double> KvAxisTicker::generate(double lower, double upper, 
	std::vector<double>* subTicks, std::vector<std::string>* tickLabels)
{

}

