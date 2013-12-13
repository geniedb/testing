#ifndef SLOW_SEND_PLAN_H_
#define SLOW_SEND_PLAN_H_

#include "SendOnlyPlan.h"

namespace Controller {
	
class SlowSendPlan : public SendOnlyPlan {
public:
	SlowSendPlan(Model::Settings_t& settings): SendOnlyPlan(settings) {}
	virtual bool execute();
};

}

#endif /* SLOW_SEND_PLAN_H_ */