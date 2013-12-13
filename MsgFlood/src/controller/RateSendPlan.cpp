#include <boost/thread.hpp>
#include "RateSendPlan.h"
#include "../model/NetFactory.h"
#include "../DateTime.h"

using namespace Model;
using namespace Genie::DateTime;

namespace Controller {
		
bool RateSendPlan::execute() {
	// split each second up into 10 pieces, allocate 1/10 total write to each tenth second
	size_t leftThisSecond = settings.rate;
	size_t msgsPerTenth = settings.rate / 10;
	size_t leftThisTenth = msgsPerTenth;
	absolute_time startOfSecond = GetAbsoluteTime();
	absolute_time startOfTenth = startOfSecond;
	uint64_t count = 1;
	for (;;) {
		boost::this_thread::interruption_point();
		if (leftThisTenth > 0) {
			char* data = new char[settings.bytes];
			Message* message = net->createMessage(Model::DATA, data, settings.bytes, count++);
			if (count % settings.rate == 0)
				std::cout << count << " messages sent\n";
			pubSocket->send(message);
			leftThisTenth--;
			leftThisSecond--;
			continue;
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
				char* data = new char[settings.bytes];
				Message* message = net->createMessage(Model::DATA, data, settings.bytes, count++);
				if (count % settings.rate == 0)
					std::cout << count << " messages sent\n";
				pubSocket->send(message);
				leftThisSecond--;
			}
			leftThisSecond = settings.rate;
			startOfSecond = currTime;
		}
	}
	return true;
}

}