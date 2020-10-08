/*
 * CyblegsMenuComponent.h
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */

#ifndef CYBLEGSMENUCOMPONENT_H_
#define CYBLEGSMENUCOMPONENT_H_

#include "TangibleObjectMenuComponent.h"
#include "server/zone/objects/scene/SceneObject.h"



class CyblegsMenuComponent : public TangibleObjectMenuComponent {
public:

	/**
	 * Fills the radial options, needs to be overriden
	 * @pre { this object is locked }
	 * @post { this object is locked, menuResponse is complete}
	 * @param menuResponse ObjectMenuResponse that will be sent to the client
	 */
	virtual void fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player);

	/**
	 * Handles the radial selection sent by the client, must be overriden by inherited objects
	 * @pre { this object is locked, player is locked }
	 * @post { this object is locked, player is locked }
	 * @param player PlayerCreature that selected the option
	 * @param selectedID selected menu id
	 * @returns 0 if successfull
	 */
	virtual int handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID);

	virtual String getCooldownString(uint32 delta);

};


#endif /* CYBLEGSMENUCOMPONENT_H_ */
