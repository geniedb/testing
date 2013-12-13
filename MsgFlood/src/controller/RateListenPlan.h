#ifndef RATE_LISTEN_PLAN_H_
#define RATE_LISTEN_PLAN_H_

#include "TwoWayPlan.h"

namespace Controller {
	
class RateListenPlan : public TwoWayPlan {
private:
	uint64_t expected;
	uint64_t count;
	uint64_t totalCount;
	Model::Message* getMessage();
public:
	RateListenPlan(Model::Settings_t& settings): TwoWayPlan(settings) {}
	virtual bool execute();
};

}

#endif /* RATE_LISTEN_PLAN_H_ */
