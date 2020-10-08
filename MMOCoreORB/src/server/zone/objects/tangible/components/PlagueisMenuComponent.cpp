/*
 * PlagueisMenuComponent.cpp
 *
 *  Created on: 06/24/2015
 *      Author: Heat
 */

#include "PlagueisMenuComponent.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/tangible/components/PlagueisDataComponent.h"
#include "server/zone/packets/object/PlayClientEffectObjectMessage.h"

void PlagueisMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {

	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

	TangibleObject* ring = cast<TangibleObject*>(sceneObject);

	if (ring == NULL)
		return;

	PlagueisDataComponent* data = cast<PlagueisDataComponent*>(ring->getDataObjectComponent()->get());

	if (data == NULL || !data->isPlagueisData())
		return;

	if (data->getCharges() > 0)
		menuResponse->addRadialMenuItem(20, 3, "Summon the essence of Darth Plagueis");

}

int PlagueisMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {

	if (selectedID == 20) {

		if (!sceneObject->isASubChildOf(player))
			return 0;

		WearableObject* wearable = cast<WearableObject*>(sceneObject);

		if (wearable == NULL)
			return 0;

		PlagueisDataComponent* data = cast<PlagueisDataComponent*>(wearable->getDataObjectComponent()->get());

		if (data == NULL || !data->isPlagueisData())
			return 0;

		int charges = data->getCharges();

		if (charges <= 0)
			return 0;

		if (!wearable->isEquipped()) {
			player->sendSystemMessage("The Dark Ring must be equipped to use");
			return 0;
		}

		if (!player->isDead()) {
			player->sendSystemMessage("The essence of Darth Plagueis drains your life force, it knows you are not dead, and a charge has been used.");
			player->inflictDamage(player, CreatureAttribute::HEALTH, 1000, true);
			data->setCharges(charges - 1);
			String hardpoint = "";

			if (player->getSlottedObject("ring_r") != NULL && player->getSlottedObject("ring_r")->getObjectID() == sceneObject->getObjectID())
				hardpoint = "hold_r";
			else if (player->getSlottedObject("ring_l") != NULL && player->getSlottedObject("ring_l")->getObjectID() == sceneObject->getObjectID())
				hardpoint = "hold_l";

			PlayClientEffectObjectMessage* effect = new PlayClientEffectObjectMessage(player, "clienteffect/trap_electric_01.cef", hardpoint);
			player->broadcastMessage(effect, true);
			return 0;
		}

		if (!player->checkCooldownRecovery("darth_plagueis")) {
			Time* timeRemaining = player->getCooldownTime("darth_plagueis");
			StringIdChatParameter cooldown("quest/hero_of_tatooine/system_messages", "restore_not_yet");
			cooldown.setTO(getCooldownString(timeRemaining->miliDifference() * -1));
			player->sendSystemMessage(cooldown);
			return 0;
		}

		PlayerManager* pm = player->getZoneServer()->getPlayerManager();

		pm->enhanceCharacter(player);

		player->healDamage(player, CreatureAttribute::HEALTH, 6200);
		player->healDamage(player, CreatureAttribute::ACTION, 6200);
		player->healDamage(player, CreatureAttribute::MIND, 6200);

		data->setCharges(charges - 1);

		String hardpoint = "";

		if (player->getSlottedObject("ring_r") != NULL && player->getSlottedObject("ring_r")->getObjectID() == sceneObject->getObjectID())
			hardpoint = "hold_r";
		else if (player->getSlottedObject("ring_l") != NULL && player->getSlottedObject("ring_l")->getObjectID() == sceneObject->getObjectID())
			hardpoint = "hold_l";

		PlayClientEffectObjectMessage* effect = new PlayClientEffectObjectMessage(player, "clienteffect/trap_electric_01.cef", hardpoint);
		player->broadcastMessage(effect, true);

		player->sendSystemMessage("The essence of Darth Plagueis courses through your body and returns you to life");
		player->addCooldown("darth_plagueis", 3 * 3600 * 1000);

		// remove pvp death debuff
		{
			uint32 buffCRC = String("pvpDeathDebuff").hashCode();
			if (player->hasBuff(buffCRC))
			{
				player->removeBuff(buffCRC);
			}
		}

		return 0;
	} else {
		return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
	}

}

String PlagueisMenuComponent::getCooldownString(uint32 delta) {

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


