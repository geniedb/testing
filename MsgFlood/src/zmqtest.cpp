#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <stdint.h>
#include <boost/thread.hpp>
#include "zmq.hpp"
#include "imp.h"
#include "CF_Platform.h"
#include "getopt.h"

const char* optString = "i:p:s:";

struct Settings_t {
	std::string pubAddress;
	uint16_t port;
	std::string subAddress;
} Settings;

void doSomething() {
	for (int i = 0;i < 1000;i++) {
		boost::this_thread::interruption_point();
		std::cout << "doing something";
	}
}

void display_usage() {
	std::cout << "usage!";
	std::cout << "-i         : publish ip\n";
	std::cout << "-s         : subscribe ip\n";
	std::cout << "-p         : port\n";
}

void getOptions(int argc, char *argv[]) {
	Settings.port = 5502;
	Settings.pubAddress = "";
	Settings.subAddress = "";
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
        }        
        opt = getopt( argc, argv, optString );
    }
}

int main (int argc, char *argv[])
{
	getOptions(argc, argv);
	int hwm = 100000;
	std::cout << "Using\n";
	std::cout << Genie::stringf("Publish IP     : %s\n", Settings.pubAddress.c_str());
	std::cout << Genie::stringf("Subscribe IP   : %s\n", Settings.subAddress.c_str());
	std::cout << Genie::stringf("Publish port   : %d\n", Settings.port);
	std::cout << Genie::stringf("High water mark: %d\n", hwm);

	zmq::context_t	context(1);
	zmq::socket_t* pubSocket = new zmq::socket_t(context, ZMQ_PUB);
	pubSocket->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
	zmq::socket_t* subSocket = new zmq::socket_t(context, ZMQ_SUB);
	subSocket->setsockopt( ZMQ_RCVHWM, &hwm, sizeof(hwm) );
	int linger = 0;
	subSocket->setsockopt( ZMQ_LINGER, &linger, sizeof(linger) );
	subSocket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
	try {
		pubSocket->bind( Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str() );
		bool pubConnected = pubSocket->connected();
		std::cout << Genie::stringf("Publish socket %sconnected.\n", pubConnected ? "" : "not ");
		
		subSocket->connect(Genie::stringf("tcp://%s:%d", Settings.subAddress.c_str(), Settings.port).c_str());
		bool subConnected = subSocket->connected();
		std::cout << Genie::stringf("Subscribe socket %sconnected.\n", subConnected ? "" : "not ");

		if (subConnected && pubConnected) {
			boost::thread testThread(doSomething);
			boost::posix_time::time_duration startupPause = boost::posix_time::microseconds(500);
			boost::this_thread::sleep(startupPause);

			testThread.interrupt();
			testThread.join();
		} else {
			std::cout << "Unable to make connections.\n";
		}
	} catch(...) {
		std::cout << "Error binding to pub socket.";
	}
	if (pubSocket)
		pubSocket->close();
	if (subSocket)
		subSocket->close();
	context.close();
	return 0;
}

