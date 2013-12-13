#include <boost/thread.hpp>
#include "SlowSendPlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {
		
bool SlowSendPlan::execute() {
	time_t startTime = time(NULL);
	double lastRunTime = 0;
	int count = 1;
	for (;;) {
		boost::this_thread::interruption_point();
		time_t currTime;
		time(&currTime);
		double runTime = difftime(currTime, startTime);
		if (runTime - lastRunTime > 1) {
			char* data = new char[1000];
			Model::Message* message = net->createMessage(Model::DATA, data, 1000, count);
			pubSocket->send(message);
			lastRunTime = runTime;
			std::cout << "sending msg " << count++ << "\n";
		}		
	}
	return true;
}

}