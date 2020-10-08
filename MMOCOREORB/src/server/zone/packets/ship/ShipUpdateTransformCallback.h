/*
 * ShipUpdateTransformCallback.h
 *
 *  Created on: 01/09/2011
 *      Author: TheAnswer
 */

#ifndef SHIPUPDATETRANSFORMCALLBACK_H_
#define SHIPUPDATETRANSFORMCALLBACK_H_

#include "../MessageCallback.h"
#include "engine/engine.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "ShipUpdateTransformCollisionMessage.h"
#include "ShipUpdateTransformMessage.h"
#include "server/zone/packets/object/DataTransform.h"
#include "server/zone/managers/collision/CollisionManager.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/packets/object/PlayClientEffectObjectMessage.h"

#include "PackedVelocity.h"
#include "PackedRotationRate.h"

class ShipUpdateTransformCallback : public MessageCallback {
	uint16 shipId;

	//PackedTransform dir is multiplied by 127, positions by 4.0958748
	uint8 dirX, dirY, dirZ, dirW;
	int16 posX, posZ, posY;

	//PackedVelocity
	//int16 velocitySpeed, velocityDirection;
	PackedVelocity velocity;

	//3 PackedRotationRate
	PackedRotationRate yawRate, pitchRate, rollRate;

	uint32 counter;

public:
	ShipUpdateTransformCallback(ZoneClientSession* client, ZoneProcessServer* server) :
		MessageCallback(client, server), shipId(0), dirX(0), dirY(0), dirZ(0), dirW(0),
		posX(0), posZ(0), posY(0), counter(0) {
	}

	void parse(Message* message) {
		//info(message->toStringData(), true);
		shipId = message->parseShort();

		dirX = message->parseByte();
		dirY = message->parseByte();
		dirZ = message->parseByte();
		dirW = message->parseByte();

		posX = message->parseSignedShort();
		posZ = message->parseSignedShort();
		posY = message->parseSignedShort();

		velocity.parse(message);

		yawRate.parse(message);
		pitchRate.parse(message);
		rollRate.parse(message);

		counter = message->parseInt();
	}

	void run() {
		float positionMultiplier = 4.0958748f;
		float positionX = posX / positionMultiplier;
		float positionY = posY / positionMultiplier;
		float positionZ = posZ / positionMultiplier;

		float directionX = (float)dirX / 127.f;
		float directionY = (float)dirY / 127.f;
		float directionZ = (float)dirZ / 127.f;
		float directionW = (float)dirW / 127.f;

		/*StringBuffer msg;
		msg << "short: 0x" << hex << unknownShort;
		info(msg.toString(), true);

		StringBuffer msg;
		msg << "positionX: " << posX << " positionZ:" << posZ << " positionY:" << posY;
		info(msg.toString(), true);

		StringBuffer msg;
		msg << "velA:" << velA << " velB:" << velB;
		info(msg.toString(), true);*/

    ManagedReference<SceneObject*> scene = client->getPlayer();

    if (scene == NULL)
      return;

    CreatureObject* player = cast<CreatureObject*>(scene.get());

		if (player == NULL)
			return;

		Locker _locker(player);

		PlayerObject* ghost = player->getPlayerObject();

		if (ghost == NULL)
			return;

		if (isnan(positionX) || isnan(positionY) || isnan(positionZ))
			return;

		if (isinf(positionX) || isinf(positionY) || isinf(positionZ))
			return;

		if (ghost->isTeleporting())
			return;

		if (player->getZone() == NULL)
			return;

		if (positionX > 8000.0f || positionX < -8000.0f || positionY > 8000.0f || positionY < -8000.0f) {
			StringBuffer msg;
			msg << "position out of bounds";
			player->error(msg.toString());

			return;
		}

		ManagedReference<ShipObject*> ship = dynamic_cast<ShipObject*>(player->getParent().get().get());

		if (ship == NULL)
			return;

		Locker clocker(ship, player);

		// if we are not in space, we want to transition when we've hit the upper atmosphere
		if (Zone* zone = player->getZone()) {
			if (!zone->getZoneName().contains("space")) {

				String spaceZone = "space_corellia";

				if (zone->getZoneName().contains("corellia")) {
					// yes this is redundant, don't judge me.
					spaceZone = "space_corellia";
				} else if (zone->getZoneName().contains("naboo")) {
					spaceZone = "space_naboo";
				} else if (zone->getZoneName().contains("tatooine")) {
					spaceZone = "space_tatooine";
				} else if (zone->getZoneName().contains("rori")) {
					spaceZone = "space_naboo";
				} else if (zone->getZoneName().contains("yavin4")) {
					spaceZone = "space_yavin4";
				} else if (zone->getZoneName().contains("talus")) {
					spaceZone = "space_corellia";
				} else if (zone->getZoneName().contains("lok")) {
					spaceZone = "space_lok";
				} else if (zone->getZoneName().contains("endor")) {
					spaceZone = "space_endor";
				} else if (zone->getZoneName().contains("dathomir")) {
					spaceZone = "space_dathomir";
				} else if (zone->getZoneName().contains("dantooine")) {
					spaceZone = "space_dantooine";

				}

				// TODO: fill in the rest of the zones here


				if (positionZ > 3000.0f) {
					player->info("a player has entered a space zone", true);

					ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

					if (!playerManager) {
						return;
					}

					ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

					if (!datapad) {
						return;
					}

					ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

					for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
						Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

						if (obj && obj->isShipControlDevice()) {
							Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
							if (controlDevice->getControlledObject() == ship) {
								shipControlDevice = controlDevice;
								break;
							}
						}
					}

					if (!shipControlDevice) {
						return;
					}

					Locker shipLock(shipControlDevice);

					player->info("storing ship", true);
					shipControlDevice->storeObject(player, true);

					// get the player in space
					player->switchZone(spaceZone, System::random(1500), System::random(1500), System::random(1500));

					player->info("generating ship", true);
					shipControlDevice->generateObject(player);

					return;
				}
			}
		}
		//++++++++++++++++++++++++++++------Planet Landing Procedure Begin----++++++++++++++++++++++++++++++++++++++++++++



