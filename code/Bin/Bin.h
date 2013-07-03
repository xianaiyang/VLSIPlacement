# ifndef BIN_H
# define BIN_H

class Bin {
 private:
  uint binIdx;
  uint left, right, bot, top;
  double newRight, newTop;
  double utilization;
  double cellArea;
  double totalCellWidth;
  double averageCellWidth;
  bool stretchedInX, stretchedInY;
  /* Vector to store list of all cells that are cut by the 
     bin or completely encompassed in the bin */
  vector<Cell *> cellsOfBin;

 public:
  /* Constructors */
  Bin();
  Bin(uint);
  Bin(uint, uint, uint, uint, uint);
  Bin(uint, uint, uint, uint, uint, vector<Cell *> &);

  /* Get functions */
  void BinGetBoundingBox(uint&, uint&, uint&, uint&);
  uint BinGetIdx(void);
  uint BinGetLeft(void);
  uint BinGetRight(void);
  uint BinGetTop(void);
  uint BinGetBot(void);
  double BinGetNewRight(void);
  double BinGetNewTop(void);
  double BinGetUtilization(void);
  double BinGetCellArea(void);
  double BinGetAverageCellWidth(void);
  bool BinStretchInX(void);
  bool BinStretchInY(void);
  vector<Cell *>& BinGetCells(void);

  /* Set functions */
  void BinSetBoundingBox(uint, uint, uint, uint);
  void BinSetCellArea(double);
  void BinAddCellArea(double);
  void BinSetTotalCellWidth(double);
  void BinAddCellWidth(double);
  void BinComputeAverageCellWidth(void);
  void BinComputeUtilization(void);
  void BinAddCells(vector<Cell *>&);
  void BinAddCell(Cell *);
  void BinAddCellVirtual(Cell *);
  void BinRemoveCell(uint);
  void BinSetCells(vector<Cell *>&);
  void BinSetRight(uint);
  void BinSetTop(uint);
  void BinSetNewRight(double);
  void BinSetNewTop(double);
  void BinDeleteData(void);

  /* Debug functions */
  void BinPrintBin(void);

  ~Bin();
};

# endif
