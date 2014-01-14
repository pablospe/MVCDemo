#include "MVCCloner.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

using std::max;
using std::min;

#define PI 3.14159265

/*! \def LUT_SIZE
 The size of trigonometric Look-Up Tables 
 */
#define LUT_SIZE 8192

/*! ACOS_LUT
 Look up table for acos calls
 */
double ACOS_LUT[LUT_SIZE];

/*! TAN_LUT
 Look up table for tan calls
 */
double TAN_LUT[LUT_SIZE];

inline int sign(double d) {
	return (d<0)?(-1):(d>0);
}

inline int closestPowerOf2(int n) {
	int res =1;
	while (res < n) {
		res = res << 1;
	}
	return res>>1;
}   

double* mvc_resize_mem(double* srcPtr, int newSize) {
	double* result = new double[newSize];
	memcpy(result,srcPtr,newSize*sizeof(double));
	delete[] srcPtr;
	return result;
}

int* mvc_resize_mem(int* srcPtr, int newSize) {
	int* result = new int[newSize];
	memcpy(result,srcPtr,newSize*sizeof(int));
	delete[] srcPtr;
	return result;
}   

MVCCloner::MVCCloner() {
	for (int i=0 ; i<LUT_SIZE ; ++i) {
		ACOS_LUT[i] = acos(-1.0+i*2.0/(LUT_SIZE-1));
		TAN_LUT[i] = tan(0+i*(PI/2.0)/(LUT_SIZE-1));
	}     
	
	maxRelevantWeights = 0;
}
CoordinatesMesh* MVCCloner::preprocess(const std::vector<Point>& curve, const CloningParameters& params) {
	std::cout << "MVCCloner: Working on " << curve.size() << " boundary vertices" << std::endl;
	// first step - create the adaptive mesh using CGAL
	std::cout << "MVCCloner: Creating adaptive mesh ";
	if (params.isHierarchic()) {
		std::cout << "with hierarchic sampling ";
	}
	std::cout << "...";
	
	DelaunayTriangulation* adaptiveMesh = new DelaunayTriangulation();
	adaptiveMesh->clear();
	adaptiveMesh->insert(curve.begin(), curve.end());
	for (unsigned int i=0 ; i < curve.size() ; ++i) {
		Point curr = curve[i];
		Point next = curve[(i+1) % curve.size()];
		adaptiveMesh->insert_constraint(curr,next);
	}
	Mesher mesher(*adaptiveMesh);
	mesher.set_criteria(Criteria(params.getAdaptiveMeshShapeCriteria(), params.getAdaptiveMeshSizeCriteria()));
	mesher.refine_mesh();
	std::cout << " Done with " << adaptiveMesh->number_of_vertices() << " vertices and " << adaptiveMesh->number_of_faces()
	<< " triangles." << std::endl;
	// now - calculate the coordinates for each vertex in the mesh
	CoordinatesMesh* mesh = new CoordinatesMesh(adaptiveMesh, curve);
	for (FiniteVerticesIterator iter = adaptiveMesh->finite_vertices_begin(); iter != adaptiveMesh->finite_vertices_end() ; ++iter) {
		Point p = iter->point();
		if (params.isHierarchic()) {
			int maxHierarchicResolution = closestPowerOf2((int)((1.0/params.getMiniminimalHierachicResolution())*curve.size()));
			int maxHierarchicDepth =(int) log((double)maxHierarchicResolution)/log(2.0)+1;
			
			//std::cout << "maxHierarchicDepth: " << maxHierarchicDepth << std::endl;
			HierarchicCoordinateVector* hcv = getCoordinatesHierarchic(curve, p, maxHierarchicResolution, maxHierarchicDepth, 
																	   params.getDistanceThreshold(), params.getAngleThreshold(), params.getAngleExponentBase());
			//mesh->addCoordinateVector(iter->handle(), hcv);
			mesh->vertexToHierarchicCoordinatesMap[iter->handle()] = hcv;
		} else {
			// not using the hierarchy
			double* coords = getCoordinates(curve, p);
			// store the coordinates for p (under curve)
			//mesh->addCoordinateVector(iter->handle(),coords);
			mesh->vertexToCoordinatesMap[iter->handle()] = coords;
		}
	}
	
	return mesh;
}

