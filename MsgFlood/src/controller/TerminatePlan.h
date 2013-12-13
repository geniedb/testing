#ifndef TERMINATE_PLAN_H_
#define TERMINATE_PLAN_H_

#include "SendOnlyPlan.h"

namespace Controller {
	
class TerminatePlan : public SendOnlyPlan {
public:
	TerminatePlan(Model::Settings_t& settings): SendOnlyPlan(settings) {}
	virtual bool execute();
};

}

#endif /* TERMINATE_PLAN_H_ */