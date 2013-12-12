#ifndef ZMQ_MESSAGE_H_
#define ZMQ_MESSAGE_H_

#include "../CF_Platform.h"
#include "Message.h"

/*
	This is a simple test implementation of a message class.
	It uses 12 bytes in the supplied data buffer to store counter
	and type, so bear this in mind when comparing data
	*** if this message is sent, it should NOT be freed, zmq will do it
	*** if this message is not sent you must free it yourself
*/

namespace ZmqNet {

class ZmqMessage : public Model::Message {
friend class ZmqNetImpl;
friend class ZmqSubscribeSocket;
private:
	char* data;
	size_t dataSize;
	ZmqMessage(Model::MessageType type, char* data, size_t dataSize, uint64_t counter);
	ZmqMessage(char* data, size_t dataSize);
public:
	~ZmqMessage();
	virtual Model::MessageType getType();
	virtual size_t getDataSize();
	virtual char* getData();
	virtual uint64_t getCounter();
};

}

#endif /* ZMQ_MESSAGE_H_ */