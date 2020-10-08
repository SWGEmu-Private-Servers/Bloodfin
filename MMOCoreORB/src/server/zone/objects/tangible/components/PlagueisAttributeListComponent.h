
#ifndef PLAGUEISATTIBUTELISTCOMPONENT_H_
#define PLAGUEISATTIBUTELISTCOMPONENT_H_

#include "server/zone/objects/scene/components/AttributeListComponent.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/tangible/components/PlagueisDataComponent.h"

class PlagueisAttributeListComponent: public AttributeListComponent {
public:

	/**
	 * Fills the Attributes
	 * @pre { this object is locked }
	 * @post { this object is locked, menuResponse is complete}
	 * @param menuResponse ObjectMenuResponse that will be sent to the client
	 */
	void fillAttributeList(AttributeListMessage* alm, CreatureObject* creature, SceneObject* object) {

		ManagedReference<TangibleObject*> tano = cast<TangibleObject*>(object);

		PlagueisDataComponent* data = cast<PlagueisDataComponent*>(tano->getDataObjectComponent()->get());

		if (data == NULL || !data->isPlagueisData())
			return;

		int charges = data->getCharges();

		alm->insertAttribute("effect", "Darth Plagueis Infusion of Life");
		alm->insertAttribute("charges", charges);

		if (!creature->checkCooldownRecovery("darth_plagueis")) {
			Time* timeRemaining = creature->getCooldownTime("darth_plagueis");
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

#endif /* HERORINGATTIBUTELISTCOMPONENT_H_ */
