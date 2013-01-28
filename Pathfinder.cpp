/*
 * File: Pathfinder.cpp
 * --------------------
 * Name: Casey Davis
 * Section: Robert Hintz
 * This file is the starter project for the Pathfinder application
 * on Assignment #6.
 */

#include <iostream>
#include <string>
#include "console.h"
#include "graphtypes.h"
#include "gpathfinder.h"
#include "tokenscanner.h"
#include "strlib.h"
#include "filelib.h"
#include "pqueue.h"
#include "path.h"
using namespace std;

/* Function prototypes */
void quitAction();
void mapAction(PathfinderGraph &thisGraph);
void appendNode(string name, GPoint nodeLoc, PathfinderGraph &thisGraph);
void appendArc(string start, string finish, double cost, PathfinderGraph &thisGraph);
void graphTest(PathfinderGraph &thisGraph);
void goForGraphics(PathfinderGraph &thisGraph); 
void highlightPath(Set<Node*> nodes, string color);
void highlightArcs(Set<Arc* > arcs);
void dijkstra(PathfinderGraph &thisGraph);
void kruskal(PathfinderGraph &thisGraph);
Set<Arc* > minSpanningTree(PathfinderGraph &thisGraph);
Path findShortestPath(Node* start, Node* finish);
Node* selectNode(PathfinderGraph &thisPath, GPoint point);
void userPathSelect(PathfinderGraph &thisPath, Node* &startNode, Node* &finishNode);

/* Main program */
int main() {
    PathfinderGraph thisGraph;
    
    initPathfinderGraphics();
    
    addButton("Map", mapAction, thisGraph);
    addButton("Dijkstra", dijkstra, thisGraph);
    addButton("Kruskal", kruskal, thisGraph);
    addButton("Quit", quitAction);
    //addButton("TestGraph", graphTest, thisGraph);
    
    mapAction(thisGraph);
    pathfinderEventLoop();

    return 0;
}

/* Exit action */
void quitAction() {
   exit(0);
}

/* Callback function for importing a map file,
 * parsing the file and displaying the map 
 */
void mapAction(PathfinderGraph &thisGraph) {
    thisGraph.clear();
    
    string mapString;
    cout << "Please enter the name of a map file: ";
    getline(cin, mapString);
    ifstream infile(mapString.c_str());
    while (!infile.good()) {
        cout << "Enter a valid file: ";
        getline(cin, mapString);
        infile.clear();
        infile.open(mapString.c_str());
    }
    
    
    string currentLine, token;
    TokenScanner scanner;
    scanner.ignoreWhitespace();  
    scanner.scanNumbers();
    getline(infile, currentLine);
    string imageString = currentLine;
    thisGraph.getImage(imageString);
    getline(infile, currentLine);
    getline(infile, currentLine);
    
    while (currentLine != "ARCS") {
        scanner.setInput(currentLine);
        string node = scanner.nextToken();
        int xcoord = stringToInteger(scanner.nextToken());
        int ycoord = stringToInteger(scanner.nextToken());
        getline(infile, currentLine);
        
        GPoint nodeLoc(xcoord, ycoord);
        appendNode(node, nodeLoc, thisGraph);
        
    }
    
    getline(infile, currentLine);
    while (!infile.fail()) {
        scanner.setInput(currentLine);
        string node1 = scanner.nextToken();
        string node2 = scanner.nextToken();
        double dist = stringToReal(scanner.nextToken());
        getline(infile, currentLine);
        
        appendArc(node1, node2, dist, thisGraph);
    }
    
    goForGraphics(thisGraph);
}

/* Function to add a node to a PathfinderGraph
 */
void appendNode(string name, GPoint nodeLoc, PathfinderGraph &thisGraph) {
    Node* node = new Node;
    node->name = name;
    node->loc = nodeLoc;
    thisGraph.addNode(node);
}

/* Function for adding an arc to a PathfinderGraph 
 */
