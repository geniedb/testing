#ifndef PLAN_H_
#define PLAN_H_

#include <string>
#include "../model/Net.h"

namespace Controller {

class Plan {
public:
	virtual bool setup() = 0;
	virtual bool execute() = 0;
	virtual void tearDown() = 0;
};

Plan* getPlan(Model::Settings_t& settings);

}

#endif /* PLAN_H_ */