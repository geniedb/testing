#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <stdint.h>
#include <boost/thread.hpp>
#include <time.h>
#include "zmq.hpp"
#include "imp.h"
#include "CF_Platform.h"
#include "getopt.h"

const char* optString = "i:p:s:S?x";

struct Settings_t {
	std::string pubAddress;
	uint16_t port;
	std::string subAddress;
	bool publisher;
	bool sendKill;
	double maxLifetime;
	int hwm;
} Settings;

void freeMe(void* item, void* hint) {
	delete[] item;
}

void doSomething() {//zmq::socket_t* pubSocket, zmq::socket_t* subSocket) {
	
	zmq::context_t	context(1);
	zmq::socket_t* pubSocket = new zmq::socket_t(context, ZMQ_PUB);
	pubSocket->setsockopt(ZMQ_SNDHWM, &Settings.hwm, sizeof(Settings.hwm));
	zmq::socket_t* subSocket = new zmq::socket_t(context, ZMQ_SUB);
	subSocket->setsockopt( ZMQ_RCVHWM, &Settings.hwm, sizeof(Settings.hwm) );
	int linger = 0;
	subSocket->setsockopt( ZMQ_LINGER, &linger, sizeof(linger) );
	subSocket->setsockopt(ZMQ_SUBSCRIBE, "x", 1);
	
	bool pubConnected = false;
	if (Settings.pubAddress != "") {
		pubSocket->bind( Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str() );
		pubConnected = pubSocket->connected();
		std::cout << Genie::stringf("Publish socket %sconnected to %s\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str());
	}
		
	subSocket->connect(Genie::stringf("tcp://%s:%d", Settings.subAddress.c_str(), Settings.port).c_str());
	bool subConnected = subSocket->connected();
	std::cout << Genie::stringf("Subscribe socket %sconnected to %s\n", subConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.subAddress.c_str(), Settings.port).c_str());


	time_t startTime = time(NULL);
	for (int i = 0;i < 10;i++) {
		boost::this_thread::interruption_point();
		std::cout << "doing something";
		
		//char* data = new char[1000];

		//zmq::message_t zmqmsg((void *)data, 1000, freeMe);
		//pubSocket->send(zmqmsg);
	}
		std::cout << "7\n";

	// receive message loop
	//std::vector<zmq::pollitem_t> subscribePollItems;
		std::cout << "8\n";
	zmq::pollitem_t pollitem = {static_cast<void*>(subSocket), 0, ZMQ_POLLIN, 0};
		std::cout << "9\n";
	//subscribePollItems.push_back(pollitem);
		std::cout << "10\n";
	int pollCount;
		std::cout << "11\n";
	for (;;) {
		time_t currTime;
		std::cout << "12\n";
		time(&currTime);
		std::cout << "13\n";
		Sleep(10000);
		double runTime = difftime(startTime, currTime);
		std::cout << "14\n";
		if (runTime > Settings.maxLifetime)
			break;
		std::cout << "15\n";
		try {
			pollCount = zmq::poll(&pollitem, 1, -1);
		} catch (int err) {
			//ETERM,EFAULT, EINTR
			std::cout << "Error: " << err << "\n";

		}
		std::cout << "1\n";
		if (pollCount == 0)
			continue;
		zmq::message_t msg;
		std::cout << "2\n";
		subSocket->recv(&msg);
		std::cout << "3\n";
		std::string messageStr((char*)msg.data());
		std::cout << "4\n";
		std::cout << messageStr << "\n";
		std::cout << "5\n";
		if (messageStr == "shutdown")
			break;
		std::cout << "6\n";
	}
	if (pubSocket)
		pubSocket->close();
	if (subSocket)
		subSocket->close();
	context.close();
}

void print_usage() {
	std::cout << "usage:\n";
	std::cout << "-i         : publish ip\n";
	std::cout << "-s         : subscribe ip\n";
	std::cout << "-p         : port\n";
	std::cout << "-S         : this node is the subscriber\n";
	std::cout << "-x         : connect to publish socket and send kill command\n";
}

int getOptions(int argc, char *argv[]) {
	Settings.port = 5502;
	Settings.pubAddress = "";
	Settings.subAddress = "";
	Settings.publisher = true;
	Settings.sendKill = false;
	Settings.maxLifetime = 180;
	Settings.hwm = 100000;
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
	std::cout << Genie::stringf("Publish IP     : %s\n", Settings.pubAddress.c_str());
	std::cout << Genie::stringf("Subscribe IP   : %s\n", Settings.subAddress.c_str());
	std::cout << Genie::stringf("Publish port   : %d\n", Settings.port);
	std::cout << Genie::stringf("High water mark: %d\n", Settings.hwm);
	std::cout << Genie::stringf("This node acting as %s\n", Settings.publisher ? "Publisher" : "Subscriber");

	try {
		if (Settings.sendKill && Settings.pubAddress != "") {
			zmq::context_t	context(1);
			zmq::socket_t* pubSocket = new zmq::socket_t(context, ZMQ_PUB);
			pubSocket->bind( Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str() );
			bool pubConnected = pubSocket->connected();
			std::cout << Genie::stringf("Publish socket %sconnected to %s.\n", pubConnected ? "" : "not ", Genie::stringf("tcp://%s:%d", Settings.pubAddress.c_str(), Settings.port).c_str());
			char* data = "shutdown";
			zmq::message_t zmqmsg((void *)data, strlen(data), freeMe);
			pubSocket->send(zmqmsg);
			if (pubSocket)
				pubSocket->close();
			context.close();
		} else {

			boost::thread testThread(doSomething);
			boost::posix_time::time_duration startupPause = boost::posix_time::microseconds(500);
			boost::this_thread::sleep(startupPause);
			//testThread.interrupt();
			testThread.join();
		}
	} catch(...) {
		std::cout << "Error binding to pub socket.";
	}
	return 0;
}

