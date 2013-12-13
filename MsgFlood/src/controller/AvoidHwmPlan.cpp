#include <boost/thread.hpp>
#include "AvoidHwmPlan.h"
#include "../model/NetFactory.h"

using namespace Model;
using namespace Genie::DateTime;

namespace Controller {

bool AvoidHwmPlan::execute() {
	writeRate = 0;
	count = 0;
	countInSecond = 0;
	tenthStartTime = GetAbsoluteTime();
	secondStartTime = GetAbsoluteTime();
	sendInTenthAllotment = 0;
	for (;;) {
		applyFeedback(subSocket->receive());
		sendNextMessage();
	}
}

void AvoidHwmPlan::applyFeedback(Message* hbMessage) {
	if (hbMessage != NULL) {
		lastHbCount = hbMessage->getCounter();
		uint64_t subscriberBehind = count -  hbMessage->getCounter();
		// assume next Hb will come in 1 second. apply a message rate that will not
		// quite fill up the hwm in the next second.
		writeRate = (settings.targetHighWaterMark - subscriberBehind) * 0.95;
		if (writeRate < 0)
			writeRate == 0;
		tenthStartTime = GetAbsoluteTime();
		sendInTenthAllotment = writeRate / 10;
		//std::cout << "write rate set: " << writeRate << std::endl;
	}
}

void AvoidHwmPlan::sendNextMessage() {
	if (sendInTenthAllotment > 0) {
		char* data = new char[settings.bytes];
		Message* newMessage = net->createMessage(DATA, data, settings.bytes, ++count);
		countInSecond++;
		pubSocket->send(newMessage);
		sendInTenthAllotment--;
		//std::cout << sendInTenthAllotment << std::endl;
		//return;
	}
	absolute_time currentTime = GetAbsoluteTime();
	int64_t timeUsed = GetTimeDurationMillisecs(tenthStartTime, currentTime);
	//std::cout << "timeUsed: " << timeUsed << std::endl;
	if (timeUsed > 100) {
		tenthStartTime = currentTime;
		sendInTenthAllotment = writeRate / 10;
		//std::cout << "allotment: " << sendInTenthAllotment << std::endl;
	}
	timeUsed = GetTimeDurationMillisecs(secondStartTime, currentTime);
	if (timeUsed > 1000) {
		std::cout << countInSecond << " messages sent. write rate: " << writeRate;
		std::cout << " ahead by: " << (count - lastHbCount) << std::endl;
		secondStartTime = currentTime;
		countInSecond = 0;
	}
}

}