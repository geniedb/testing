#include <boost/thread.hpp>
#include "ReportEveryPlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {
		
bool ReportEveryPlan::execute() {
	uint64_t count = 0;
	uint64_t expecting = 0;
	for (;;) {
		Message* newMessage = subSocket->receive();
		if (newMessage != NULL) {
			if (expecting == 0) {
				expecting = newMessage->getCounter();
			}
			count++;
			if (count % settings.rate == 0)
				std::cout << count << " messages received\n";
			if (newMessage->getCounter() != expecting) {
				std::cout << "message drop detected. expected " << expecting << " got " << newMessage->getCounter() << "\n";
				expecting = 0;
			} else
				expecting++;
		}
	}
	return true;
}

}