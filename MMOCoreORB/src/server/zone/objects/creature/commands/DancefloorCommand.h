/**
 * file DANCEFLOORCOMMAND_H_
 * author Heat
 * date 11.04.2015
 */

#ifndef DANCEFLOORCOMMAND_H_
#define DANCEFLOORCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class DancefloorCommand : public QueueCommand {
public:

	DancefloorCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (!creature->isEntertaining()) {
			creature->sendSystemMessage("@performance:effect_not_performing");
			return GENERALERROR;
		}

		int actionModifier = Integer::valueOf(arguments.toString());

		if (actionModifier > 3 || actionModifier < 1)
			actionModifier = 3;

		int actionCost = 30 * actionModifier;
		actionCost = creature->calculateCostAdjustment(CreatureAttribute::QUICKNESS, actionCost);
		if (creature->getHAM(CreatureAttribute::ACTION) <= actionCost) {
			creature->sendSystemMessage("@performance:effect_too_tired");
			return GENERALERROR;
		}
		creature->inflictDamage(creature, CreatureAttribute::ACTION, actionCost, true);


		StringBuffer effect;
		effect << "clienteffect/entertainer_dance_floor" << dec << actionModifier << ".cef";
		creature->playEffect(effect.toString(), "");

		creature->sendSystemMessage("You perform a dance floor stage effect");

		return SUCCESS;
	}

};

#endif //DANCEFLOORCOMMAND_H_
