#include <boost/thread.hpp>
#include "ListenOnlyPlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {
		
ListenOnlyPlan::ListenOnlyPlan(Model::Settings_t& settings) : settings(settings) {}

bool ListenOnlyPlan::setup() {
	net = createNet(settings.protocol);
	netSettings.sendHighWaterMark = settings.hwm;
	netSettings.receiveHighWaterMark = settings.hwm;
	netSettings.linger = -1;
	subSocket = net->connectSubscribe("", settings.pubAddress, settings.port, netSettings);
	bool pubConnected = subSocket->connected();
	std::cout << Genie::stringf("Publish socket %sconnected to %s\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", settings.pubAddress.c_str(), settings.port).c_str());
	return true;
}

void ListenOnlyPlan::tearDown() {
	if (subSocket) {
		subSocket->close();
		delete subSocket;
	}
	net->close();
	delete net;
}

}