HierarchicCoordinateVector* MVCCloner::getCoordinatesHierarchic(const std::vector<Point>& curve, const Point& p, 
																const int maxHierarchicResolution, const int maxHierarchicDepth,
																const double distanceThershold, const double angleThreshold, const double angleExpBase)  {
	
	const double EPSILON = 1e-8;	
	int cageSize = curve.size();
	double* coords = new double[cageSize];
	int* indices = new int[cageSize];
	int* depths = new int[cageSize] ;
	bool* handledMask = new bool[cageSize];
	for (int i=0 ; i < cageSize ; ++i) {
		handledMask[i] = false;
	}
	int counter = 0;
	int x = p.x(); int y = p.y();
	
	std::stack<int> indicesToHandle;
	std::stack<int> indicesDepths;
	
	for (int i=0 ; i< cageSize ; i+=maxHierarchicResolution) {
		indicesToHandle.push(i);		
		indicesDepths.push(0);
	}
	
	double coordSum = 0;
	
	while (!indicesToHandle.empty()) {
		
		int index = indicesToHandle.top();	indicesToHandle.pop();
		int depth = indicesDepths.top();	indicesDepths.pop();
		
		if (handledMask[index]) {
			// we can't handle the same vertex twice...
			continue;
		}
		
		int offset = (int) pow(2.0,maxHierarchicDepth - depth);
		int prevIndex = (index - offset);
		if (prevIndex < 0){
			prevIndex += cageSize;
		}
		prevIndex = prevIndex % cageSize;
		int nextIndex = (index + offset);
		if (nextIndex < 0) {
			nextIndex += cageSize;
		}
		nextIndex = nextIndex % cageSize;
		
		int currX = curve[index].x();		int currY = curve[index].y();
		int prevX = curve[prevIndex].x();	int prevY = curve[prevIndex].y();
		int nextX = curve[nextIndex].x();	int nextY = curve[nextIndex].y();
		
		double currLength = max(sqrt(pow(currX-x,2.0)+pow(currY-y,2.0)), EPSILON);
		double prevLength = max(sqrt(pow(prevX-x,2.0)+pow(prevY-y,2.0)), EPSILON);
		double nextLength = max(sqrt(pow(nextX-x,2.0)+pow(nextY-y,2.0)), EPSILON);
		
		int prevdx = prevX-x; int prevdy = prevY - y;
		int currdx = currX-x; int currdy = currY - y;
		int nextdx = nextX-x; int nextdy = nextY - y;
		
		double cosine = ((prevdx*currdx)+(prevdy*currdy))/(currLength*prevLength);
		cosine = max(-1.0, min(cosine, 1.0));
		double prevAngle = ACOS_LUT[(int) ((cosine+1)*((LUT_SIZE-1)/2.0))];
		
		cosine = ((nextdx*currdx)+(nextdy*currdy))/(currLength*nextLength);
		cosine = max(-1.0, min(cosine, 1.0));
		double nextAngle = ACOS_LUT[(int) ((cosine+1)*((LUT_SIZE-1)/2.0))];
		
		int indexDistance = 0;
		if (index > prevIndex) {
			indexDistance = index - prevIndex;
		} else {
			indexDistance = cageSize-prevIndex+index;
		}
		if (index < nextIndex) {
			indexDistance = min (indexDistance, nextIndex - index);
		} else {
			indexDistance = min(indexDistance, cageSize - nextIndex + index);
		}
		
		double lengthThreshold = maxHierarchicResolution/pow(distanceThershold,depth);
		double finalAngleThreshold = angleThreshold*pow(angleExpBase,depth);
		
		if (indexDistance <= 1 || (currLength >= lengthThreshold && prevAngle <= finalAngleThreshold && nextAngle <= finalAngleThreshold)) {
			// no need to subdivide the hierarchy
			handledMask[index] = true;
			double prevTan = TAN_LUT[(int)((prevAngle/2)*((LUT_SIZE-1)/(PI/2)))];
			double nextTan = TAN_LUT[(int)((nextAngle/2)*((LUT_SIZE-1)/(PI/2)))];
			coords[counter] = (1/currLength)*(prevTan+nextTan);			
			indices[counter] = index;
			depths[counter] = depth;			
			coordSum += coords[counter];
			counter++;
			
		} else {		
			// should subdivide more in this area - update the stacks
			if (!handledMask[prevIndex]) {
				indicesToHandle.push(prevIndex);
				indicesDepths.push(depth+1);
			}
			int tempI = (prevIndex+offset/2) % cageSize;
			if (!handledMask[tempI]) {
				indicesToHandle.push(tempI);
				indicesDepths.push(depth+1);
			}
			if (!handledMask[index]) {
				indicesToHandle.push(index);
				indicesDepths.push(depth+1);
			}
			tempI = (nextIndex-offset/2);
			if (tempI < 0) {
				tempI = tempI + cageSize;
			}
			tempI = tempI % cageSize;
			if (!handledMask[tempI]) {
				indicesToHandle.push(tempI);
				indicesDepths.push(depth+1);
			}
			if (!handledMask[nextIndex]) {
				indicesToHandle.push(nextIndex);
				indicesDepths.push(depth+1);
			}
		}
	}
	// normalize the coordinates
	for (int i=0 ; i< counter ; ++i) {
		coords[i] /= coordSum;
	}
	if (counter>maxRelevantWeights)
		maxRelevantWeights = counter;
	
	HierarchicCoordinateVector* result = 
	new HierarchicCoordinateVector(counter, mvc_resize_mem(coords,counter), mvc_resize_mem(indices,counter),mvc_resize_mem(depths,counter));
	
	return result;
}

