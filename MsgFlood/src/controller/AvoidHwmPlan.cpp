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
		uint64_t subscriberBehind = count -  hbMessage->getCounter();
		// assume next Hb will come in 1 second. apply a message rate that will not
		// quite fill up the hwm in the next second.
		writeRate = (settings.hwm - subscriberBehind) * 0.95;
		if (writeRate < 0)
			writeRate == 0;
	}
}

void AvoidHwmPlan::sendNextMessage() {
	if (sendInTenthAllotment > 0) {
		char* data = new char[settings.bytes];
		Message* newMessage = net->createMessage(DATA, data, settings.bytes, ++count);
		countInSecond++;
		pubSocket->send(newMessage);
		delete newMessage;
		sendInTenthAllotment--;
		return;
	}
	absolute_time currentTime = GetAbsoluteTime();
	int64_t timeUsed = GetTimeDurationMillisecs(tenthStartTime, currentTime);
	if (timeUsed > 100) {
		tenthStartTime = currentTime;
		sendInTenthAllotment = writeRate / 10;
	}
	timeUsed = GetTimeDurationMillisecs(secondStartTime, currentTime);
	if (timeUsed > 1000) {
		std::cout << countInSecond << " messages sent\n";
		secondStartTime = currentTime;
		countInSecond = 0;
	}
}

}