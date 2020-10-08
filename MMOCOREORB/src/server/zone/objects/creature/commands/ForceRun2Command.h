/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions. */

#ifndef FORCERUN2COMMAND_H_
#define FORCERUN2COMMAND_H_

#include "server/zone/objects/creature/buffs/PrivateSkillMultiplierBuff.h"
#include "JediQueueCommand.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/creature/buffs/Buff.h"

class ForceRun2Command : public JediQueueCommand {
public:

	ForceRun2Command(const String& name, ZoneProcessServer* server)
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
			creature->removeBuff(buffcrc2);
			return GENERALERROR;
		}


		// Force cost of skill.
		int forceCost = 400;


		//Check for and deduct Force cost.

		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();


		if (playerObject->getForcePower() <= forceCost) {
			creature->sendSystemMessage("@jedi_spam:no_force_power"); //"You do not have enough Force Power to peform that action.

			return GENERALERROR;
		}

		playerObject->setForcePower(playerObject->getForcePower() - forceCost);

		StringIdChatParameter startStringId("jedi_spam", "apply_forcerun2");
		StringIdChatParameter endStringId("jedi_spam", "remove_forcerun2");

		int duration = 120;
		int divider = creature->getSkillMod("private_damage_divider");
		if (divider == 0) divider = 4;

		divider /= 4;

		ManagedReference<Buff*> buff = new Buff(creature, buffcrc2, duration, BuffType::JEDI);
		
		Locker locker(buff);
				
		buff->setSpeedMultiplierMod(2.5f);
		buff->setAccelerationMultiplierMod(2.5f);
		buff->setStartMessage(startStringId);
		buff->setEndMessage(endStringId);
		buff->setSkillModifier("force_run", 2);
		buff->setSkillModifier("private_damage_divisor", divider);

		creature->addBuff(buff);
		creature->playEffect("clienteffect/pl_force_run_self.cef", "");



		return SUCCESS;
	}

};


#endif //FORCERUN2COMMAND_H_
