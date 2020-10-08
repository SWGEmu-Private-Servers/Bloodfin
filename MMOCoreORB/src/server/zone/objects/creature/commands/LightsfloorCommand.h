/**
 * file DANCEFLOORCOMMAND_H_
 * author Heat
 * date 11.04.2015
 */

#ifndef LIGHTSFLOORCOMMAND_H_
#define LIGHTSFLOORCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class LightsfloorCommand : public QueueCommand {
public:

	LightsfloorCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (!creature->isEntertaining()) {
			creature->sendSystemMessage("@performance:effect_not_performing");
			return GENERALERROR;
		}

		int actionCost = 30 * 2;
		actionCost = creature->calculateCostAdjustment(CreatureAttribute::QUICKNESS, actionCost);
		if (creature->getHAM(CreatureAttribute::ACTION) <= actionCost) {
			creature->sendSystemMessage("@performance:effect_too_tired");
			return GENERALERROR;
		}
		creature->inflictDamage(creature, CreatureAttribute::ACTION, actionCost, true);

		StringBuffer effect;
		effect << "clienteffect/entertainer_dance_floor"<< ".cef";
		creature->playEffect(effect.toString(), "");

		creature->sendSystemMessage("You perform a dance floor stage effect");

		return SUCCESS;
	}

};

#endif //LIGHTSFLOORCOMMAND_H_
