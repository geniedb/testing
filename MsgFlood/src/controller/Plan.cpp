//#include "Plan.h"
#include "TerminatePlan.h"
#include "RateSendPlan.h"

using namespace Model;

namespace Controller {

Plan* getPlan(Settings_t& settings) {
	if (settings.plan == "terminate") 
		return new TerminatePlan(settings);
	else if (settings.plan == "ratesend")
		return new RateSendPlan(settings);
	return NULL;
}

}