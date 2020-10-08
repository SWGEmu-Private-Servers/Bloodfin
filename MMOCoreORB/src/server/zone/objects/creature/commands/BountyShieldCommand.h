/**
 * file BOUNTYSHIELDCOMMAND_H_
 * author Heat
 * date 11.04.2015
 */

#ifndef BOUNTYSHIELDCOMMAND_H_
#define BOUNTYSHIELDCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
//#include "CombatQueueCommand.h"
//#include "server/zone/objects/creature/buffs/SingleUseBuff.h"

class BountyShieldCommand : public JediQueueCommand {
public:

	BountyShieldCommand(const String& name, ZoneProcessServer* server)
		: JediQueueCommand(name, server) {

		buffCRC = BuffCRC::TEST_SECOND;

		singleUseEventTypes.add(ObserverEventType::FORCEBUFFHIT);

		skillMods.put("force_armor", 95);


	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!creature->checkCooldownRecovery("bounty_shield")) {
			StringIdChatParameter stringId;

			Time* cdTime = creature->getCooldownTime("bounty_shield");

			// Returns -time. Multiple by -1 to return positive.
			int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

			stringId.setStringId("@innate:equil_wait"); // You are still recovering from your last equilization. Command available in %DI seconds.
			stringId.setDI(timeLeft);
			creature->sendSystemMessage(stringId);
			return GENERALERROR;
	}
		        creature->addCooldown("bounty_shield", 300 * 1000); 
			doJediSelfBuffCommand(creature);

		return SUCCESS;
		
	}

	void handleBuff(SceneObject* creature, ManagedObject* object, int64 param) {

		ManagedReference<CreatureObject*> creo = cast<CreatureObject*>( creature);
		if (creo == NULL)
			return;

		// Client Effect upon hit (needed)
		creo->playEffect("clienteffect/pl_force_armor_hit.cef", "");

		ManagedReference<PlayerObject*> playerObject = creo->getPlayerObject();
		if (playerObject == NULL)
		return;


	}

};

#endif //BOUNTYSHIELDCOMMAND_H_
