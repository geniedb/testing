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
#include "controller/Plan.h"

const char* optString = "i:p:s:S?xm:P:r:b:h:t:";

void print_usage() {
	std::cout << "usage:\n";
	std::cout << "-i:        : publish ip\n";
	std::cout << "-s:        : subscribe ip\n";
	std::cout << "-p:        : port. Default: 5502\n";
	std::cout << "-S         : this node is the subscriber. Default: no\n";
	std::cout << "-m:        : messaging protocol. Default: zmq\n";
	std::cout << "-P:        : plan to execute\n";
	std::cout << "-r:        : rate to send or receive. used by some plans\n";
	std::cout << "-b:        : size of messages to send. used by some plans\n";
	std::cout << "-h:        : high water mark\n";
	std::cout << "-t:        : target high water mark\n";
	std::cout << "Plans:\n";
	std::cout << "     terminate: Publisher. Sends a terminate message\n";
	std::cout << "     ratesend : Publisher. Sends messages of size/rate specified\n";
	std::cout << "     reportevery : Subscriber. Reports every [rate] messages it receives\n";
	std::cout << "     ratelisten : Subscriber. Reads at specified rate, reports msg/s\n";
	std::cout << "     avoidhwm : Two Way. Uses heartbeats to adjust it's write rate so as to avoid HWM\n";
	std::cout << "                Use with ratelisten on other node\n";
}

const char* ZMQ_PROTOCOL = "zmq";

int getOptions(Model::Settings_t& settings, int argc, char *argv[]) {
	settings.port = 5502;
	settings.pubAddress = "";
	settings.subAddress = "";
	settings.publisher = true;
	settings.sendKill = false;
	settings.maxLifetime = 180;
	settings.hwm = 100000;
	settings.targetHighWaterMark = 50000;
	settings.rate = 1;
	settings.bytes = 9;
	settings.protocol = ZMQ_PROTOCOL;
	int opt = getopt(argc, argv, optString);
    while( opt != -1 ) {
		switch( opt ) {
			case 'i':
				settings.pubAddress = optarg;
				break;
			case 's':
				settings.subAddress = optarg;
				break;
			case 'p':
				settings.port = atoi(optarg);
				break;
			case '?':
				print_usage();
				return 1;
				break;
			case 'S':
				settings.publisher = false;
				break;
			case 'x':
				settings.sendKill = true;
				break;
			case 'm':
				settings.protocol = optarg;
				break;
			case 'P':
				settings.plan = optarg;
				break;
			case 'r':
				settings.rate = atoi(optarg);
				if (settings.rate < 1)
					settings.rate = 1;
				break;
			case 'b':
				settings.bytes = atoi(optarg);
				if (settings.bytes < 9)
					settings.bytes = 9;
				break;
			case 'h':
				settings.hwm = atoi(optarg);
				break;
			case 't':
				settings.targetHighWaterMark = atoi(optarg);
				break;
        }        
        opt = getopt( argc, argv, optString );
    }
	return 0;
}

void executePlan(Controller::Plan* plan) {
	if (plan->setup()) {
		plan->execute();
		plan->tearDown();
	}
	else
		std::cout << "plan setup failed\n";	
}

int main (int argc, char *argv[])
{
	Model::Settings_t settings;
	if (getOptions(settings, argc, argv))
		return 0;
	std::cout << "Using\n";
	std::cout << Genie::stringf("This node acting as %s\n", settings.publisher ? "Publisher" : "Subscriber");
	std::cout << Genie::stringf("Publish IP     : %s\n", settings.pubAddress.c_str());
	std::cout << Genie::stringf("Subscribe IP   : %s\n", settings.subAddress.c_str());
	std::cout << Genie::stringf("Publish port   : %d\n", settings.port);
	std::cout << Genie::stringf("High water mark: %d\n", settings.hwm);
	std::cout << Genie::stringf("Target HWM     : %d\n", settings.targetHighWaterMark);
	std::cout << Genie::stringf("Protocol       : %s\n", settings.protocol);
	std::cout << Genie::stringf("Rate           : %d\n", settings.rate);
	std::cout << Genie::stringf("Size(bytes)    : %d\n", settings.bytes);
	std::cout << Genie::stringf("Plan           : %s\n", settings.plan.c_str());

	try {
		Controller::Plan* plan = Controller::getPlan(settings);
		if (plan) {				
			boost::thread testThread(executePlan, plan);
			testThread.join();
		} else {
			std::cout << "unable to load plan " << settings.plan << "\n";
		}
	} catch(...) {
		std::cout << "Error binding to pub socket.\n";
	}
	return 0;
}

