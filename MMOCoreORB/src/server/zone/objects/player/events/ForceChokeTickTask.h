/*
 * ChannelForceRegenTask.h
 *
 *  Created on: Aug 18, 2011
 *      Author: swgemu
 */

#ifndef FORCECHOKETICKTASK_H_
#define FORCECHOKETICKTASK_H_


#include "server/zone/objects/creature/CreatureObject.h"

class ForceChokeTickTask : public Task {
	ManagedReference<CreatureObject*> creature;
	ManagedReference<CreatureObject*> creatureTarget;
	int counter;
	int damage;
	int amountOfTicks;
public:

	ForceChokeTickTask(CreatureObject* creo, CreatureObject* creoTarget) {
		creature = creo;  // This is the Attacker
		creatureTarget = creoTarget; // This is the target (one that takes damage)
		counter = 0;
		damage = 367 + (MAX(creo->getSkillMod("force_power_dark"), creo->getSkillMod("force_power_light")) * 3);
		amountOfTicks = 5;
		if (creo->hasSkill("force_rank_light_rank_01") || creo->hasSkill("force_rank_dark_rank_01"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_rank_02") || creo->hasSkill("force_rank_dark_rank_02"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_rank_03") || creo->hasSkill("force_rank_dark_rank_03"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_rank_04") || creo->hasSkill("force_rank_dark_rank_04"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_rank_07") || creo->hasSkill("force_rank_dark_rank_07"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_rank_10") || creo->hasSkill("force_rank_dark_rank_10"))
			amountOfTicks += 2;
		if (creo->hasSkill("force_rank_light_master") || creo->hasSkill("force_rank_dark_master"))
			amountOfTicks += 2;
	}

	void run() {
		Locker lockerC(creature);
		Locker lockerCT(creatureTarget,creature);
		if(creature != NULL && creatureTarget != NULL) {
			if (counter < amountOfTicks && !creatureTarget->isIncapacitated() && !creatureTarget->isDead() && (creatureTarget->getPvpStatusBitmask() != CreatureFlag::NONE)) {
				float damage = this->damage;

/*				ManagedReference<SceneObject*> psg = creatureTarget->getSlottedObject("utility_belt");
				if (psg != NULL && psg->isPsgArmorObject()) {
						if (cast<ArmorObject*>(psg.get())->isVulnerable(1)){
						float armorReduction =  cast<ArmorObject*>(psg.get())->getKinetic();
						if (armorReduction > 0) damage *= (1.f - (armorReduction / 50.f));
						Locker locker(cast<ArmorObject*>(psg.get()));
						cast<ArmorObject*>(psg.get())->inflictDamage(cast<ArmorObject*>(psg.get()), 0, damage * 0.1, true, true);
						}
					}
				float healthDamage = damage;
				float actionDamage = damage;
				float mindDamage = damage;

				Vector<ManagedReference<ArmorObject*> > healthArmor = creatureTarget->getWearablesDeltaVector()->getArmorAtHitLocation(CombatManager::CHEST);
				if (System::random(1) == 0)
					healthArmor = creatureTarget->getWearablesDeltaVector()->getArmorAtHitLocation(CombatManager::ARMS);
				ManagedReference<ArmorObject*> healthArmorToHit = NULL;
				if (!healthArmor.isEmpty())
					healthArmorToHit = healthArmor.get(System::random(healthArmor.size() - 1));
				if (healthArmorToHit !=NULL){
						float armorReduction = healthArmorToHit->getKinetic();
						if (armorReduction > 0) healthDamage *= (1.f - (armorReduction / 50.f));
						Locker locker(healthArmorToHit);
						healthArmorToHit->inflictDamage(healthArmorToHit, 0, healthDamage * 0.1, true, true);
					}
				Vector<ManagedReference<ArmorObject*> > actionArmor = creatureTarget->getWearablesDeltaVector()->getArmorAtHitLocation(CombatManager::LEGS);
				ManagedReference<ArmorObject*> actionArmorToHit = NULL;
				if (!actionArmor.isEmpty())
					actionArmorToHit = actionArmor.get(System::random(actionArmor.size() - 1));
				if (actionArmorToHit != NULL){
						float armorReduction = actionArmorToHit->getKinetic();
						if (armorReduction > 0) actionDamage *= (1.f - (armorReduction / 50.f));
						Locker locker(actionArmorToHit);
						actionArmorToHit->inflictDamage(actionArmorToHit, 0, actionDamage * 0.1, true, true);
					}
				Vector<ManagedReference<ArmorObject*> > mindArmor = creatureTarget->getWearablesDeltaVector()->getArmorAtHitLocation(CombatManager::HEAD);
				ManagedReference<ArmorObject*> mindArmorToHit = NULL;
				if (!mindArmor.isEmpty())
					mindArmorToHit = mindArmor.get(System::random(mindArmor.size() - 1));
				if (mindArmorToHit != NULL){
						float armorReduction = mindArmorToHit->getKinetic();
						if (armorReduction > 0) mindDamage *= (1.f - (armorReduction / 50.f));
						Locker locker(mindArmorToHit);
						mindArmorToHit->inflictDamage(mindArmorToHit, 0, mindDamage * 0.1, true, true);
				} */

				float healthDamage = damage;
				float actionDamage = damage;
				float mindDamage = damage;

				creatureTarget->inflictDamage(creature, CreatureAttribute::HEALTH, healthDamage, true);
				creatureTarget->inflictDamage(creature, CreatureAttribute::ACTION, actionDamage, true);
				creatureTarget->inflictDamage(creature, CreatureAttribute::MIND, mindDamage, true);

				creatureTarget->playEffect("clienteffect/pl_force_choke.cef", "");

				counter++;
				this->reschedule(6000); // Reschedule in 6 seconds...
			}
			else {

				creatureTarget->removePendingTask("forceChokeTickTask");
			}
		}
	}
};


#endif /* CHANNELFORCEREGENTASK_H_ */