				if (ship == NULL)
					return;


				// Landing from Space Station to planet in each Space Zone
				// this is a copy and modify of space entry codework
				if (Zone* zone = player->getZone()) {
					if (zone->getZoneName().contains("space")) {

						String spaceZone = "space_corellia";

						if (zone->getZoneName().contains("space_cor")) {
							spaceZone = "corellia";
						} else if (zone->getZoneName().contains("space_nab")) {
							spaceZone = "naboo";
						} else if (zone->getZoneName().contains("space_tat")) {
							spaceZone = "tatooine";
						} else if (zone->getZoneName().contains("space_yav")) {
							spaceZone = "yavin4";
						} else if (zone->getZoneName().contains("space_lo")) {
							spaceZone = "lok";
						} else if (zone->getZoneName().contains("space_end")) {
							spaceZone = "endor";
						} else if (zone->getZoneName().contains("space_dath")) {
							spaceZone = "dathomir";
						} else if (zone->getZoneName().contains("space_dant")) {
							spaceZone = "dantooine";

						}

						if (zone->getZoneName().contains("space") && positionX > 6000.0f && positionX < 7700.0f && positionZ > -6500.0f && positionZ < -4200.0f && positionY > -3500.0f && positionY < -1600.0f) {
							player->info("a player is landing on a planet", true);

							ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

							if (!playerManager) {
								return;
							}

							ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

							if (!datapad) {
								return;
							}

							ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

							for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
								Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

								if (obj && obj->isShipControlDevice()) {
									Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
									if (controlDevice->getControlledObject() == ship) {
										shipControlDevice = controlDevice;
										break;
									}
								}
							}

							if (!shipControlDevice) {
								return;
							}

							Locker shipLock(shipControlDevice);


							player->info("storing ship for planet entry", true);
							shipControlDevice->storeObject(player, true);
							player->clearState(CreatureState::PILOTINGSHIP);
							player->switchZone(spaceZone, System::random(5000), 0, System::random(5000));
							player->info("Planet Transfer Success", true);


							return;
						}
					}
				}

