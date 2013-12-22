#ifndef __COORDINATES__MESH__H
#define __COORDINATES__MESH__H

#include <map>

// CGAL includes for triangulation
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>

// CGAL typedefs and definitions
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<Vb> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> DelaunayTriangulation;

typedef CGAL::Delaunay_mesh_size_criteria_2<DelaunayTriangulation> Criteria;

typedef DelaunayTriangulation::Finite_vertices_iterator FiniteVerticesIterator;
typedef DelaunayTriangulation::Finite_faces_iterator FiniteFacesIterator;
typedef DelaunayTriangulation::Finite_edges_iterator FiniteEdgesIterator;
typedef DelaunayTriangulation::Face_circulator FacesCirculator;
typedef DelaunayTriangulation::Point_iterator PointIterator;
typedef DelaunayTriangulation::Vertex Vertex;
typedef DelaunayTriangulation::Vertex_handle VertexHandle;
typedef DelaunayTriangulation::Edge Edge;
typedef DelaunayTriangulation::Face Face;
typedef DelaunayTriangulation::Face_handle FaceHandle;
typedef DelaunayTriangulation::Triangle Triangle;

typedef CGAL::Cartesian<double> Metric;
typedef DelaunayTriangulation::Point Point;
typedef CGAL::Polygon_2<Metric> Polygon2D;
typedef CGAL::Bbox_2 BBox2D;

typedef CGAL::Delaunay_mesher_2<DelaunayTriangulation, Criteria> Mesher;

class HierarchicCoordinateVector {
public:
	HierarchicCoordinateVector(int _size, double* _coords, int* _indices, int* _depths) : 
	size(_size), coords(_coords), indices(_indices), depths(_depths) {}
	int getSize() const {return this->size;}
	double* getCoords() const {return this->coords;}
	int* getIndices() const {return this->indices;}
	int* getDepths() const {return this->depths;}
	virtual ~HierarchicCoordinateVector() { 
		delete[] coords;
		delete[] indices;
		delete[] depths;
	}
	
private:
	int size;
	double* coords;
	int* indices;
	int* depths;
};
/*! \brief CoordinatesMesh: This class holds a refined DelaunayTriangulation and a map
 from each vertex in it to its coordinates (as double*)
 */
class CoordinatesMesh {
public:
	
	/*! Create a new mesh, with no attached coordinates		
	 \param t the triangulation which makes this mehs
	 \param boundaryCurve the boundary curve of the mesh (the actual user selection curve)
	 */
	CoordinatesMesh(DelaunayTriangulation* t, const std::vector<Point>& boundaryCurve) : mesh(t), boundary(boundaryCurve) {}
	
	/*! Get a reference to the internl delaunay triangulation (from CGAL) data structure
	 */
	DelaunayTriangulation* getMesh() {
		return this->mesh;
	}
	/*! Add a coordinate array to a vertex from the mesh
	 \param v the vertex-handle of the vertex we'd like to set coordiantes for
	 \param coords a double array with the corresponding coordinates
	 */
	void addCoordinateVector(VertexHandle& v, double* coords) {this->vertexToCoordinatesMap[v] = coords;}
	
	/*!
	 \return a const iterator to the begining of the vertex:coordinates map
	 */
	std::map<VertexHandle,double*>::const_iterator beginVertexToCoordinates() const {return this->vertexToCoordinatesMap.begin();}
	/*!
	 \return a const iterator to the end of the vertex:coordinates map
	 */
	std::map<VertexHandle,double*>::const_iterator endVertexToCoordinates() const {return this->vertexToCoordinatesMap.end();}
	
	void addCoordinateVector(VertexHandle& v, HierarchicCoordinateVector* hcv) {this->vertexToHierarchicCoordinatesMap[v] = hcv;}
	/*!
	 \return a const iterator to the begining of the vertex:coordinates map
	 */
	std::map<VertexHandle,HierarchicCoordinateVector*>::const_iterator beginVertexToHierarchicCoordinates() const {
		return this->vertexToHierarchicCoordinatesMap.begin();
	}
	/*!
	 \return a const iterator to the end of the vertex:coordinates map
	 */
	std::map<VertexHandle,HierarchicCoordinateVector*>::const_iterator endVertexToHierarchicCoordinates() const {
		return this->vertexToHierarchicCoordinatesMap.end();
	}
	
	/*!
	 \return the boundary curve with the vertices which define the boundary of the mesh
	 */
	std::vector<Point>& getBoundary() {return boundary;}
	
	virtual ~CoordinatesMesh() {
		for (std::map<VertexHandle,double*>::const_iterator iter = beginVertexToCoordinates() ; iter != endVertexToCoordinates(); ++iter) {
			delete[] iter->second;			
		}
		for (std::map<VertexHandle,HierarchicCoordinateVector*>::const_iterator iter = beginVertexToHierarchicCoordinates() ; 
			 iter != endVertexToHierarchicCoordinates(); ++iter) {
			HierarchicCoordinateVector* hcv = iter->second;
			delete[] hcv->getCoords();
			delete[] hcv->getDepths();
			delete[] hcv->getIndices();
			delete hcv;
		}
	}
	
public:
	DelaunayTriangulation* mesh;
	std::map<VertexHandle,double*> vertexToCoordinatesMap;
	std::map<VertexHandle, HierarchicCoordinateVector*> vertexToHierarchicCoordinatesMap;
private:	
	std::vector<Point> boundary;
};

#endif