/*! 
 \file 
 \brief TODO!
 \author Gil Hoffer, 2009
 */
#ifndef __MVC_CLONER__H
#define __MVC_CLONER__H

#include "CloningParameters.h"
#include "CoordinatesMesh.h"

/*! \brief MVCCloner: MVC Coordinates for cloning calculator
 */
class MVCCloner {
public:
	
	/*! Constructor: create a new MVCCloner object
	 */
	MVCCloner();
	
	/*! Run the preprocessing stage, and compute the MVC coordinates for the given boundary curve, using the
	 given parameters
	 \param curve a vector containing the points (in 2D) which makes the boundary of the area to clone
	 \param params a CloningParameters object containing the relevant parameters for cloning
	 \return The computed mesh with the coordinates stored in it
	 */
	CoordinatesMesh* preprocess(const std::vector<Point>& curve, const CloningParameters& params);
	
	void tagFaces(DelaunayTriangulation* adaptiveMesh) const;
	
	/*! Virtual d'tor
	 */
	virtual ~MVCCloner() {};
	
	// TODO: Shove it somewhere
	int maxRelevantWeights;	
	
private:
	
	/*! Calculate the MVC coordinates for the point p, using the boundary curve curve, with no hierarchic scheme
	 \param curve a vector containing the points (in 2D) which makes the boundary of the area to clone
	 \param p the point to calculate coordinates to
	 \return an array of size curve.size() containing one MVC coordinate for each vertex in the curve, in the same order
	 as the vertices in the curve
	 */
	double* getCoordinates(const std::vector<Point>& curve, const Point& p) const;
	
	HierarchicCoordinateVector* getCoordinatesHierarchic(const std::vector<Point>& curve, const Point& p, 
														 const int maxHierarchicResolution, const int maxHierarchicDepth,
														 const double distanceThershold, const double angleThreshold, const double angleExpBase);
	
	/*! Normalize the coordinates given in coords to be valid MVC coordinates
	 \param coords the coordinates to normalize - changed in place
	 \param length the number of coordinates in coords
	 \return the nomralized coordinaes (equals to coords)
     */
	double* normalize(double* coords, const int length) const;

};

#endif