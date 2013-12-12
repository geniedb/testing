#include "ZmqMessage.h"

using namespace ZmqNet;
using namespace Model;


ZmqMessage::ZmqMessage(MessageType type, char* data, size_t dataSize, uint64_t counter) {
	this->data = data;
	this->dataSize = dataSize;
	if (dataSize < 12)
		throw("data not large enough");
	char* ptr = data;
	memcpy(ptr, &counter, sizeof(uint64_t));
	ptr += 8;
	memcpy(ptr, (type == DATA) ? "DATA" : "SHUT", 4);
}

ZmqMessage::ZmqMessage(char* data, size_t dataSize) {
	this->data = data;
	this->dataSize = dataSize;
}

ZmqMessage::~ZmqMessage() {
	delete data;
}

MessageType ZmqMessage::getType() {
	char typestr[5];
	memcpy(&typestr, (data+8), 4);
	typestr[4] = 0;
	if (!strcmp((const char*)&typestr, "DATA"))
		return DATA;
	else
		return SHUTDOWN;
}

size_t ZmqMessage::getDataSize() {
	return dataSize;
}

char* ZmqMessage::getData() {
	return data;
}

uint64_t ZmqMessage::getCounter() {
	uint64_t counterValue;
	memcpy(&counterValue, data, sizeof(uint64_t));
	return counterValue;
}