//+++++++++++++++++++++++++++++++-------Planet Landing Procedure End---------+++++++++++++++++++++++++++++++++
		//++++++++++++++++++++++++++++------Corvette Dungeon Zone in Begin----++++++++++++++++++++++++++++++++++++++++++++



				if (ship == NULL)
					return;


				// in space and looking to zone into another structure
				// this is a copy and modify of space entry codework for seperate space zone dungeons.
				if (Zone* zone = player->getZone()) {
					if (zone->getZoneName().contains("space")) {

						String spaceZone = "space_corellia";

						if (zone->getZoneName().contains("space_cor")) {
							spaceZone = "space_corellia";
						} else if (zone->getZoneName().contains("space_nab")) {
							spaceZone = "space_naboo";
						} else if (zone->getZoneName().contains("space_tat")) {
							spaceZone = "space_tatooine";
						} else if (zone->getZoneName().contains("space_yav")) {
							spaceZone = "space_yavin4";
						} else if (zone->getZoneName().contains("space_lo")) {
							spaceZone = "space_lok";
						} else if (zone->getZoneName().contains("space_end")) {
							spaceZone = "space_endor";
						} else if (zone->getZoneName().contains("space_dath")) {
							spaceZone = "space_dathomir";
						} else if (zone->getZoneName().contains("space_dant")) {
							spaceZone = "space_dantooine";

						}

						if (spaceZone == ("space_corellia") && positionX > 6000.0f && positionX < 8700.0f && positionZ > -8000.0f && positionZ < -6700.0f && positionY > 6000.0f && positionY < 8700.0f) {
							player->info("a player is attempting to enter the Corvette", true);

							ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

							if (!playerManager) {
								return;
							}

							ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

							if (!datapad) {
								return;
							}

							ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

							for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
								Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

								if (obj && obj->isShipControlDevice()) {
									Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
									if (controlDevice->getControlledObject() == ship) {
										shipControlDevice = controlDevice;
										break;
									}
								}
							}

							if (!shipControlDevice) {
								return;
							}

							Locker shipLock(shipControlDevice);


							player->info("storing ship Corvette Entry", true);
							shipControlDevice->storeObject(player, true);
							player->clearState(CreatureState::PILOTINGSHIP);
							player->switchZone("space_corellia", -42.6, 0, 0.1, 281475060670030);
							player->info("Corvette Transfer Success", true);


							return;
						}
					}
				}

//+++++++++++++++++++++++++++++++-------Corvette Dungeon Zone in End---------+++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++------ISD Dungeon Zone in Begin----++++++++++++++++++++++++++++++++++++++++++++



		if (ship == NULL)
			return;


		// in space and looking to zone into another structure
		// this is a copy and modify of space entry codework for seperate space zone dungeons.
		if (Zone* zone = player->getZone()) {
			if (zone->getZoneName().contains("space")) {

				String spaceZone = "space_corellia";

				if (zone->getZoneName().contains("space_cor")) {
					spaceZone = "space_corellia";
				} else if (zone->getZoneName().contains("space_nab")) {
					spaceZone = "space_naboo";
				} else if (zone->getZoneName().contains("space_tat")) {
					spaceZone = "space_tatooine";
				} else if (zone->getZoneName().contains("space_yav")) {
					spaceZone = "space_yavin4";
				} else if (zone->getZoneName().contains("space_lo")) {
					spaceZone = "space_lok";
				} else if (zone->getZoneName().contains("space_end")) {
					spaceZone = "space_endor";
				} else if (zone->getZoneName().contains("space_dath")) {
					spaceZone = "space_dathomir";
				} else if (zone->getZoneName().contains("space_dant")) {
					spaceZone = "space_dantooine";

				}

				if (spaceZone == ("space_tatooine") && positionX > -7400.0f && positionX < -5000.0f && positionZ > -7400.0f && positionZ < -5000.0f && positionY > -7400.0f && positionY < -5000.0f) {
					player->info("a player is attempting to enter the ISD", true);

					ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

					if (!playerManager) {
						return;
					}

					ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

					if (!datapad) {
						return;
					}

					ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

					for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
						Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

						if (obj && obj->isShipControlDevice()) {
							Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
							if (controlDevice->getControlledObject() == ship) {
								shipControlDevice = controlDevice;
								break;
							}
						}
					}

					if (!shipControlDevice) {
						return;
					}

					Locker shipLock(shipControlDevice);


					player->info("storing ship ISD Entry", true);
					shipControlDevice->storeObject(player, true);
					player->clearState(CreatureState::PILOTINGSHIP);
					player->switchZone("space_tatooine", 8.8, 173.8, 64.3, 281475109153579);
					player->info("ISD Transfer Success", true);


					return;
				}
			}
		}

