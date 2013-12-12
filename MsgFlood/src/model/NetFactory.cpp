#include "NetFactory.h"
#include "ZmqNet.h"

using namespace ZmqNet;
using namespace Model;

namespace Model {

Net* createNet(std::string protocol) {
	if (protocol != "zmq")
		throw("only zmq protocol is currently supported");
	return new ZmqNetImpl();
}

}