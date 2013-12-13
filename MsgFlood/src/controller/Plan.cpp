//#include "Plan.h"
#include "TerminatePlan.h"
#include "RateSendPlan.h"
#include "ReportEveryPlan.h"
#include "RateListenPlan.h"
#include "AvoidHwmPlan.h"

using namespace Model;

namespace Controller {

Plan* getPlan(Settings_t& settings) {
	if (settings.plan == "terminate") 
		return new TerminatePlan(settings);
	else if (settings.plan == "ratesend")
		return new RateSendPlan(settings);
	else if (settings.plan == "reportevery")
		return new ReportEveryPlan(settings);
	else if (settings.plan == "ratelisten")
		return new RateListenPlan(settings);
	else if (settings.plan == "avoidhwm")
		return new AvoidHwmPlan(settings);
	return NULL;
}

}