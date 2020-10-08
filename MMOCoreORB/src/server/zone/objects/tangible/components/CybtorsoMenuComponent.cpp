/*
 * CybtorsoMenuComponent.cpp
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */

#include "CybtorsoMenuComponent.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/tangible/components/CybtorsoDataComponent.h"
#include "server/zone/packets/object/PlayClientEffectObjectMessage.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/creature/buffs/SingleUseBuff.h"



void CybtorsoMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {

	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

	TangibleObject* ring = cast<TangibleObject*>(sceneObject);

	if (ring == NULL)
		return;

	CybtorsoDataComponent* data = cast<CybtorsoDataComponent*>(ring->getDataObjectComponent()->get());

	if (data == NULL || !data->isCybtorsoData())
		return;

	if (data->getCharges() > 0)
		menuResponse->addRadialMenuItem(20, 3, "Activate Shield Generator");

}

int CybtorsoMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {

	if (selectedID == 20) {

		if (!sceneObject->isASubChildOf(player))
			return 0;

		WearableObject* wearable = cast<WearableObject*>(sceneObject);

		if (wearable == NULL)
			return 0;

		CybtorsoDataComponent* data = cast<CybtorsoDataComponent*>(wearable->getDataObjectComponent()->get());

		if (data == NULL || !data->isCybtorsoData())
			return 0;

		int charges = data->getCharges();

		if (charges <= 0)
			return 0;

		if (!wearable->isEquipped()) {
			player->sendSystemMessage("This Torso must be attached to your body to use its Shield");
			return 0;
		}

		if (!player->checkCooldownRecovery("torso_shield")) {
			Time* timeRemaining = player->getCooldownTime("torso_shield");
			StringIdChatParameter cooldown("quest/hero_of_tatooine/system_messages", "restore_not_yet");
			cooldown.setTO(getCooldownString(timeRemaining->miliDifference() * -1));
			player->sendSystemMessage(cooldown);
			return 0;
		}

		PlayerManager* pm = player->getZoneServer()->getPlayerManager();



		uint32 buffCRC = BuffCRC::JEDI_FORCE_ARMOR_2;

		int buffTime = 3; // minutes

		StringIdChatParameter startMessage("Torso Shield Active - 95 % Damage Reduction");
		StringIdChatParameter endMessage("Shield Power Exhausted");

		buffTime *= 60; // convert minutes to seconds

		ManagedReference<Buff*> torsoBuff = new Buff(player, buffCRC, buffTime, BuffType::OTHER);
		
		Locker locker(torsoBuff);
				
		torsoBuff->setSkillModifier("force_armor", 95);
		torsoBuff->setStartMessage(startMessage);
		torsoBuff->setEndMessage(endMessage);

		player->addBuff(torsoBuff);

		data->setCharges(charges - 1);

		player->playEffect("clienteffect/pl_force_regain_consciousness_self.cef","");
		player->playEffect("clienteffect/trap_electric_01.cef","");
		player->playEffect("clienteffect/pl_force_regain_consciousness_self.cef", "");

		player->sendSystemMessage("Personal Shield Active");
		player->addCooldown("torso_shield", 3 * 3600 * 1000);


		return 0;
	} else {
		return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
	}

}

String CybtorsoMenuComponent::getCooldownString(uint32 delta) {

	int seconds = delta / 1000;

	int hours = seconds / 3600;
	seconds -= hours * 3600;

	int minutes = seconds / 60;
	seconds -= minutes * 60;

	StringBuffer buffer;

	if (hours > 0) {
		buffer << hours << " hour";

		if (hours > 1)
			buffer << "s";

		if (minutes > 0 || seconds > 0)
			buffer << ", ";
	}

	if (minutes > 0) {
		buffer << minutes << " minute";

		if (minutes > 1)
			buffer << "s";

		if (seconds > 0)
			buffer << ", ";
	}

	if (seconds > 0) {
		buffer << seconds << " second";

		if (seconds > 1)
			buffer << "s";
	}

	return buffer.toString();
}


