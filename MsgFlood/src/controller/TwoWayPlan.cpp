#include <boost/thread.hpp>
#include "TwoWayPlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {
		
TwoWayPlan::TwoWayPlan(Model::Settings_t& settings) : settings(settings) {}

bool TwoWayPlan::setup() {
	net = createNet(settings.protocol);
	netSettings.sendHighWaterMark = settings.hwm;
	netSettings.receiveHighWaterMark = settings.hwm;
	netSettings.linger = -1;
	subSocket = net->connectSubscribe("", settings.subAddress, settings.port, netSettings);
	bool subConnected = subSocket->connected();
	std::cout << Genie::stringf("Subscribe socket %sconnected to %s\n", subConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", settings.subAddress.c_str(), settings.port).c_str());
	pubSocket = net->connectPublish(settings.pubAddress, settings.port, netSettings);
	bool pubConnected = pubSocket->connected();
	std::cout << Genie::stringf("Publish socket %sconnected to %s\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", settings.pubAddress.c_str(), settings.port).c_str());
	return true;
}

void TwoWayPlan::tearDown() {
	if (subSocket) {
		subSocket->close();
		delete subSocket;
	}
	if (pubSocket) {
		pubSocket->close();
		delete pubSocket;
	}
	net->close();
	delete net;
}

void TwoWayPlan::sendHeartbeat(uint64_t counterAccepted) {
	char* data = new char[10];
	Message* hb = net->createMessage(DATA, data, 10, counterAccepted);
	pubSocket->send(hb);
	delete hb;
}

}