double* MVCCloner::getCoordinates(const std::vector<Point>& curve, const Point& p) const {
	
	
	
    int cageSize = curve.size();
	
    double* coords = new double[curve.size()];
	
    for (int i=0 ; i< cageSize ; ++i) {
		
        coords[i] = 0;
		
    }
	
	
	
    for (int i=0 ; i<= cageSize-1 ; ++i) {
		
        double p1x = curve[i].x();
		
        double p1y = curve[i].y();
		
        int i2 =  (int) (i+1)%cageSize;
		
        double p2x = curve[i2].x();
		
        double p2y = curve[i2].y();
		
        double dx1 = (p1x-p.x()); double dy1 = (p1y - p.y());
		
        double dx2 = (p2x-p.x()); double dy2 = (p2y - p.y());
		
        double len1 = sqrt((double)dx1*dx1+dy1*dy1);
		
        if (len1 < 1) {
			
            for (int j=0 ; j< cageSize ; ++j) {    coords[j] = 0;    }
			
            coords[i] = 1;
			
            return coords;
			
        }
		
        double len2 = sqrt((double)dx2*dx2+dy2*dy2);
		
        if (len2 < 1) {
			
            for (int j=0 ; j< cageSize ; ++j) {    coords[j] = 0;    }
			
            coords[i2] = 1;
			
            return coords;
			
        }
		
        double cosine = ((dx1*dx2)+(dy1*dy2))/(len1*len2);
		
        cosine = max(-1.0, min(cosine, 1.0));
		
        double tet = acos(cosine);//ACOS_LUT[(int) ((cosine+1)*((LUT_SIZE-1)/2.0))];
		
        double tana2 = tan(tet/2);//TAN_LUT[(int)((tet/2)*((LUT_SIZE-1)/(PI/2)))];
		
        int sgn = sign(dx1*dy2-dy1*dx2);
		
        coords[i] = coords[i] + sgn*tana2/len1;
		
        coords[i2] = coords[i2] + sgn*tana2/len2;
		
    }
	
    return this->normalize(coords, cageSize);
	
}

double* MVCCloner::normalize(double* coords, const int length) const {
	double sum = 0;
	for (int i=0 ; i< length ; ++i) {
		sum += coords[i];
	}
	for (int i=0 ; i< length ; ++i) {
		coords[i] = coords[i]/sum;
	}
	return coords;
}

bool equals(Face& f1, Face& f2) {
    // two faces are equal if they have the same vertices in the same order
    for (int i=0 ; i<3 ; ++i) {
        VertexHandle v1 = f1.vertex(i);
        VertexHandle v2 = f2.vertex(i);
        if (v1->point().x() != v2->point().x()) return false;
        if (v1->point().y() != v2->point().y()) return false;
    }
    return true;
}

void MVCCloner::tagFaces(DelaunayTriangulation* adaptiveMesh) const {
	
    // implement BFS to go over the mesh faces
    std::stack<Face> stack;
    std::set<Face*> visitedFaces; // a set of faces we've already visited, in order to avoid revisiting them
	
    // push all the infinite faces (which are made of the infinite vertex + convex hull edge) to the stack with tag FALSE
    FacesCirculator fc = adaptiveMesh->incident_faces(adaptiveMesh->infinite_vertex());
    FacesCirculator done(fc);
    do {
        fc->set_in_domain(false);
        stack.push(*fc);
        visitedFaces.insert(&(*fc));       
        fc++;
    } while (done != fc);
	
    while (!stack.empty()) {
        Face currFace = stack.top();
        stack.pop();
        visitedFaces.insert(&currFace);
        // now, go over all the incident faces to this face
        for (int i=0 ; i<3 ; ++i) {
            FaceHandle fh = currFace.neighbor(i);
            // if we haven't treated this face already
            if (visitedFaces.find(&(*fh)) == visitedFaces.end()) {
                // no matter what - we should push fh to the stack
                stack.push(*fh);
                visitedFaces.insert(&(*fh));
                // an edge is simply a pair of face+number according to the neighbor
                // if the corresponding edge is constrained - we should flip the tag for fh, else - retain the same tag
				
                // we need to find the correct edge which is actually Edge(currFace,i)
                int j;
                for (j=0 ; j<3 ; ++j) {
                    // for which j is fh.neighbor(j) == currFace
                    if (equals(*(fh->neighbor(j)),currFace)) break;
                }
                assert(j<3);
                if (adaptiveMesh->is_constrained(Edge(fh,j))) {
                    // switch tag for fh
                    fh->set_in_domain(!currFace.is_in_domain());
                } else {
                    fh->set_in_domain(currFace.is_in_domain());
                }
            }
        }
    }
}
