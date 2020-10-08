/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions. */

#ifndef FORCERUN1COMMAND_H_
#define FORCERUN1COMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "JediQueueCommand.h"
#include "server/zone/objects/creature/buffs/Buff.h"

class ForceRun1Command : public JediQueueCommand {
public:

	ForceRun1Command(const String& name, ZoneProcessServer* server)
	: JediQueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (creature->isInvisible()) {
			return GENERALERROR;
		}

		if (isWearingArmor(creature)) {
			return NOJEDIARMOR;
		}

		ManagedReference<WeaponObject*> weapon = creature->getWeapon();

		if (!weapon->isJediWeapon()) {
			creature->sendSystemMessage("You must have a Lightsaber equipped to use this ability");
			return INVALIDWEAPON;
		}



		if (creature->hasBuff(String("burstrun").hashCode())) {
			creature->sendSystemMessage("You cannot force run when burst running.");
			return GENERALERROR;
		}

		uint32 buffcrc1 = BuffCRC::JEDI_FORCE_RUN_1;
		uint32 buffcrc2 = BuffCRC::JEDI_FORCE_RUN_2;
		uint32 buffcrc3 = BuffCRC::JEDI_FORCE_RUN_3;

		if(creature->hasBuff(buffcrc1) || creature->hasBuff(buffcrc2) || creature->hasBuff(buffcrc3)) {
			creature->sendSystemMessage("@jedi_spam:force_buff_present");
			creature->removeBuff(buffcrc1);
			return GENERALERROR;
		}


		// Force cost of skill.
		int forceCost = 200;


		//Check for and deduct Force cost.

		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();


		if (playerObject->getForcePower() <= forceCost) {
			creature->sendSystemMessage("@jedi_spam:no_force_power"); //"You do not have enough Force Power to peform that action.

			return GENERALERROR;
		}

		playerObject->setForcePower(playerObject->getForcePower() - forceCost);

		StringIdChatParameter startStringId("jedi_spam", "apply_forcerun1");
		StringIdChatParameter endStringId("jedi_spam", "remove_forcerun1");

		int duration = 120;
		
		

		ManagedReference<Buff*> buff = new Buff(creature, buffcrc1, duration, BuffType::JEDI);
		
		Locker locker(buff);
				
		buff->setSpeedMultiplierMod(1.5f);
		buff->setAccelerationMultiplierMod(1.5f);
		buff->setStartMessage(startStringId);
		buff->setEndMessage(endStringId);
		buff->setSkillModifier("force_run", 1);

		creature->addBuff(buff);
		creature->playEffect("clienteffect/pl_force_run_self.cef", "");



		return SUCCESS;
	}

};


#endif //FORCERUN1COMMAND_H_
