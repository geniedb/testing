#ifndef REPORT_EVERY_PLAN_H_
#define REPORT_EVERY_PLAN_H_

#include "ListenOnlyPlan.h"

namespace Controller {
	
class ReportEveryPlan : public ListenOnlyPlan {
public:
	ReportEveryPlan(Model::Settings_t& settings): ListenOnlyPlan(settings) {}
	virtual bool execute();
};

}

#endif /* REPORT_EVERY_PLAN_H_ */