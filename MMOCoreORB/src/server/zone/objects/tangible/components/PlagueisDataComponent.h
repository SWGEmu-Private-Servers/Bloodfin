
#ifndef PLAGUEISDATACOMPONENT_H_
#define PLAGUEISDATACOMPONENT_H_

#include "server/zone/objects/scene/components/DataObjectComponent.h"

class PlagueisDataComponent : public DataObjectComponent {
protected:
	int charges;
public:
	PlagueisDataComponent() {
		charges = 30;
		addSerializableVariables();
	}

	virtual ~PlagueisDataComponent() {

	}

	void setCharges(int num) {
		charges = num;
	}

	int getCharges() {
		return charges;
	}

	bool isPlagueisData() {
		return true;
	}

private:
	void addSerializableVariables() {
		addSerializableVariable("charges", &charges);
	}
};


#endif /* PLAGUEISDATACOMPONENT_H_ */
