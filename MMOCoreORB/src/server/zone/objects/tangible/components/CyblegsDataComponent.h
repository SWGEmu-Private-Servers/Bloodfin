/*
 *  CyblegsDataComponent.h
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */

#ifndef CYBLEGSDATACOMPONENT_H_
#define CYBLEGSDATACOMPONENT_H_

#include "server/zone/objects/scene/components/DataObjectComponent.h"

class CyblegsDataComponent : public DataObjectComponent {
protected:
	int charges;
public:
	CyblegsDataComponent() {
		charges = 30;
		addSerializableVariables();
	}

	virtual ~CyblegsDataComponent() {

	}

	void setCharges(int num) {
		charges = num;
	}

	int getCharges() {
		return charges;
	}

	bool isCyblegsData() {
		return true;
	}

private:
	void addSerializableVariables() {
		addSerializableVariable("charges", &charges);
	}
};


#endif /* CYBLEGSDATACOMPONENT_H_ */
