/*
Copyright (C) 2007 <SWGEmu>
This File is part of Core3.
This program is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software
Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General
Public License along with this program; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Linking Engine3 statically or dynamically with other modules
is making a combined work based on Engine3.
Thus, the terms and conditions of the GNU Lesser General Public License
cover the whole combination.

In addition, as a special exception, the copyright holders of Engine3
give you permission to combine Engine3 program with free software
programs or libraries that are released under the GNU LGPL and with
code included in the standard release of Core3 under the GNU LGPL
license (or modified versions of such code, with unchanged license).
You may copy and distribute such a system following the terms of the
GNU LGPL for Engine3 and the licenses of the other code concerned,
provided that you include the source code of that other code when
and as the GNU LGPL requires distribution of source code.

Note that people who make modified versions of Engine3 are not obligated
to grant this special exception for their modified versions;
it is their choice whether to do so. The GNU Lesser General Public License
gives permission to release a modified version without this exception;
this exception also makes it possible to release a modified version
which carries forward this exception.

*/

#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/Zone.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/packets/player/PlayMusicMessage.h"

void ShipControlDeviceImplementation::generateObject(CreatureObject* player) {

	if (player->isDead() || player->isIncapacitated()){
		player->sendSystemMessage("You cant fly on your back cowboy");
		return;
	}
	if (player->getParent() != NULL || player->isInCombat()) {
		player->sendSystemMessage("You cant deploy your ship in combat");
		return;
	}
	if (player->isInvisible()) {
		player->sendSystemMessage("You cant fly while stealthed");
		return;
	}
	Zone* zone = player->getZone();

	if (zone->getZoneName().contains("kashyyyk")) {

		player->sendSystemMessage("The Wookiee High Council has barred any personal starship flight on this planet");
		return;
	}
	if (zone->getZoneName().contains("rori")) {

		player->sendSystemMessage("Due to the GCW conflict on this planet starship flight on this planet has been restricted");
		return;
	}
	if (zone->getZoneName().contains("hoth")) {

		player->sendSystemMessage("Due to the stormy nature of this planet, ground control has denied any personal starship flight on this planet");
		return;
	}


	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if(zone == NULL || controlledObject == NULL || controlledObject->isInQuadTree() || !this->isASubChildOf(player))
		return;

	Locker clocker(controlledObject, player);

	controlledObject->initializePosition(player->getPositionX(), player->getPositionZ() + 10, player->getPositionY());

	zone->transferObject(controlledObject, -1, true);

	controlledObject->transferObject(player, 5, true);

	player->setState(CreatureState::PILOTINGSHIP);

	player->sendSystemMessage("if your Ship goes 2d at any point, simply store it using your datapad radial and re-deploy it. If after many tries it does not render back to 3d, time to buy a new one and try to avoid crashing into objects");

	updateStatus(1);

	PlayerObject* ghost = player->getPlayerObject();

	if (ghost != NULL)
		ghost->setTeleporting(true);

}
void ShipControlDeviceImplementation::storeObject(CreatureObject* player, bool force) {


	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject == NULL)
		return;

	Locker clocker(controlledObject, player);

	if (!controlledObject->isInQuadTree())
		return;

	Zone* zone = player->getZone();

	if (zone == NULL)
		return;

	if (zone->getZoneName().contains("space_")) {

		player->sendSystemMessage("Your Ship is stored. Your body begins to ache from exposure");

		player->playEffect("clienteffect/cbt_msl_impact_concussion.cef");

		player->clearState(CreatureState::PILOTINGSHIP);

		zone->transferObject(player, -1, false);

		controlledObject->destroyObjectFromWorld(true);

		transferObject(controlledObject, 4, true);

		player->setDirection(0);

/*		player->switchZone("corellia", -148, 28, -4721, 0); */

		updateStatus(0);
		return;
	}
	player->clearState(CreatureState::PILOTINGSHIP);

	zone->transferObject(player, -1, false);
	
	controlledObject->destroyObjectFromWorld(true);

	transferObject(controlledObject, 4, true);
	
	updateStatus(0);
}

void ShipControlDeviceImplementation::fillObjectMenuResponse(ObjectMenuResponse* menuResponse, CreatureObject* player) {
	//ControlDeviceImplementation::fillObjectMenuResponse(menuResponse, player);

	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	PlayMusicMessage* pmm = new PlayMusicMessage("sound/ship_hyperspace_begin.snd");
	player->sendMessage(pmm);

	if (!controlledObject->isInQuadTree()) {
		menuResponse->addRadialMenuItem(60, 3, "Deploy Ship"); //Launch
	} else
		menuResponse->addRadialMenuItem(61, 3, "Store Ship"); //Launch
	//menuResponse->addRadialMenuItem(61, 3, "Launch Ship"); //Launch
}

bool ShipControlDeviceImplementation::canBeTradedTo(CreatureObject* player, CreatureObject* receiver, int numberInTrade) {
	ManagedReference<SceneObject*> datapad = receiver->getSlottedObject("datapad");

	if (datapad == NULL)
		return false;

	ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

	int shipsInDatapad = numberInTrade;
	int maxStoredShips = playerManager->getBaseStoredShips();

	for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
		Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

		if (obj != NULL && obj->isShipControlDevice() ){
			shipsInDatapad++;
		}
	}

	if( shipsInDatapad >= maxStoredShips){
		player->sendSystemMessage("That person has too many ships in their datapad");
		receiver->sendSystemMessage("You already have the maximum number of ships that you can own.");
		return false;
	}

	return true;
}
