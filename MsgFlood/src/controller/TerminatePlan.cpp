#include <boost/thread.hpp>
#include "TerminatePlan.h"
#include "../model/NetFactory.h"

using namespace Model;

namespace Controller {

bool TerminatePlan::execute() {
	char* shutdown = new char[12];
	Model::Message* message = net->createMessage(Model::SHUTDOWN, shutdown, 12, 1);
	pubSocket->send(message);
	std::cout << "shutdown sent\n";
	char* data = new char[1000];
	Model::Message* message2 = net->createMessage(Model::DATA, data, 1000, 2);
	pubSocket->send(message2);
	boost::posix_time::time_duration startupPause = boost::posix_time::microseconds(500);
	boost::this_thread::sleep(startupPause);
	return true;
}

}