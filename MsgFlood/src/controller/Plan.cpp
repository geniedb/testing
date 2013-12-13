//#include "Plan.h"
#include "TerminatePlan.h"
#include "SlowSendPlan.h"

using namespace Model;

namespace Controller {

Plan* getPlan(Settings_t& settings) {
	if (settings.plan == "terminate") 
		return new TerminatePlan(settings);
	else if (settings.plan == "slowsend")
		return new SlowSendPlan(settings);
	return NULL;
}

}