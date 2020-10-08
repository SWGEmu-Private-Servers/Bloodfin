/*
 * HolocronMenuComponent.cpp
 *
 *  Created on: 01/23/2012
 *      Author: xyborn
 */

#include "HolocronMenuComponent.h"

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/player/PlayerObject.h"

#include "server/zone/ZoneServer.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/managers/jedi/JediManager.h"
#include "server/zone/objects/player/sui/messagebox/SuiMessageBox.h"
#include "server/zone/managers/skill/SkillManager.h"

int HolocronMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* creature, byte selectedID) {
	if (selectedID != 20)
		return 0;

	if (!sceneObject->isASubChildOf(creature))
		return 0;

	ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

	if (playerObject == NULL)
		return 0;

	if (!creature->checkCooldownRecovery("used_holocron")) {
				StringIdChatParameter stringId;

				Time* cdTime = creature->getCooldownTime("used_holocron");


				int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

				stringId.setStringId("@innate:vit_wait");
				stringId.setDI(timeLeft);
				creature->sendSystemMessage(stringId);
				return 0;}

	if (playerObject->getJediState() <= 0) {
		int rand = System::random(100);
		if (rand >= 70) {
			creature->sendSystemMessage("You successfully open the Holocron... You feel connected to the Force. You feel drawn to a Force shrine.");
			creature->playEffect("clienteffect/trap_electric_01.cef", "");
			creature->addCooldown("used_holocron", 0.5 * 3600);
			sceneObject->destroyObjectFromWorld(true);
			SkillManager::instance()->awardSkill("force_title_jedi_novice", creature, true, true, true);
			playerObject->setJediState(1);
		}else {
			creature->sendSystemMessage("You break the Holocron while you try to unlock it...");
			creature->playEffect("clienteffect/trap_electric_01.cef", "");
			creature->addCooldown("used_holocron", 2 * 3600);
			sceneObject->destroyObjectFromWorld(true);
			return 0;
		}
	}
	if (playerObject != NULL && playerObject->getJediState() >= 2) {
		if (playerObject->getForcePower() <= playerObject->getForcePowerMax()) {
			creature->sendSystemMessage("@jedi_spam:holocron_force_replenish"); //The holocron hums softly as you feel your Force power replenish.
			creature->playEffect("clienteffect/trap_electric_01.cef", "");

			playerObject->setForcePower(playerObject->getForcePowerMax(), true);

			creature->addCooldown("used_holocron", 1 * 3600000); //3,600,000 = 1 hr
			sceneObject->destroyObjectFromWorld(true);
		} else {
			creature->sendSystemMessage("@jedi_spam:holocron_force_max"); //You are already at your maximum Force power.
		}
	} else {
		if (playerObject->getJediState() >= 2)
			creature->sendSystemMessage("@jedi_spam:holocron_no_effect"); //The holocron hums briefly, but otherwise does nothing.
	}


	return 0;
}

