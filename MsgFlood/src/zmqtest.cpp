#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <stdint.h>
#include <boost/thread.hpp>
#include <time.h>
#include "CF_Platform.h"
#include "getopt.h"
#include "model/Net.h"
#include "model/NetFactory.h"
#include "model/Message.h"

const char* optString = "i:p:s:S?xt";

struct Settings_t {
	std::string pubAddress;
	uint16_t port;
	std::string subAddress;
	bool publisher;
	bool sendKill;
	double maxLifetime;
	int hwm;
	bool testModeSend;
	const char* protocol;
} Settings;

void doSomething() {
	Model::Net* net = Model::createNet(Settings.protocol);
	Model::NetSettings netSettings = {Settings.hwm, Settings.hwm, -1};

	Model::PublishSocket* pubSocket = net->connectPublish(Settings.pubAddress, Settings.port, netSettings);
	Model::SubscribeSocket* subSocket = net->connectSubscribe("", Settings.subAddress, Settings.port, netSettings);
		
	bool pubConnected = pubSocket->connected();
	std::cout << Genie::stringf("Publish socket %sconnected to %s\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str());
		
	bool subConnected = subSocket->connected();
	std::cout << Genie::stringf("Subscribe socket %sconnected to %s\n", subConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.subAddress.c_str(), Settings.port).c_str());

	time_t startTime = time(NULL);
	double lastRunTime = 0;
	int count = 1;
	if (Settings.testModeSend) {
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
	}

	int pollCount = 0;
	for (;;) {
		time_t currTime;
		time(&currTime);
		Model::Message* newMessage = NULL;
		double runTime = difftime(currTime, startTime);
		if (runTime > Settings.maxLifetime)
			break;
		try {
			subConnected = subSocket->connected();
			newMessage = subSocket->receive();
		} catch (...) {
			std::cout << "oops there was an exception\n";
		}
		if (newMessage == NULL)
			continue;
		std::cout << (newMessage->getType() == Model::DATA ? "DATA" : "SHUTDOWN") << "\n";
		if (newMessage->getType() == Model::SHUTDOWN)
			break;
	}
	if (pubSocket) {
		pubSocket->close();
		delete pubSocket;
	}
	if (subSocket) {
		subSocket->close();
		delete subSocket;
	}
	net->close();
	delete net;
}

void print_usage() {
	std::cout << "usage:\n";
	std::cout << "-i         : publish ip\n";
	std::cout << "-s         : subscribe ip\n";
	std::cout << "-p         : port. Default: 5502\n";
	std::cout << "-S         : this node is the subscriber. Default: no\n";
	std::cout << "-t         : this node will open a pub socket and send a message every 2 seconds. Default: no\n";
	std::cout << "-x         : connect to publish socket and send kill command. Default: no\n";
	std::cout << "-P         : messaging protocol. Default: zmq\n";
}

const char* ZMQ_PROTOCOL = "zmq";

int getOptions(int argc, char *argv[]) {
	Settings.port = 5502;
	Settings.pubAddress = "";
	Settings.subAddress = "";
	Settings.publisher = true;
	Settings.sendKill = false;
	Settings.maxLifetime = 180;
	Settings.hwm = 100000;
	Settings.testModeSend = false;
	Settings.protocol = ZMQ_PROTOCOL;
	int opt = getopt(argc, argv, optString);
    while( opt != -1 ) {
		switch( opt ) {
			case 'i':
				Settings.pubAddress = optarg;
				break;
			case 's':
				Settings.subAddress = optarg;
				break;
			case 'p':
				Settings.port = atoi(optarg);
				break;
			case '?':
				print_usage();
				return 1;
				break;
			case 'S':
				Settings.publisher = false;
				break;
			case 'x':
				Settings.sendKill = true;
				break;
			case 't':
				Settings.testModeSend = true;
				break;
			case 'P':
				Settings.protocol = optarg;
        }        
        opt = getopt( argc, argv, optString );
    }
	return 0;
}

int main (int argc, char *argv[])
{
	if (getOptions(argc, argv))
		return 0;
	std::cout << "Using\n";
	std::cout << Genie::stringf("This node acting as %s\n", Settings.publisher ? "Publisher" : "Subscriber");
	std::cout << Genie::stringf("Publish IP     : %s\n", Settings.pubAddress.c_str());
	std::cout << Genie::stringf("Subscribe IP   : %s\n", Settings.subAddress.c_str());
	std::cout << Genie::stringf("Publish port   : %d\n", Settings.port);
	std::cout << Genie::stringf("High water mark: %d\n", Settings.hwm);
	std::cout << Genie::stringf("Protocol       : %s\n", Settings.protocol);

	try {
		if (Settings.sendKill && Settings.pubAddress != "") {
			Model::Net* net = Model::createNet(Settings.protocol);
			Model::NetSettings netSettings = {Settings.hwm, Settings.hwm, -1};
			Model::PublishSocket* pubSocket = net->connectPublish(Settings.pubAddress, Settings.port, netSettings);
			bool pubConnected = pubSocket->connected();
			std::cout << Genie::stringf("Publish socket %sconnected to %s.\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str());
			char* shutdown = new char[12];
			Model::Message* message = net->createMessage(Model::SHUTDOWN, shutdown, 12, 1);
			pubSocket->send(message);
			if (pubSocket) {
				pubSocket->close();
				delete pubSocket;
			}
			net->close();
			delete net;
		} else {

			boost::thread testThread(doSomething);
			//boost::posix_time::time_duration startupPause = boost::posix_time::microseconds(500);
			//boost::this_thread::sleep(startupPause);
			//testThread.interrupt();
			testThread.join();
		}
	} catch(...) {
		std::cout << "Error binding to pub socket.";
	}
	return 0;
}

