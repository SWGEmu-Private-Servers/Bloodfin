/*
 * DeedImplementation.cpp
 *
 *  Created on: Apr 18, 2010
 *      Author: crush
 */

#include "server/zone/objects/tangible/deed/ship/ShipDeed.h"
#include"server/zone/ZoneServer.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/manufactureschematic/ManufactureSchematic.h"
#include "server/zone/templates/tangible/VehicleDeedTemplate.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/ship/ShipObject.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/managers/planet/PlanetManager.h"

void ShipDeedImplementation::loadTemplateData(SharedObjectTemplate* templateData) {
	DeedImplementation::loadTemplateData(templateData);

	VehicleDeedTemplate* deedData = dynamic_cast<VehicleDeedTemplate*>(templateData);

	if (deedData == NULL)
		return;

	controlDeviceObjectTemplate = deedData->getControlDeviceObjectTemplate();
}

void ShipDeedImplementation::fillAttributeList(AttributeListMessage* alm, CreatureObject* object) {
	DeedImplementation::fillAttributeList(alm, object);

	//alm->insertAttribute("hit_points", hitPoints);
}

void ShipDeedImplementation::initializeTransientMembers() {
	DeedImplementation::initializeTransientMembers();

	setLoggingName("ShipDeed");
}

void ShipDeedImplementation::updateCraftingValues(CraftingValues* values, bool firstUpdate) {
	/*
	 * Values available:	Range:
	 *
	 * hitpoints			varies, integrity of vehicle
	 */

	hitPoints = (int) values->getCurrentValue("hit_points");
}

void ShipDeedImplementation::fillObjectMenuResponse(ObjectMenuResponse* menuResponse, CreatureObject* player) {
	DeedImplementation::fillObjectMenuResponse(menuResponse, player);

	if(isASubChildOf(player))
		menuResponse->addRadialMenuItem(20, 3, "@pet/pet_menu:menu_generate");
}

int ShipDeedImplementation::handleObjectMenuSelect(CreatureObject* player, byte selectedID) {

	if (selectedID == 20) {

		if (generated || !isASubChildOf(player))
			return 1;

		if (player->isInCombat() || player->getParentRecursively(SceneObjectType::BUILDING) != NULL) {
			player->sendSystemMessage("@pet/pet_menu:cant_call_vehicle"); //You can only unpack vehicles while Outside and not in Combat.
			return 1;
		}

		ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

		if (datapad == NULL) {
			player->sendSystemMessage("Datapad doesn't exist when trying to create vehicle");
			return 1;
		}

		// Check if this will exceed maximum number of vehicles allowed
		ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();
		/*
		int vehiclesInDatapad = 0;
		int maxStoredVehicles = playerManage2r->getBaseStoredVehicles();

		for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
			Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

			if (obj != NULL && obj->isVehicleControlDevice() )
				vehiclesInDatapad++;

		}

		if (vehiclesInDatapad >= maxStoredVehicles) {
			player->sendSystemMessage("@pet/pet_menu:has_max_vehicle"); // You already have the maximum number of vehicles that you can own.
			return 1;
		}
	*/


		Zone* zone = player->getZone();

		if (zone == NULL) {
			error("Failed to obtain zone.");
			return 1;
		}

		// try to find closest travel location that is a proper starport to park the ship there
		PlanetManager* planetManager= zone->getPlanetManager();

		if (planetManager == NULL) {
			player->sendSystemMessage("Sorry, can't create a ship around here.");
			error("Failed to obtain PlanetManager");
			return 1;
		}

		PlanetTravelPoint* travelPoint = planetManager->getNearestPlanetTravelPoint(player);

		if (travelPoint == NULL || !travelPoint->isInterplanetary()) {
			player->sendSystemMessage("Sorry, can't create a ship around here. Move closer to an interplanetary starport.");
			error("Failed to obtain PlanetTravelPoint");
			return 1;
		}
		CityRegion* arrivalRegion = planetManager->getRegionAt(travelPoint->getArrivalPositionX(), travelPoint->getArrivalPositionY());

		if (arrivalRegion == NULL) {
			error("No region at shuttle destination.");
			return 1;
		}

		String parkedCity = arrivalRegion->getRegionName();

		player->sendSystemMessage(parkedCity);

		Reference<ShipControlDevice*> shipControlDevice = server->getZoneServer()->createObject(controlDeviceObjectTemplate.hashCode(), 1).castTo<ShipControlDevice*>();
		
		Locker locker(shipControlDevice);
		
		Reference<ShipObject*> ship = (server->getZoneServer()->createObject(generatedObjectTemplate.hashCode(), 1)).castTo<ShipObject*>();

		if (ship == NULL || shipControlDevice == NULL) {
			if (shipControlDevice != NULL)
				shipControlDevice->destroyObjectFromDatabase(true);

			if (ship != NULL)
				ship->destroyObjectFromDatabase(true);

			player->sendSystemMessage("wrong vehicle object template " + generatedObjectTemplate);
			return 1;
		}

		Locker vlocker(ship, player);
		ship->createChildObjects();
		ship->setMaxCondition(hitPoints);
		ship->setConditionDamage(0);
		ship->setParkedLocation(arrivalRegion);
		shipControlDevice->setControlledObject(ship);
		datapad->transferObject(shipControlDevice, -1);

		if (datapad->transferObject(shipControlDevice, -1)) {
			datapad->broadcastObject(shipControlDevice, true);
			shipControlDevice->generateObject(player);

			generated = true;

			destroyObjectFromWorld(true);
			destroyObjectFromDatabase(true);

			return 0;
		} else {
			shipControlDevice->destroyObjectFromDatabase(true);
			return 1;
		}
	}

	return DeedImplementation::handleObjectMenuSelect(player, selectedID);
}

