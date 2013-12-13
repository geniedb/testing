#include <boost/thread.hpp>
#include "ReportEveryPlan.h"
#include "../model/NetFactory.h"
#include "../DateTime.h"

using namespace Model;
using namespace Genie::DateTime;

namespace Controller {
		
bool ReportEveryPlan::execute() {
	uint64_t count = 0;
	uint64_t expecting = 0;
	absolute_time startOfPeriod = GetAbsoluteTime();
	for (;;) {
		Message* newMessage = subSocket->receive();
		if (newMessage != NULL) {
			if (expecting == 0) {
				expecting = newMessage->getCounter();
			}
			count++;
			if (count % settings.rate == 0) {
				absolute_time endOfPeriod = GetAbsoluteTime();
				int64_t duration = GetTimeDurationMillisecs(startOfPeriod, endOfPeriod);
				startOfPeriod = endOfPeriod;
				std::cout << count << " messages received. write rate: " << count / duration * 1000 << std::endl;
			}
			if (newMessage->getCounter() != expecting) {
				std::cout << "message drop detected. expected " << expecting << " got " << newMessage->getCounter() << "\n";
				expecting = 0;
			} else
				expecting++;
			delete newMessage->getData();
			delete newMessage;
		}
	}
	return true;
}

}