
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "CF_Platform.h"

namespace Model {

enum MessageType {
	DATA, SHUTDOWN
};

class Message {
public:
	virtual MessageType getType()=0;
	virtual size_t getDataSize()=0;
	virtual char* getData()=0;
	virtual uint64_t getCounter()=0;
};

}
#endif /* MESSAGE_H_ */