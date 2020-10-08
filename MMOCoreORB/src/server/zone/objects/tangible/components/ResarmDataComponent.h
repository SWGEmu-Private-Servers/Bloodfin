/*
 *  ResarmDataComponent.h
 *
 *  Created on: 08/19/2015
 *      Author: Heat
 */

#ifndef RESARMDATACOMPONENT_H_
#define RESARMDATACOMPONENT_H_

#include "server/zone/objects/scene/components/DataObjectComponent.h"

class ResarmDataComponent : public DataObjectComponent {
protected:
	int charges;
public:
	ResarmDataComponent() {
		charges = 30;
		addSerializableVariables();
	}

	virtual ~ResarmDataComponent() {

	}

	void setCharges(int num) {
		charges = num;
	}

	int getCharges() {
		return charges;
	}

	bool isResarmData() {
		return true;
	}

private:
	void addSerializableVariables() {
		addSerializableVariable("charges", &charges);
	}
};


#endif /* RESARMDATACOMPONENT_H_ */
