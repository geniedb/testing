#include <boost/thread.hpp>
#include "RateListenPlan.h"
#include "../model/NetFactory.h"
#include "../DateTime.h"

using namespace Model;
using namespace Genie::DateTime;

namespace Controller {

Message* RateListenPlan::getMessage() {
	Message* result = subSocket->receive();
	if (result != NULL) {
		if (expected == 0)
			expected = result->getCounter();
		if (result->getCounter() != expected) 
			std::cout << "message drop detected. expected " << expected << " got " << result->getCounter() << "\n";
		expected = result->getCounter() + 1;
	}
	return result;
}
		
bool RateListenPlan::execute() {
	// split each second up into 10 pieces, allocate 1/10 total write to each tenth second
	size_t leftThisSecond = settings.rate;
	size_t msgsPerTenth = settings.rate / 10;
	size_t leftThisTenth = msgsPerTenth;
	expected = 0;
	absolute_time startOfSecond = GetAbsoluteTime();
	absolute_time startOfTenth = startOfSecond;
	uint64_t count = 0;
	for (;;) {
		boost::this_thread::interruption_point();
		if (leftThisTenth > 0) {
			Message* message = getMessage();
			if (message != NULL) {
				leftThisTenth--;
				leftThisSecond--;
				delete message->getData();
				delete message;
				continue;
			}
		}
		absolute_time currTime = GetAbsoluteTime();
		int64_t tenthTime = GetTimeDurationMillisecs(startOfTenth, currTime);
		if (tenthTime > 100) {
			//start new tenth
			leftThisTenth = msgsPerTenth;
			startOfTenth = currTime;
		} else
			continue;
		int64_t secondTime = GetTimeDurationMillisecs(startOfSecond, currTime);
		if (secondTime >= 1000) {
			//might be 1 message left over, if rate is less than 10
			if (leftThisSecond > 0) {
				Message* message = getMessage();
				if (message != NULL) {
					leftThisSecond--;
					delete message->getData();
					delete message;
				}
			}
			leftThisSecond = settings.rate;
			std::cout << count << " messages received\n";
			count = 0;
		}
	}
	return true;
}

}