#ifndef SLOW_SEND_PLAN_H_
#define SLOW_SEND_PLAN_H_

#include "SendOnlyPlan.h"

namespace Controller {
	
class RateSendPlan : public SendOnlyPlan {
public:
	RateSendPlan(Model::Settings_t& settings): SendOnlyPlan(settings) {}
	virtual bool execute();
};

}

#endif /* SLOW_SEND_PLAN_H_ */