#ifndef __CLONING_PARAMETERS__H
#define __CLONING_PARAMETERS__H


class CloningParameters {
public:
    CloningParameters() : hierarchic(false),
                          angleThreshold(0.75),
                          angleExponentBase(0.8),
                          distanceThreshold(2.5),
                          adaptiveMeshShapeCriteria(0.125),
                          adaptiveMeshSizeCriteria(0),
                          minimalHierarchicResolution(16) {};
	
	void setHierarchic(const bool isHierarchic=true) {
		this->hierarchic = isHierarchic;
	}
	
	void setAngleThreshold(const double thresh) {
		this->angleThreshold = thresh;
	}
	
	void setDistanceThreshold(const double thresh) {
		this->distanceThreshold = thresh;
	}
	
	void setAdaptiveMeshShapeCriteria(const double criteria) {
		this->adaptiveMeshShapeCriteria = criteria;
	}
	
	void setAdaptiveMeshSizeCriteria(const double criteria) {
		this->adaptiveMeshSizeCriteria = criteria;
	}
	
	void setMinimalHierarchicResolution(const int resolution) {
		this->minimalHierarchicResolution = resolution;
	}
	
	void setAngleExponentBase(const double expBase) {
		this->angleExponentBase = expBase;
	}
	
	bool isHierarchic() const {return this->hierarchic;}
	double getAngleThreshold() const {return this->angleThreshold;}
	double getDistanceThreshold() const {return this->distanceThreshold;}
	double getAdaptiveMeshShapeCriteria() const {return this->adaptiveMeshShapeCriteria;}
	double getAdaptiveMeshSizeCriteria() const {return this->adaptiveMeshSizeCriteria;}
	double getAngleExponentBase() const {return this->angleExponentBase;}
	int getMiniminimalHierachicResolution() const {return this->minimalHierarchicResolution;}
	
	virtual ~CloningParameters() {}
	
private:
	bool hierarchic;
	double angleThreshold;
	double angleExponentBase;
	double distanceThreshold;
	double adaptiveMeshShapeCriteria;
	double adaptiveMeshSizeCriteria;
	int minimalHierarchicResolution;
};

#endif
