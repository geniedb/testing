#ifndef TWO_WAY_PLAN_H_
#define TWO_WAY_PLAN_H_

#include "Plan.h"

namespace Controller {
	
class TwoWayPlan : public Plan {
protected:
	Model::Settings_t& settings;
	Model::Net* net;
	Model::NetSettings netSettings;
	Model::SubscribeSocket* subSocket;
	Model::PublishSocket* pubSocket;
	void sendHeartbeat(uint64_t counterAccepted);
public:
	TwoWayPlan(Model::Settings_t& settings);
	virtual bool setup();
	virtual void tearDown();
};

}

#endif /* TWO_WAY_PLAN_H_ */