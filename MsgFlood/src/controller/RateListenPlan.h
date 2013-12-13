#ifndef RATE_LISTEN_PLAN_H_
#define RATE_LISTEN_PLAN_H_

#include "ListenOnlyPlan.h"

namespace Controller {
	
class RateListenPlan : public ListenOnlyPlan {
private:
	uint64_t expected;
	uint64_t count;
	Model::Message* getMessage();
public:
	RateListenPlan(Model::Settings_t& settings): ListenOnlyPlan(settings) {}
	virtual bool execute();
};

}

#endif /* RATE_LISTEN_PLAN_H_ */