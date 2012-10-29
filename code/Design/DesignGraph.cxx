/****** 
FUNCTIONS TO CONVERT THE DESIGN THAT HAS 
BEEN READ TO A HYPERGRAPH 
******/
# include <Design.h>
# include <DesignIter.h>

/**************/
void DesignCreateGraph(Design& myDesign, HyperGraph& thisGraph)
{
  vector<void*> listOfCells;
  Cell *CellPtr;
  Net *NetPtr;
  Pin *PinPtr;
  string Name;
  /* Iterate over all hyperedges and use them to create
     a hypergraph */
  DESIGN_FOR_ALL_NETS(myDesign, Name, NetPtr) {
    NET_FOR_ALL_CELLS((*NetPtr), CellPtr) {
      thisGraph.AddNode((void *)(CellPtr));
      listOfCells.insert(listOfCells.end(), (void *)CellPtr);
    } NET_END_FOR;
    thisGraph.AddEdge(listOfCells, (void *)NetPtr);
    listOfCells.clear();
  } DESIGN_END_FOR;
}

