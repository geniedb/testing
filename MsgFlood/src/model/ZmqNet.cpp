#include "ZmqNet.h"
#include "../CF_Platform.h"
#include "ZmqMessage.h"
#include "Message.h"

using namespace ZmqNet;
using namespace Model;

//=========== Class ZmqSocket members ===============
//TODO tried to use multiple inheritance to make connected() and close() implement
//virtual methods on Net level Socket classes, but had some issues. Fell back on
//having ZmqPublishSocket and ZmqSubscribeSocket implement connected() and close()
//but I suspect there is a solution to make this cleaner.

ZmqSocket::ZmqSocket(zmq::socket_t* socket) {
	this->socket = socket;
}

ZmqSocket::~ZmqSocket() {
	if (socket->connected()) 
		socket->close();
	delete socket;
}

bool ZmqSocket::connected() {
	return socket->connected();
}

void ZmqSocket::close() {
	socket->close();
}

//=========== Class ZmqPublishSocket members ===============

void freeMe(void* message, void* hint) {
	delete (char*)message;
}

bool ZmqPublishSocket::send(Message* message) {
	zmq::message_t msg(message->getData(), message->getDataSize(), freeMe);
	bool result = socket->send(msg);
	delete message;
	return result;
}

bool ZmqPublishSocket::connected() {
	return ZmqSocket::connected();
}

void ZmqPublishSocket::close() {
	return ZmqSocket::close();
}

//=========== Class ZmqSubscribeSocket members ===============

ZmqSubscribeSocket::ZmqSubscribeSocket(zmq::socket_t* socket) : ZmqSocket(socket) {
	pollitem.socket = (void*)*socket;
	pollitem.fd = 0;
	pollitem.events = ZMQ_POLLIN;
	pollitem.revents = 0;
}

Message* ZmqSubscribeSocket::receive() {
	int pollCount = zmq::poll(&pollitem, 1, 0);
	if (pollCount > 0) {
		zmq::message_t msg;
		socket->recv(&msg);
		char* data = new char[msg.size()];
		memcpy(data, msg.data(), msg.size());
		return new ZmqMessage(data, msg.size());
	} else
		return NULL;
}

bool ZmqSubscribeSocket::connected() {
	return ZmqSocket::connected();
}

void ZmqSubscribeSocket::close() {
	return ZmqSocket::close();
}

//=========== Class ZmqNetImpl members ===============

PublishSocket* ZmqNetImpl::connectPublish(std::string url, int port, NetSettings& netSettings) {
	zmq::socket_t* pubSocket = new zmq::socket_t(context, ZMQ_PUB);
	pubSocket->setsockopt(ZMQ_SNDHWM, &netSettings.sendHighWaterMark, sizeof(netSettings.sendHighWaterMark));
	pubSocket->bind(Genie::stringf("tcp://%s:%d", url.c_str(), port).c_str());
	return (PublishSocket*)new ZmqPublishSocket(pubSocket);
}

SubscribeSocket* ZmqNetImpl::connectSubscribe(std::string subject, std::string url, int port, NetSettings& netSettings) {
	zmq::socket_t* subSocket = new zmq::socket_t(context, ZMQ_SUB);
	subSocket->setsockopt(ZMQ_RCVHWM, &netSettings.receiveHighWaterMark, sizeof(netSettings.receiveHighWaterMark));
	subSocket->setsockopt(ZMQ_LINGER, &netSettings.linger, sizeof(netSettings.linger));
	subSocket->setsockopt(ZMQ_SUBSCRIBE, subject.c_str(), subject.length());
	subSocket->connect(Genie::stringf("tcp://%s:%d", url.c_str(), port).c_str());
	return (SubscribeSocket*)new ZmqSubscribeSocket(subSocket);
}

void ZmqNetImpl::close() {
	context.close();
}

Message* ZmqNetImpl::createMessage(MessageType messageType, char* data, size_t dataSize, uint64_t counter) {
	return new ZmqMessage(messageType, data, dataSize, counter);
}
