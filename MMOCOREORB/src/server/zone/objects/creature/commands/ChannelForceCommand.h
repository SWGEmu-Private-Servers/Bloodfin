/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CHANNELFORCECOMMAND_H_
#define CHANNELFORCECOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "ForcePowersQueueCommand.h"
#include "server/zone/objects/player/events/ChannelForceRegenTask.h"

class ChannelForceCommand : public ForcePowersQueueCommand {
public:

		ChannelForceCommand(const String& name, ZoneProcessServer* server)
		: ForcePowersQueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (creature->hasAttackDelay())
			return GENERALERROR;

		if (creature->isInvisible()) {
			return GENERALERROR;
		}

		if (isWearingArmor(creature)) {
			return NOJEDIARMOR;
		}
		
                if (!creature->checkCooldownRecovery("channel")) {
			StringIdChatParameter stringId;

			Time* cdTime = creature->getCooldownTime("channel");

			// Returns -time. Multiple by -1 to return positive.
			int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

			stringId.setStringId("@innate:equil_wait"); // You are still recovering from your last equilization. Command available in %DI seconds.
			stringId.setDI(timeLeft);
			creature->sendSystemMessage(stringId);
			return GENERALERROR;
		}		
		uint32 buffcrc;

		uint32 buffcrc1 = BuffCRC::JEDI_AVOID_INCAPACITATION;

		// Bonus is inbetween 200-300.
		int rand = System::random(10);
		int forceBonus = 300 + (rand * 10); // Needs to be divisible by amount of ticks.

		if(creature->hasBuff(buffcrc1)) {
			creature->sendSystemMessage("@jedi_spam:channel_ham_too_low"); // Your body is too weakened to perform that action.
			return GENERALERROR;
		}
		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

		if (playerObject == NULL)
			return GENERALERROR;

		// Do not execute if the player's force bar is full.
		if (playerObject->getForcePower() >= playerObject->getForcePowerMax())
			return GENERALERROR;

		// To keep it from going over max...
		if ((playerObject->getForcePowerMax() - playerObject->getForcePower()) < forceBonus)
			forceBonus = (playerObject->getForcePowerMax() - playerObject->getForcePower());

		int health = creature->getHAM(CreatureAttribute::HEALTH);
		int action = creature->getHAM(CreatureAttribute::ACTION);
		int mind = creature->getHAM(CreatureAttribute::MIND);

		if ((health <= forceBonus) || (action <= forceBonus) || (mind <= forceBonus)) {
			creature->sendSystemMessage("@jedi_spam:channel_ham_too_low"); // Your body is too weakened to perform that action.
			return GENERALERROR;
		}

		int maxHealth = creature->getMaxHAM(CreatureAttribute::HEALTH);
		int maxAction = creature->getMaxHAM(CreatureAttribute::ACTION);
		int maxMind = creature->getMaxHAM(CreatureAttribute::MIND);

		if ((maxHealth - creature->getWounds(CreatureAttribute::HEALTH) <= forceBonus) || (maxAction - creature->getWounds(CreatureAttribute::ACTION) <= forceBonus) || (maxMind - creature->getWounds(CreatureAttribute::MIND) <= forceBonus)) {
			creature->sendSystemMessage("@jedi_spam:channel_ham_too_low"); // Your body is too weakened to perform that action.
			return GENERALERROR;
		}

		// Give Force, and subtract HAM.

		playerObject->setForcePower(playerObject->getForcePower() + forceBonus);

		creature->setMaxHAM(CreatureAttribute::HEALTH, maxHealth - forceBonus, true);
		creature->setMaxHAM(CreatureAttribute::ACTION, maxAction - forceBonus, true);
		creature->setMaxHAM(CreatureAttribute::MIND, maxMind - forceBonus, true);
		creature->playEffect("clienteffect/pl_force_channel_self.cef", "");

		// Setup task.
		Reference<ChannelForceRegenTask*> cfTask = new ChannelForceRegenTask(creature, forceBonus);
		creature->addPendingTask("channelForceRegenTask", cfTask, 1000);
       	creature->addCooldown("channel", 19 * 1000);
		return SUCCESS;
	}

	float getCommandDuration(CreatureObject* object, const UnicodeString& arguments) const {
		return defaultTime * 1.0;
	}

};

#endif //CHANNELFORCECOMMAND_H_
