/*
 * File: path.cpp
 * --------------
 * Name: Casey Davis
 * Section: Robert Hintz
 * This file implements the path.h interface.
 * -------------------
 * The path.cpp file defines the interface for a path class that can
 * be used to represent connected arcs in a graph. The underlying
 * structure is a Vector of Arc pointers. By definition a path should
 * have elements (Arc*) such that the 'finish' node in the n-1 element
 * is the same as the 'start' node in the nth element. This is reflected
 * in how path strings are generated. Addition of non-contiguous arcs
 * is not checked however, as their is no reference made to any graph
 * structure.
 *
 * The underlying structure, Vector<Arc* >, used to store the path is 
 * advantageous because it allows fast (O(1)) retrieval of elements, 
 * addition of elements, and retrieval of size. It's destruction is 
 * done simply by a call to the Vector::clear method. 
 *
 * We achieve retrieval of the cost of a path in constant time by 
 * updating a private cost variable as new arcs are appended to the 
 * path. We do the same with path string. The path elements are 
 * pointers to Arcs, a structure imported from graphtypes.h. The Arc
 * structure associates a cost with each Arc, which is used to generate
 * the path cost.
 */

#include "path.h"

/*
 * Path()
 * -----------------
 * The constructor for the path class.
 * Initializes the cost to 0 and the path string
 * to the empty string.
 */
Path::Path() {
    cost = 0;
    pathString = "";
}


/*
 * ~Path()
 * -----------------
 * class destructor
 * frees the memory used by the underlying vector structure
 * runs in O(n)
 */
Path::~Path() {
    arcs.clear();
}


/*
 * appendArc(Arc* arc)
 * -----------------
 * Adds a new arc to the path
 * Simultaneously updates the path string and
 * the path cost
 * runs in O(1)
 */
void Path::appendArc(Arc* arc) {
    arcs.push_back(arc);
    cost += arc->cost;
    if (size() == 1) {
        pathString = arc->start->name + " -> " + arc->finish->name;
    } else {
        pathString = pathString + " -> " + arc->finish->name;
    }
}


/*
 * getArc(int index)
 * -----------------
 * Returns the Arc* element at the specified index.
 * runs in O(1)
 */
Arc* Path::getArc(int index) {
    return arcs[index];
}

/*
 * getArcs()
 * -----------------
 * Returns a copy of the underlying Vector of *Arc elements
 */
Vector<Arc* > Path::getArcs() {
    Vector<Arc* > arcVec;
    foreach(Arc* arc in arcs) {
        arcVec.push_back(arc);
    }
    return arcVec;
}


/*
 * getCost()
 * -----------------
 * Returns the path cost
 * runs in O(1)
 */
double Path::getCost() {
    return cost;
}

/*
 * size()
 * -----------------
 * Returns the path size
 * runs in O(1)
 */
int Path::size() {
    return arcs.size();
}

/*
 * getPathString
 * -----------------
 * Returns the path string
 * runs in O(1)
 */
string Path::getPathString() {
    return pathString;
}