void appendArc(string start, string finish, double cost, PathfinderGraph &thisGraph) {
    Arc* arc1 = new Arc;
    Arc* arc2 = new Arc;
    arc1->cost = cost;
    arc2->cost = cost;
    Node* startNode = thisGraph.getNode(start);
    Node* finishNode = thisGraph.getNode(finish);
    arc1->start = startNode;
    arc1->finish = finishNode;
    arc2->start = finishNode;
    arc2->finish = startNode;
    thisGraph.addArc(arc1);
    thisGraph.addArc(arc2);
}


/* Function for testing graph subclass PathfinderGraph
 * Is implemented as a callback function for use with
 * a button.
 */
void graphTest(PathfinderGraph &thisGraph) {
    cout << endl << endl;
    foreach (Node *node in thisGraph.getNodeSet()){
        cout << "Name: " << node->name << " \tLocation\tx: " << node->loc.getX() << "\ty: " << node->loc.getY() << endl;
    }
    cout << endl << endl;
    foreach (Arc *arc in thisGraph.getArcSet()){
        cout << "Start: " << arc->start->name << " \tFinish: " << arc->finish->name << "\tcost: " << arc->cost << endl;
    }
    cout << endl << endl;
    cout << "Graph image: " << thisGraph.getImageFileName() << endl;
}

/* A function for refreshing the current graph on the
 * screen.
 */
void goForGraphics(PathfinderGraph &thisGraph) {
    drawPathfinderMap(thisGraph.getImageFileName());
    foreach (Node *node in thisGraph.getNodeSet()){
        drawPathfinderNode(node->loc, NODE_COLOR, node->name);
    }
    foreach (Arc *arc in thisGraph.getArcSet()){
        drawPathfinderArc(arc->start->loc, arc->finish->loc, ARC_COLOR);
    }
    repaintPathfinderDisplay();
    
}

/* Highlights the passed path on the graphics window.
 */
void highlightPath(Path path) {
    foreach(Arc *arc in path.getArcs()) {
        drawPathfinderNode(arc->start->loc, HIGHLIGHT_COLOR, arc->start->name);
        drawPathfinderNode(arc->finish->loc, HIGHLIGHT_COLOR, arc->finish->name);
        drawPathfinderArc(arc->start->loc, arc->finish->loc, HIGHLIGHT_COLOR);
    }
    repaintPathfinderDisplay();
}

/* Highlights the arc set on the graphics window.
 */
void highlightArcs(Set<Arc* > arcs) {
    foreach(Arc *arc in arcs) {
        drawPathfinderNode(arc->start->loc, HIGHLIGHT_COLOR, arc->start->name);
        drawPathfinderNode(arc->finish->loc, HIGHLIGHT_COLOR, arc->finish->name);
        drawPathfinderArc(arc->start->loc, arc->finish->loc, HIGHLIGHT_COLOR);
    }
    repaintPathfinderDisplay();
}

/* Callback function for dijkstra
 */
void dijkstra(PathfinderGraph &thisGraph) {
    goForGraphics(thisGraph);
    Node* startNode = NULL;
    Node* finishNode = NULL;
    userPathSelect(thisGraph, startNode, finishNode);
    Path path = findShortestPath(startNode, finishNode);
    highlightPath(path);
}

/* Callback function for kruskal
 */
void kruskal(PathfinderGraph &thisGraph) {
    goForGraphics(thisGraph);
    Set<Arc* > mst = minSpanningTree(thisGraph);
    PathfinderGraph blankGraph;
    blankGraph.getImage(thisGraph.getImageFileName());
    goForGraphics(blankGraph);
    highlightArcs(mst);
}

/* The implementation of Kruskal's algorithm for
 * finding a minimum spanning tree. Adapted for 
 * the data structure used in this project.
 */
