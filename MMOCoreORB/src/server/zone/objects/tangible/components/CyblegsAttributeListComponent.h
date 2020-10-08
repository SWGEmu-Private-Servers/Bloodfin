/*
 *  CyblegsAttribureList.h
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */
#ifndef CYBLEGSATTIBUTELISTCOMPONENT_H_
#define CYBLEGSATTIBUTELISTCOMPONENT_H_

#include "server/zone/objects/scene/components/AttributeListComponent.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/tangible/components/CyblegsDataComponent.h"

class CyblegsAttributeListComponent: public AttributeListComponent {
public:

	/**
	 * Fills the Attributes
	 * @pre { this object is locked }
	 * @post { this object is locked, menuResponse is complete}
	 * @param menuResponse ObjectMenuResponse that will be sent to the client
	 */
	void fillAttributeList(AttributeListMessage* alm, CreatureObject* creature, SceneObject* object) {

		ManagedReference<TangibleObject*> tano = cast<TangibleObject*>(object);

		CyblegsDataComponent* data = cast<CyblegsDataComponent*>(tano->getDataObjectComponent()->get());

		if (data == NULL || !data->isCyblegsData())
			return;

		int charges = data->getCharges();

		alm->insertAttribute("effect", "Cybernetic Leg Reactor Overload");
		alm->insertAttribute("charges", charges);

		if (!creature->checkCooldownRecovery("cybleg_boost")) {
			Time* timeRemaining = creature->getCooldownTime("cybleg_boost");
			alm->insertAttribute("time_remaining", getCooldownString(timeRemaining->miliDifference() * -1));
		}
	}

	String getCooldownString(uint32 delta) {

		int seconds = delta / 1000;

		int hours = seconds / 3600;
		seconds -= hours * 3600;

		int minutes = seconds / 60;
		seconds -= minutes * 60;

		StringBuffer buffer;

		if (hours > 0)
			buffer << hours << "h ";

		if (minutes > 0)
			buffer << minutes << "m ";

		if (seconds > 0)
			buffer << seconds << "s";

		return buffer.toString();
	}

};

#endif /* CYBLEGSATTIBUTELISTCOMPONENT_H_ */
