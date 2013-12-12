#ifndef NET_FACTORY_H_
#define NET_FACTORY_H_

#include <string>
#include "Net.h"
//#include "GenieExport.h"

namespace Model {
	
//GENIE_API
	Net* createNet(std::string protocol);

}

#endif /* NET_FACTORY_H_ */