Set<Arc* > minSpanningTree(PathfinderGraph &thisGraph) {
    Set<Arc* > mst;
    Map<string , Set<string >* > nodeStar;
    foreach(Node* node in thisGraph.getNodeSet()) {
        Set<string >* strSet = new Set<string>;
        strSet->add(node->name);
        nodeStar[node->name] = strSet;
    }
    PriorityQueue<Arc *> arcs;
    foreach(Arc* arc in thisGraph.getArcSet()) {
        arcs.enqueue(arc, arc->cost);
    }
    while (!arcs.isEmpty()) {
        Arc* thisArc = arcs.dequeue();
        if (!nodeStar[thisArc->start->name]->contains(thisArc->finish->name)) {
            *nodeStar[thisArc->start->name] += *nodeStar[thisArc->finish->name];
            foreach(string str in *nodeStar[thisArc->finish->name]) {
                if (nodeStar[str] == nodeStar[thisArc->finish->name] && (str != thisArc->finish->name)) {
                    nodeStar[str] = nodeStar[thisArc->start->name];
                }
            }
            nodeStar[thisArc->finish->name] = nodeStar[thisArc->start->name];
            mst.add(thisArc);
            cout << thisArc->cost << ": " << thisArc->start->name << " - " << thisArc->finish->name << endl;
        }else {
            cout << thisArc->cost << ": " << thisArc->start->name << " - " << thisArc->finish->name << " (ignored)" << endl;
        }
    }
    return mst;
}

/* The implementation of Dijkstra's algorithm for
 * finding the shortest path between two nodes.
 */
Path findShortestPath(Node *start, Node *finish) {
    Path path;
    PriorityQueue<Path > queue;
    Map<string, double> fixed;
    while (start != finish) {
        if (!fixed.containsKey(start->name)) {
            fixed.put(start->name, path.getCost());
            cout << "Fix cost to " << start->name << " at " << path.getCost() << endl;
            foreach (Arc *arc in start->arcs) {
                if (!fixed.containsKey(arc->finish->name)) {
                    Path updatedPath = path;
                    updatedPath.appendArc(arc);
                    queue.enqueue(updatedPath, updatedPath.getCost());
                    cout << "   Enqueue path: " << updatedPath.getPathString() << " (" << updatedPath.getCost() << ")" << endl;
                } else {
                    cout << "   Ignore " << arc->finish->name << " because its distance is fixed." << endl;
                }
            }
        } else {
            cout << "Ignore " << start->name << " because its distance is fixed." << endl;
        }
        if (queue.isEmpty()) {
            return Path();
        }
        path = queue.dequeue();
        cout << "Dequeue shortest path: " << path.getPathString() << " (" << path.getCost() << ")" << endl;
        start = path.getArc(path.size()-1)->finish;

    }
    return path;
}

/* Function for testing if a mouse click was within a box
 * of size 2*NODE_RADIUS pixels around any node and returning
 * that node if so, NULL otherwise.
 */
Node* selectNode(PathfinderGraph &thisPath, GPoint point) {
	foreach (Node* node in thisPath.getNodeSet())
	{
		if (point.getX() > (node->loc.getX() - NODE_RADIUS) && 
			point.getX() < (node->loc.getX() + NODE_RADIUS) &&
            point.getY() > (node->loc.getY() - NODE_RADIUS) &&
            point.getY() < (node->loc.getY() + NODE_RADIUS)) {
            return node;
        }
	}
	return NULL;
}

/* Function for getting two unique node selections from user.
 * For use with Dijkstra's algorithm
 */
void userPathSelect(PathfinderGraph &thisPath, Node* &startNode, Node* &finishNode) {
    
	GPoint click1;
    GPoint click2;
    
    cout << "Select a starting location." << endl;
    click1 = getMouseClick();
    while ((startNode = selectNode(thisPath, click1)) == NULL){
        cout << "Invalid selection; Select a location." << endl;
        click1 = getMouseClick();
    }
    cout << "Atta boy." << endl;
    cout << "Start: " << startNode->name << endl;
    drawPathfinderNode(startNode->loc, HIGHLIGHT_COLOR, startNode->name);
    
    cout << "Select a destination." << endl;
    click2 = getMouseClick();
    while ((finishNode = selectNode(thisPath, click2)) == NULL || (selectNode(thisPath, click2) == selectNode(thisPath, click1))) {
        cout << "Invalid selection; Select a location." << endl;
        click2 = getMouseClick();
    }
    cout << "Destination: " << finishNode->name << endl;
    drawPathfinderNode(finishNode->loc, HIGHLIGHT_COLOR, finishNode->name);
	
}