#include <boost/thread.hpp>
#include "SendOnlyPlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {
		
SendOnlyPlan::SendOnlyPlan(Model::Settings_t& settings) : settings(settings) {}

bool SendOnlyPlan::setup() {
	net = createNet(settings.protocol);
	netSettings.sendHighWaterMark = settings.hwm;
	netSettings.receiveHighWaterMark = settings.hwm;
	netSettings.linger = -1;
	pubSocket = net->connectPublish(settings.pubAddress, settings.port, netSettings);
	bool pubConnected = pubSocket->connected();
	std::cout << Genie::stringf("Publish socket %sconnected to %s\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", settings.pubAddress.c_str(), settings.port).c_str());
	return true;
}

void SendOnlyPlan::tearDown() {
	if (pubSocket) {
		pubSocket->close();
		delete pubSocket;
	}
	net->close();
	delete net;
}

}