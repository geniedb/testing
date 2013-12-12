#ifndef NET_H_
#define NET_H_

#include "Message.h"

namespace Model {

struct NetSettings {
	int sendHighWaterMark;
	int receiveHighWaterMark;
	int linger;
};

class PublishSocket {
public:
	virtual bool send(Message* message) = 0;
	virtual bool connected() = 0;
	virtual void close() = 0;
};

class SubscribeSocket {
public:
	virtual Message* receive() = 0;
	virtual bool connected() = 0;
	virtual void close()= 0;
};

class Net {
public:
	virtual PublishSocket* connectPublish(std::string url, int port, NetSettings& netSettings) = 0;
	virtual SubscribeSocket* connectSubscribe(std::string subject, std::string url, int port, NetSettings& netSettings) = 0;
	virtual void close() = 0;
	virtual Message* createMessage(MessageType messageType, char* data, size_t dataSize, uint64_t counter) = 0;
};

}

#endif /* NET_H_ */