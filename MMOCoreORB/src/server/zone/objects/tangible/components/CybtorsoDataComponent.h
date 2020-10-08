/*
 *  CybtorsoDataComponent.h
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */

#ifndef CYBERTORSODATACOMPONENT_H_
#define CYBERTORSODATACOMPONENT_H_

#include "server/zone/objects/scene/components/DataObjectComponent.h"

class CybtorsoDataComponent : public DataObjectComponent {
protected:
	int charges;
public:
	CybtorsoDataComponent() {
		charges = 30;
		addSerializableVariables();
	}

	virtual ~CybtorsoDataComponent() {

	}

	void setCharges(int num) {
		charges = num;
	}

	int getCharges() {
		return charges;
	}

	bool isCybtorsoData() {
		return true;
	}

private:
	void addSerializableVariables() {
		addSerializableVariable("charges", &charges);
	}
};


#endif /* CYBERTORSODATACOMPONENT_H_ */
