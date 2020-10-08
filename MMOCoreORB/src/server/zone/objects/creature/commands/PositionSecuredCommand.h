/**
 * file POSITIONSECUREDCOMMAND_H_
 * author Heat
 * date 11.04.2015
 */

#ifndef POSITIONSECUREDCOMMAND_H_
#define POSITIONSECUREDCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
//#include "CombatQueueCommand.h"
//#include "server/zone/objects/creature/buffs/SingleUseBuff.h"
//#include "server/zone/objects/creature/events/PosSecEvent.h"

class PositionSecuredCommand : public JediQueueCommand {
public:

	PositionSecuredCommand(const String& name, ZoneProcessServer* server)
		: JediQueueCommand(name, server) {

		buffCRC = BuffCRC::TEST_FIRST;

		singleUseEventTypes.add(ObserverEventType::FORCEBUFFHIT);

		skillMods.put("force_armor", 95);


	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!creature->checkCooldownRecovery("position_secured")) {
			StringIdChatParameter stringId;

			Time* cdTime = creature->getCooldownTime("position_secured");

			// Returns -time. Multiple by -1 to return positive.
			int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

			stringId.setStringId("@innate:equil_wait"); // You are still recovering from your last equilization. Command available in %DI seconds.
			stringId.setDI(timeLeft);
			creature->sendSystemMessage(stringId);
			return GENERALERROR;
	}
		        creature->addCooldown("position_secured", 300 * 1000); 
			doJediSelfBuffCommand(creature);

		return SUCCESS;
		
	}

	void handleBuff(SceneObject* creature, ManagedObject* object, int64 param) {

		ManagedReference<CreatureObject*> creo = cast<CreatureObject*>( creature);
		if (creo == NULL)
			return;

		// Client Effect upon hit (needed)
		creo->playEffect("clienteffect/commando_position_secured.cef", "");

		ManagedReference<PlayerObject*> playerObject = creo->getPlayerObject();
		if (playerObject == NULL)
			return;

	}

};

#endif //POSITIONSECUREDCOMMAND_H_
