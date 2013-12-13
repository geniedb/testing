#ifndef SEND_ONLY_PLAN_H_
#define SEND_ONLY_PLAN_H_

#include "Plan.h"

namespace Controller {
	
class SendOnlyPlan : public Plan {
protected:
	Model::Settings_t& settings;
	Model::Net* net;
	Model::NetSettings netSettings;
	Model::PublishSocket* pubSocket;
public:
	SendOnlyPlan(Model::Settings_t& settings);
	virtual bool setup();
	virtual void tearDown();
};

}

#endif /* SEND_ONLY_PLAN_H_ */