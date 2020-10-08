/*
 * JtlShipListResponse.h
 *
 *  Created on: Apr 25, 2011
 *      Author: crush
 */

#ifndef JTLSHIPLISTRESPONSE_H_
#define JTLSHIPLISTRESPONSE_H_

#include "ObjectControllerMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "ObjectControllerMessageCallback.h"
#include "server/zone/objects/area/ActiveArea.h"
#include "server/zone/objects/region/Region.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/ship/ShipObject.h"
#include "server/zone/managers/stringid/StringIdManager.h"

//TODO: This is very unsafe still...
class JtlShipListResponse : public ObjectControllerMessage {
public:
	JtlShipListResponse(CreatureObject* creo, SceneObject* terminal)
		: ObjectControllerMessage(creo->getObjectID(), 0x0B, 0x41D) {

		insertInt(1);

		SceneObject* datapad = creo->getSlottedObject("datapad");



		if (datapad == NULL) {
			insertInt(1);

		} else {

			VectorMap<uint64, ManagedReference<SceneObject*> >* datapadObjects = datapad->getContainerObjects();

			if (datapadObjects != NULL) {

				insertInt(datapadObjects->size()+1); // size




				int offs = getOffset();

	//			insertInt(sizeof(unsigned long));

				insertLong(terminal->getObjectID());

			//	 TODO: Better method of this.
			//	ManagedReference<ActiveArea*> region = terminal->getActiveRegion();

			//	if (region != NULL && region->isRegion())
			//	else
			//		insertAscii(terminal->getZone()->getZoneName());



				//insertAscii("cRush Rocks");


				for (int i = 0; i < datapadObjects->size(); ++i) {
					ManagedReference<SceneObject*> datapadObject = datapadObjects->get(i);

					if (datapadObject->getGameObjectType() == SceneObjectType::SHIPCONTROLDEVICE) {
						ManagedReference<ShipControlDevice*> shipControlDevice = cast<ShipControlDevice*>( datapadObject.get());

			//			if (shipControlDevice->getControlledObject() != NULL) {
			//				ManagedReference<ShipObject*> ship = cast<ShipObject*>( shipControlDevice->getControlledObject());
			//				Reference<CityRegion*> cityRegion = ship->getParkedLocation();
			//				if (cityRegion != NULL) {
			//					insertLong(ship->getObjectID());
			//					String regionString = StringIdManager::instance()->getStringId(cityRegion->getRegionName().hashCode()).toString();
			//					insertAscii(regionString);
			//			}
			//			}
					}
				}
	//		} else {
			//	insertInt(0);
			}
		}

	}
};

#endif /* JTLSHIPLISTRESPONSE_H_ */
