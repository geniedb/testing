//#include "Plan.h"
#include "TerminatePlan.h"
#include "RateSendPlan.h"
#include "ReportEveryPlan.h"

using namespace Model;

namespace Controller {

Plan* getPlan(Settings_t& settings) {
	if (settings.plan == "terminate") 
		return new TerminatePlan(settings);
	else if (settings.plan == "ratesend")
		return new RateSendPlan(settings);
	else if (settings.plan == "reportevery")
		return new ReportEveryPlan(settings);
	return NULL;
}

}