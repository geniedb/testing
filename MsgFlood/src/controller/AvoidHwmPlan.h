#ifndef AVOID_HWM_PLAN_H_
#define AVOID_HWM_PLAN_H_

#include "TwoWayPlan.h"
#include "../DateTime.h"

namespace Controller {
	
class AvoidHwmPlan : public TwoWayPlan {
private:
	Genie::DateTime::absolute_time tenthStartTime;
	Genie::DateTime::absolute_time secondStartTime;
	uint64_t sendInTenthAllotment;
	uint64_t count;
	uint64_t countInSecond;
	uint64_t writeRate;
	void applyFeedback(Model::Message* hbMessage);
	void sendNextMessage();
public:
	AvoidHwmPlan(Model::Settings_t& settings): TwoWayPlan(settings) {};
	virtual bool execute();
};

}

#endif /* AVOID_HWM_PLAN_H_ */