//+++++++++++++++++++++++++++++++-------ISD Dungeon Zone in End---------+++++++++++++++++++++++++++++++++
		//++++++++++++++++++++++++++++------Nova Orion Zone in Begin----++++++++++++++++++++++++++++++++++++++++++++



				if (ship == NULL)
					return;


				// in space and looking to zone into another structure
				// this is a copy and modify of space entry codework for seperate space zone dungeons.
				if (Zone* zone = player->getZone()) {
					if (zone->getZoneName().contains("space")) {

						String spaceZone = "space_corellia";

						if (zone->getZoneName().contains("space_cor")) {
							spaceZone = "space_corellia";
						} else if (zone->getZoneName().contains("space_nab")) {
							spaceZone = "space_naboo";
						} else if (zone->getZoneName().contains("space_tat")) {
							spaceZone = "space_tatooine";
						} else if (zone->getZoneName().contains("space_yav")) {
							spaceZone = "space_yavin4";
						} else if (zone->getZoneName().contains("space_lo")) {
							spaceZone = "space_lok";
						} else if (zone->getZoneName().contains("space_end")) {
							spaceZone = "space_endor";
						} else if (zone->getZoneName().contains("space_dath")) {
							spaceZone = "space_dathomir";
						} else if (zone->getZoneName().contains("space_dant")) {
							spaceZone = "space_dantooine";

						}

						if (spaceZone == ("space_corellia") && positionX > -3000.0f && positionX < -500.0f && positionZ > 4000.0f && positionZ < 6800.0f && positionY > 4000.0f && positionY < 6700.0f) {
							player->info("a player is attempting to enter Nova Orion", true);

							ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

							if (!playerManager) {
								return;
							}

							ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

							if (!datapad) {
								return;
							}

							ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

							for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
								Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

								if (obj && obj->isShipControlDevice()) {
									Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
									if (controlDevice->getControlledObject() == ship) {
										shipControlDevice = controlDevice;
										break;
									}
								}
							}

							if (!shipControlDevice) {
								return;
							}

							Locker shipLock(shipControlDevice);


							player->info("storing ship Nova Orion Entry", true);
							shipControlDevice->storeObject(player, true);
							player->clearState(CreatureState::PILOTINGSHIP);
							player->switchZone("space_corellia", 71.5, 0.8, 51.8, 281475109085805);
							player->info("Nova Orion Transfer Success", true);


							return;
						}
					}
				}

//+++++++++++++++++++++++++++++++-------Nova Orion Zone in End---------+++++++++++++++++++++++++++++++++


		Vector3 collisionPoint, targetPosition(positionX, positionY, positionZ);

		if (CollisionManager::checkShipCollision(ship, targetPosition, collisionPoint)) {
			//ship->teleport(ship->getPositionX(), ship->getPositionZ(), ship->getPositionY());
			//ship->info("colliding with terrain", true);
			//ship->setDirection(directionW, directionX, directionY, directionZ);
			ship->setMovementCounter(counter);

			ShipUpdateTransformCollisionMessage* msg = new ShipUpdateTransformCollisionMessage(ship);
			player->broadcastMessage(msg, true);

			PlayClientEffectObjectMessage* effect = new PlayClientEffectObjectMessage(ship, "clienteffect/space_collision.cef", "");
			player->broadcastMessage(effect, true);

			return;
		}

		ship->setMovementCounter(counter);
		ship->setPosition(positionX, positionZ, positionY);
		ship->setDirection(directionW, directionX, directionY, directionZ);
		ship->updateZone(false, false);

		player->setPosition(positionX, positionZ, positionY);
		//object->setDirection(directionW, directionX, directionY, directionZ);

		player->updateZone(false, false);

		ShipUpdateTransformMessage* msga = new ShipUpdateTransformMessage(ship, dirX, dirY, dirZ, dirW, posX, posZ, posY,
				velocity, yawRate, pitchRate, rollRate);
		player->broadcastMessage(msga, false);

		ValidatedPosition* last = ghost->getLastValidatedPosition();
		last->setPosition(positionX, positionZ, positionY);
		ghost->updateServerLastMovementStamp();
	}
};


#endif /* SHIPUPDATETRANSFORMCALLBACK_H_ */
