#ifndef LISTEN_ONLY_PLAN_H_
#define LISTEN_ONLY_PLAN_H_

#include "Plan.h"

namespace Controller {
	
class ListenOnlyPlan : public Plan {
protected:
	Model::Settings_t& settings;
	Model::Net* net;
	Model::NetSettings netSettings;
	Model::SubscribeSocket* subSocket;
public:
	ListenOnlyPlan(Model::Settings_t& settings);
	virtual bool setup();
	virtual void tearDown();
};

}

#endif /* LISTEN_ONLY_PLAN_H_ */