# include <Design.h>

bool moveDebug = false;

using namespace std;

bool cmpCellLeftPos(Cell *cellPtri, Cell *cellPtrj)
{
  uint lefti = (*cellPtri).CellGetXpos();
  uint leftj = (*cellPtrj).CellGetXpos();
  return (lefti < leftj);
}

bool cmpCellRightPos(Cell *cellPtri, Cell *cellPtrj)
{
  return ((*cellPtri).CellGetRight() < (*cellPtrj).CellGetRight());
}

bool cmpCellBotPos(Cell *cellPtri, Cell *cellPtrj)
{
  uint boti = (*cellPtri).CellGetYpos();
  uint botj = (*cellPtrj).CellGetYpos();
  return (boti < botj);
}

map<string,Net*>& Design::DesignGetNets(void)
{
  map<string, Net*>& retVal = this->DesignNets;

  return (retVal);
}

map<string,Cell*>& Design::DesignGetCells(void)
{
  map<string, Cell*>& retVal = this->DesignCells;

  return (retVal);
}

map<string,Cell*>& Design::DesignGetClusters(void)
{
  map<string, Cell*>& retVal = this->DesignClusters;

  return (retVal);
}

vector<PhysRow*>& Design::DesignGetRows(void)
{
  vector<PhysRow*>& retVal = this->DesignPhysRows;

  return (retVal);
}

vector<Bin*>& Design::DesignGetBins(void)
{
  vector<Bin*>& retVal = this->DesignBins;

  return (retVal);
}

vector<Path*>& Design::DesignGetPaths(void)
{
  vector<Path*>& retVal = this->DesignPaths;

  return (retVal);
}

Cell *
Design::DesignGetNode(string nodeName)
{
  Cell *foundNode;
  
  foundNode = NIL(Cell *);
  _KEY_EXISTS(DesignCells, nodeName) {
    foundNode = DesignCells[nodeName];
  }
  if (foundNode == NIL(Cell *)) {
    _KEY_EXISTS(DesignClusters, nodeName) {
      foundNode = DesignClusters[nodeName];
    }
  }
  
  return (foundNode);
}

string
Design::DesignGetName(void)
{
  return (this->Name);
}

map<unsigned int, unsigned int> 
Design::DesignGetRowHeights()
{
  unsigned int height;
  map<unsigned int, unsigned int> retVal;
  
  MAP_FOR_ALL_KEYS(RowHeights, unsigned int, unsigned int, height) {
    retVal[height] = 1;
  } END_FOR;
  
  return retVal;
}

void
Design::DesignAddOneCellToDesignDB(Cell *newCell)
{
  if (!DesignCellIsStdCell((*this),*newCell)) {
    (*newCell).CellSetIsMacro(true);
    NumMacroCells++;
  } else {
    this->averageStdCellWidth += (*newCell).CellGetWidth();
    this->averageStdCellHeight += (*newCell).CellGetHeight();
    NumStdCells++;
  }
  DesignCells[(*newCell).CellGetName()] = newCell;
  this->NumCells++;
}

void
Design::DesignAddOneClusterToDesignDB(Cell *clusterCell)
{
  string cellName;
  
  cellName = (*clusterCell).CellGetName();

  _KEY_DOES_NOT_EXIST(DesignClusters, cellName) {
    DesignClusters[cellName] = clusterCell;
    this->NumClusters++;
  }
}

void
Design::DesignAddOneNetToDesignDB(Net *newNet, double Weight)
{
  double actualWeight;
  
  actualWeight = Weight / ((*newNet).NetGetPinCount() - 1);
  (*newNet).NetSetWeight(actualWeight);
  DesignNets[(*newNet).NetGetName()] = newNet;
  this->NumNets++;
}

void
Design::DesignAddPseudoNet(Net *pseudoNet)
{
  PseudoNets.push_back(pseudoNet);
}

void
Design::DesignAddOnePhysRowToDesignDB(PhysRow *row)
{
  unsigned int RowHeight;
  
  RowHeight = (*row).PhysRowGetHeight();

  DesignPhysRows.push_back(row);
  DesignUpdateChipDim(row);

  if (RowHeights.find(RowHeight) == RowHeights.end()) {
    RowHeights[RowHeight] = 1;
    if (singleRowHeight == -1) {
      singleRowHeight = RowHeight;
    } else {
      singleRowHeight = -1;
    }
  } 
  
  this->NumPhysRows++;
}

void
Design::DesignRemoveOneClusterFromDesignDB(Cell *clusterCell)
{
  string clusterCellName = (*clusterCell).CellGetName();

  _KEY_EXISTS(DesignClusters, clusterCellName) {
    DesignClusters.erase(clusterCellName);
    this->NumClusters--;
  }
}

void
Design::DesignDeleteCell(Cell *cellPtr)
{
  delete cellPtr;
}

void
Design::DesignDeletePin(Pin *pinPtr)
{
  delete pinPtr;
}

void
Design::DesignSetPeakUtil(double maxUtil)
{
  this->peakUtilization = maxUtil;
}

void
Design::DesignSetMaxUtil(double maxUtil)
{
  if (this->maxUtilization == 0) {
    maxUtilization = maxUtil;
  }
}

void
Design::DesignSetPeakUtilBinIdx(uint peakUtilBinIdx)
{
  this->peakUtilizationBinIdx = peakUtilBinIdx;
}

void
Design::DesignSetNumBinRows(uint numRows)
{
  this->numBinRows = numRows;
}

void
Design::DesignSetNumBinCols(uint numCols)
{
  this->numBinCols = numCols;
}

uint
Design::DesignGetNumBinRows(void)
{
  return (this->numBinRows);
}

uint
Design::DesignGetNumBinCols(void)
{
  return (this->numBinCols);
}

void
Design::DesignSetHPWL(ulong designHPWL)
{
  this->DesignHPWL = designHPWL;
}

void 
Design::DesignRemoveFromHPWL(ulong removeHPWL)
{
  this->DesignHPWL -= removeHPWL;
}

void 
Design::DesignAddToHPWL(ulong addHPWL)
{
  this->DesignHPWL += addHPWL;
}

void
Design::DesignComputeHPWL()
{
  Net *netPtr;
  string netName;
  ulong totalXHPWL, totalYHPWL;
  ulong totalHPWL;
  uint xHPWL, yHPWL;
  
  totalXHPWL = 0.0;
  totalYHPWL = 0.0;
  totalHPWL = 0.0;
  DESIGN_FOR_ALL_NETS((*this), netName, netPtr) {
    (*netPtr).NetComputeHPWL(xHPWL, yHPWL);
    totalXHPWL += xHPWL;
    totalYHPWL += yHPWL;
    totalHPWL += (xHPWL + yHPWL);
  } DESIGN_END_FOR;
  this->DesignXHPWL = totalXHPWL;
  this->DesignYHPWL = totalYHPWL;
  this->DesignHPWL = totalHPWL;
}

ulong 
Design::DesignGetHPWL(void)
{
  return (this->DesignHPWL);
}

ulong 
Design::DesignGetXHPWL(void)
{
  return (this->DesignXHPWL);
}

ulong 
Design::DesignGetYHPWL(void)
{
  return (this->DesignYHPWL);
}

void
Design::DesignSetXHPWL(ulong DesignXHPWL)
{
  this->DesignXHPWL = DesignXHPWL;
}

void
Design::DesignSetYHPWL(ulong DesignYHPWL)
{
  this->DesignYHPWL = DesignYHPWL;
}

void
Design::DesignCellGetMoveCostHPWL(Cell *cellPtr, uint newXpos, uint newYpos, 
				  long &diffXHPWL, long &diffYHPWL)
{
  Net *netPtr;
  long oldXHPWL, oldYHPWL;
  long newXHPWL, newYHPWL;
  uint xPos, yPos;
  uint netXHPWL, netYHPWL;
  uint newNetXHPWL, newNetYHPWL;

  Cell &thisCell = (*cellPtr);
  thisCell.CellGetPos(xPos, yPos);
  thisCell.CellSetPos(newXpos, newYpos);
  oldXHPWL = 0; oldYHPWL = 0;
  newXHPWL = 0; newYHPWL = 0;

  CELL_FOR_ALL_NETS(thisCell, PIN_DIR_ALL, netPtr) {
    (*netPtr).NetSetDirtyHPWL(true);
    (*netPtr).NetComputeHPWL(netXHPWL, netYHPWL, newNetXHPWL, newNetYHPWL);
    oldXHPWL += netXHPWL; oldYHPWL += netYHPWL;
    newXHPWL += newNetXHPWL; newYHPWL += newNetYHPWL;
  } CELL_END_FOR;

  diffXHPWL = oldXHPWL - newXHPWL;
  diffYHPWL = oldYHPWL - newYHPWL;
  thisCell.CellGetPos(xPos, yPos);
}

inline void
getCellMoveCostHPWL(Cell *cellPtr, uint newXpos, uint newYpos, long &diffXHPWL, 
		    long &diffYHPWL)
{
  Net *netPtr;
  long oldXHPWL, oldYHPWL;
  long newXHPWL, newYHPWL;
  uint xPos, yPos;
  uint netOldXHPWL, netOldYHPWL;
  uint newNetXHPWL, newNetYHPWL;

  Cell &thisCell = (*cellPtr);
  
  xPos = thisCell.CellGetXpos();
  yPos = thisCell.CellGetYpos();
  oldXHPWL = 0; oldYHPWL = 0;

  //  CELL_FOR_ALL_NETS(thisCell, PIN_DIR_ALL, netPtr) {
  //    (*netPtr).NetComputeHPWL(netXHPWL, netYHPWL);
  //    oldXHPWL += netXHPWL; oldYHPWL += netYHPWL;
  //  } CELL_END_FOR;

  thisCell.CellMoveCell(newXpos, newYpos);

  newXHPWL = 0; newYHPWL = 0;
  oldXHPWL = 0; oldYHPWL = 0;
  CELL_FOR_ALL_NETS(thisCell, PIN_DIR_ALL, netPtr) {
    (*netPtr).NetComputeHPWL(netOldXHPWL, netOldYHPWL, newNetXHPWL, newNetYHPWL);
    oldXHPWL += netOldXHPWL; oldYHPWL += netOldYHPWL;
    newXHPWL += newNetXHPWL; newYHPWL += newNetYHPWL;
    //    newXHPWL += netXHPWL; newYHPWL += netYHPWL;
  } CELL_END_FOR;

  thisCell.CellMoveCell(xPos, yPos);
  diffXHPWL = oldXHPWL - newXHPWL;
  diffYHPWL = oldYHPWL - newYHPWL;
}

inline double 
computeScore(double maxUtil, double curBinUtil, double targetUtil, long diffHPWL) 
{
  double alpha, beta_m, beta_n, util_m, util_n;
  double gamma, cont_m, cont_n;
  double score;

  alpha = 1.0;
  gamma = 0; cont_m = 0; cont_n = 0;
  beta_m = 10000 * curBinUtil * curBinUtil; beta_n = 10000 * targetUtil * targetUtil;
  util_m = exp(curBinUtil * curBinUtil); util_n = exp(targetUtil * targetUtil);
  if (moveDebug) {
    cout << "SCORE: " 
	 << " HPWL COMP: " << (alpha * diffHPWL) << endl
	 << " UTIL COMP: " << ((beta_m * util_m - beta_n * util_n)) << endl;
  }

  score = alpha * diffHPWL + (beta_m * util_m - beta_n * util_n) + 
    gamma * (cont_m - cont_n);

  return score;
}

bool
Design::DesignCellGetBestPosForILR(Cell &thisCell, Bin &srcBin, Bin *& targetBinPtr,
				   uint &newXpos, uint &newYpos)
{
  Bin *binLeft, *binRight, *binTop, *binBot;
  Bin *binLeftTop, *binRightTop, *binLeftBot, *binRightBot;
  Cell *cellPtr;
  double curBinUtil, diffBinUtil;
  uint curBinIdx, cellXpos, cellYpos;
  uint binHeight, binWidth;
  int nRight, nLeft, nTop, nBot;
  int nDiagRightTop, nDiagRightBot;
  int nDiagLeftTop, nDiagLeftBot;
  bool rtv = false;
  
  nDiagRightTop = -1;
  nDiagLeftTop = -1;
  nDiagRightBot = -1;
  nDiagLeftBot = -1;
  binLeft = NIL(Bin *); binRight = NIL(Bin *);
  binTop = NIL(Bin *); binBot = NIL(Bin *);
  binLeftTop = NIL(Bin *); binLeftBot = NIL(Bin *);
  binRightTop = NIL(Bin *); binRightBot = NIL(Bin *);

  curBinIdx = srcBin.BinGetIdx();
  curBinUtil = srcBin.BinGetUtilization();
  cellXpos = thisCell.CellGetXpos();
  cellYpos = thisCell.CellGetYpos();
  string cellName = thisCell.CellGetName();

  binHeight = DesignGetBinHeight();
  binWidth = DesignGetBinWidth();
  cellPtr = &(thisCell);
  nRight = DesignGetNextRowBinIdx(curBinIdx);
  nLeft = DesignGetPrevRowBinIdx(curBinIdx);
  nTop = DesignGetNextColBinIdx(curBinIdx);
  nBot = DesignGetPrevColBinIdx(curBinIdx);

  if (nRight >= 0) {
    nDiagRightTop = DesignGetNextColBinIdx(nRight);
    nDiagRightBot = DesignGetPrevColBinIdx(nRight);
    binRight = DesignBins[nRight];
    if (nDiagRightTop >= 0) {
      binRightTop = DesignBins[nDiagRightTop];
    }
    if (nDiagRightBot >= 0) {
      binRightBot = DesignBins[nDiagRightBot];
    }
  }
  if (nLeft >= 0) {
    nDiagLeftTop = DesignGetNextColBinIdx(nLeft);
    nDiagLeftBot = DesignGetPrevColBinIdx(nLeft);
    binLeft = DesignBins[nLeft];
    if (nDiagLeftTop >= 0) {
      binLeftTop = DesignBins[nDiagLeftTop];
    }
    if (nDiagLeftBot >= 0) {
      binLeftBot = DesignBins[nDiagLeftBot];
    }
  }
  if (nTop >= 0) {
    binTop = DesignBins[nTop];
  }
  if (nBot >= 0) {
    binBot = DesignBins[nBot];
  }
  
  double topScore, botScore, leftScore, rightScore;
  double topUtil, botUtil, leftUtil, rightUtil;
  double maxUtil, diffUtilization;
  double highestScore;
  uint targetXpos, targetYpos;
  long diffXHPWL, diffYHPWL, diffHPWL;
  topScore = 0; botScore = 0; leftScore = 0; rightScore = 0; highestScore = -DBL_MAX;
  targetXpos = cellXpos; targetYpos = cellYpos;
  maxUtil = DesignGetMaxUtil();
  targetBinPtr = NIL(Bin*);
  //  bool moveDebug = true;
  /* Compute four scores */
  if (moveDebug) {
    cout << "***************************************************" << endl;
    cout << "Score computation summary:" << endl;
    cout << " TOP BIN: " << endl;
    if (!binTop) {
      cout << "   INVALID" << endl;
    }
  }
  if (binTop) {
    diffXHPWL = 0; diffYHPWL = 0; diffHPWL = 0;
    getCellMoveCostHPWL(cellPtr, cellXpos, (cellYpos + binHeight), diffXHPWL, diffYHPWL);
    topUtil = (*binTop).BinGetUtilization();
    diffHPWL = diffXHPWL + diffYHPWL;
    diffUtilization = curBinUtil - topUtil;
    topScore = computeScore(maxUtil, curBinUtil, topUtil, diffHPWL);
    if (highestScore < topScore) {
      highestScore = topScore;
      targetYpos = cellYpos + binHeight;
      targetXpos = cellXpos;
      targetBinPtr = binTop;
    }
    if (moveDebug) {
      cout << "     Old X-pos: " << cellXpos
	   << "  Old Y-pos: " << cellYpos << endl;
      cout << "     Target X-pos: " << cellXpos
	   << "  Target Y-pos: " << cellYpos + binHeight << endl;
      cout << "     Current Util: " << curBinUtil
	   << "  Top bin Util: " << topUtil << endl;
      cout << "        HPWL Diff: " << diffHPWL << endl;
      cout << "        Util Diff: " << diffUtilization << endl;
      cout << "        Score    : " << topScore << endl;
    }
  }
  if (moveDebug) {
    cout << endl;
    cout << " RIGHT BIN: " << endl;
    if (!binRight) {
      cout << "   INVALID" << endl;
    }
  }
  if (binRight) {
    diffXHPWL = 0; diffYHPWL = 0; diffHPWL = 0;
    getCellMoveCostHPWL(cellPtr, cellXpos + binWidth, cellYpos, diffXHPWL, diffYHPWL);
    rightUtil = (*binRight).BinGetUtilization();
    diffHPWL = diffXHPWL + diffYHPWL;
    diffUtilization = curBinUtil - rightUtil;
    rightScore = computeScore(maxUtil, curBinUtil, rightUtil, diffHPWL);
    if (highestScore < rightScore) {
      targetYpos = cellYpos;
      targetXpos = cellXpos + binWidth;
      highestScore = rightScore;
      targetBinPtr = binRight;
    }
    if (moveDebug) {
      cout << "     Old X-pos: " << cellXpos
	   << "  Old Y-pos: " << cellYpos << endl;
      cout << "     Target X-pos: " << cellXpos + binWidth
	   << "  Target Y-pos: " << cellYpos << endl;
      cout << "     Current Util: " << curBinUtil
	   << "  Right bin Util: " << rightUtil << endl;
      cout << "        HPWL Diff: " << diffHPWL << endl;
      cout << "        Util Diff: " << diffUtilization << endl;
      cout << "        Score    : " << rightScore << endl;
    }
  }
  if (moveDebug) {
    cout << endl;
    cout << " LEFT BIN: " << endl;
    if (!binLeft) {
      cout << "   INVALID" << endl;
    }
  }
  if (binLeft) {
    diffXHPWL = 0; diffYHPWL = 0; diffHPWL = 0;
    getCellMoveCostHPWL(cellPtr, (cellXpos - binWidth), cellYpos, diffXHPWL, diffYHPWL);
    leftUtil = (*binLeft).BinGetUtilization();
    diffHPWL = diffXHPWL + diffYHPWL;
    diffUtilization = curBinUtil - leftUtil;
    leftScore = computeScore(maxUtil, curBinUtil, leftUtil, diffHPWL);
    if (highestScore < leftScore) {
      highestScore = leftScore;
      targetYpos = cellYpos;
      targetXpos = cellXpos - binWidth;
      targetBinPtr = binLeft;
    }
    if (moveDebug) {
      cout << "     Old X-pos: " << cellXpos
	   << "  Old Y-pos: " << cellYpos << endl;
      cout << "     Target X-pos: " << cellXpos - binWidth
	   << "  Target Y-pos: " << cellYpos << endl;
      cout << "     Current Util: " << curBinUtil
	   << "  Left bin Util: " << leftUtil << endl;
      cout << "        HPWL Diff: " << diffHPWL << endl;
      cout << "        Util Diff: " << diffUtilization << endl;
      cout << "        Score    : " << leftScore << endl;
    }
  }
  if (moveDebug) {
    cout << endl;
    cout << " BOT BIN: " << endl;
    if (!binBot) {
      cout << "   INVALID" << endl;
    }
  }
  if (binBot) {
    diffXHPWL = 0; diffYHPWL = 0; diffHPWL = 0;
    getCellMoveCostHPWL(cellPtr, cellXpos, (cellYpos - binHeight), diffXHPWL, diffYHPWL);
    botUtil = (*binBot).BinGetUtilization();
    diffHPWL = diffXHPWL + diffYHPWL;
    diffUtilization = curBinUtil - botUtil;
    botScore = computeScore(maxUtil, curBinUtil, botUtil, diffHPWL);
    if (highestScore < botScore) {
      targetYpos = cellYpos - binHeight;
      targetXpos = cellXpos;
      highestScore = botScore;
      targetBinPtr = binBot;
    }
    if (moveDebug) {
      cout << "     Old X-pos: " << cellXpos
	   << "  Old Y-pos: " << cellYpos << endl;
      cout << "     Target X-pos: " << cellXpos
	   << "  Target Y-pos: " << cellYpos - binHeight << endl;
      cout << "     Current Util: " << curBinUtil
	   << "  Bot bin Util: " << botUtil << endl;
      cout << "        HPWL Diff: " << diffHPWL << endl;
      cout << "        Util Diff: " << diffUtilization << endl;
      cout << "        Score    : " << botScore << endl;
    }
  }
  newXpos = targetXpos;
  newYpos = targetYpos;
  if (highestScore > 0) {
    rtv = true;
  } else {
    rtv = false;
  }
    
  if (moveDebug) {
    cout << "#################################################" << endl;
    cout << " Highest score    : " << highestScore << endl;
    cout << " old x-pos : " << thisCell.CellGetXpos() << " old y-pos: " << thisCell.CellGetYpos() 
	 << "  new x-pos: " << newXpos << "  new y-pos:" << newYpos
	 << endl;
    cout << "***************************************************" << endl;
  }

  return (rtv);
}

/*******************************************************************************
  COMPUTE THE INITIAL BIN SIZE FOR CELL SPREADING AND ILR
*******************************************************************************/
void
Design::DesignComputeBinSize(bool ILR) 
{
  uint binHeight, binWidth;
  uint ILRMultiple;

  binHeight = this->averageStdCellHeight;
  binWidth = this->averageStdCellWidth;

  if (!ILR) {
    binHeight *= 4;
    binWidth *= 4;
  } else {
    ILRMultiple = 10;
    binHeight *= ILRMultiple;
    binWidth *= ILRMultiple;
    DesignSetILRMultiple(ILRMultiple);
  }

  DesignComputeBinSize(binHeight, binWidth);
}

void
Design::DesignComputeBinSize(uint binHeight, uint binWidth)
{
  double divideToBins;
  uint endRow, endCol;
  uint overflowx, overflowy;
  uint numRows, numCols;
  uint maxx, maxy;
  
  DesignGetBoundingBox(maxx, maxy);
  numCols = (uint)ceil(((double)maxx) / binWidth);
  numRows = (uint)ceil(((double)maxy) / binHeight);
  
  endRow = (numCols) * binWidth;
  endCol = (numRows) * binHeight;
  overflowx = 0;
  overflowy = 0;
  if (endRow > maxx)  
    overflowx = maxx - ((numCols - 1) * binWidth);
  if (endCol > maxy) 
    overflowy = maxy - ((numRows - 1) * binHeight);
  
  /* Adjust the binWidth/binHeight such that the overflow
     contributes to at least one unit of placement space
     in the x/y-direction to the binWidth/binHeight */
  if (overflowx > 0) {
    divideToBins = overflowx / (numCols - 1);
    if (divideToBins >= 1) {
      binWidth += floor(divideToBins);
      numCols = (uint)ceil(((double)maxx) / binWidth);
      endRow = (numCols) * binWidth;
      if (endRow > maxx) {
	overflowx = maxx - ((numCols - 1) * binWidth);
	divideToBins = overflowx / (numCols - 1);
	numCols--;
	if (divideToBins >= 1) {
	  _ASSERT_TRUE("Error: Overflow after adjustment is more than one unit per column");
	}
      }
    }
  }
  if (overflowy > 0) {
    divideToBins = overflowy / (numRows - 1);
    if (divideToBins >= 1) {
      binHeight += floor(divideToBins);
      numRows = (uint)ceil(((double)maxy) / binHeight);
      endCol = (numRows) * binHeight;
      if (endCol > maxy) {
	overflowy = maxy - ((numRows - 1) * binHeight);
	divideToBins = overflowy / (numRows - 1);
	numRows--;
	if (divideToBins >= 1) {
	  _ASSERT_TRUE("Error: Overflow after adjustment is more than one unit per row");
	}
      }
    }
  }
  DesignSetNumBinRows(numRows);
  DesignSetNumBinCols(numCols);
  DesignSetBinHeight(binHeight);
  DesignSetBinWidth(binWidth);

  cout << "Average cell height: " << binHeight 
       << "  Average cell width : " << binWidth 
       << endl;
  cout << "Row X Cols: " << numRows << " X " << numCols 
       << endl;
}

void
Design::DesignShrinkBinsForILR(void)
{
  uint modBinHeight, modBinWidth;
  uint binHeight, binWidth;
  uint oldNumRows, oldNumCols;
  uint newNumRows, newNumCols;
  uint ILRMultiple;

  ILRMultiple = DesignGetILRMultiple();
  binHeight = this->averageStdCellHeight;
  binWidth = this->averageStdCellWidth;
  oldNumRows = DesignGetNumBinRows();
  oldNumCols = DesignGetNumBinCols();
  newNumRows = oldNumRows;
  newNumCols = oldNumCols;
  
  while((newNumRows > oldNumRows) &&
	(newNumCols > newNumCols)) {
    ILRMultiple--;
    modBinHeight = binHeight * ILRMultiple;
    modBinWidth = binWidth * ILRMultiple;
    DesignComputeBinSize(modBinHeight, modBinWidth);
    newNumRows = DesignGetNumBinRows();
    newNumCols = DesignGetNumBinCols();
  }
  
  DesignCreateBins();
}

void
Design::DesignRefreshBins()
{
  Bin *binPtr;
  vector<Cell*> cellsOfBin;
  vector<Cell*> cellsSortedByLeft, cellsSortedByRight;
  double maxUtilization, overlapArea;
  double totalCellWidth, averageCellWidth;
  double utilization;
  uint left, right, bot, top;
  uint peakUtilBinIdx;
  uint numBins, numCells;
  uint binIdx;
  int prevBinIdx;
  bool binCreated;
  bool addToPreviousRowBin, addToPreviousColBin;

  numBins = DesignBins.size();
  if (numBins <= 0) {
    cout << "Error: Refresh bins called without creating bins!" << endl;
    return;
  }
  _STEP_BEGIN("Bin refresh");
  maxUtilization = 0; 
  peakUtilBinIdx = 0;
  cellsSortedByLeft = DesignGetCellsSortedByLeft();
  cellsSortedByRight = DesignGetCellsSortedByRight();
  DESIGN_FOR_ALL_BINS((*this), binIdx, binPtr) {
    (*binPtr).BinDeleteData();
    (*binPtr).BinGetBoundingBox(left, right, bot, top);
    cellsOfBin =
      DesignGetCellsOfBin(binPtr, left, right, bot, top, cellsSortedByLeft,
			  cellsSortedByRight, overlapArea, totalCellWidth);
    numCells = cellsOfBin.size();      
    if (numCells <= 0) {
      continue;
    }
    (*binPtr).BinSetCells(cellsOfBin);
    (*binPtr).BinSetCellArea(overlapArea);
    (*binPtr).BinComputeUtilization();
    (*binPtr).BinSetTotalCellWidth(totalCellWidth);
    (*binPtr).BinComputeAverageCellWidth();
    (*binPtr).BinSetNewRight(right);
    (*binPtr).BinSetNewTop(top);
    utilization = (*binPtr).BinGetUtilization();
    totalUtilization += utilization;
    //    (*binPtr).BinPrintBin();
    if (numCells > 0) {
      totalNumOccupiedBins++;
    }
    if (maxUtilization < utilization) {
      maxUtilization = utilization;
      peakUtilBinIdx = binIdx;
    }
  } DESIGN_END_FOR;
  DesignSetMaxUtil(maxUtilization);
  DesignSetPeakUtil(maxUtilization);
  DesignSetPeakUtilBinIdx(peakUtilBinIdx);
  _STEP_END("Bin refresh");
}

void
Design::DesignCreateBins()
{
  Bin *binPtr;
  vector<Cell*> cellsOfBin;
  vector<Cell*> cellsSortedByLeft, cellsSortedByRight;
  double oldUtilization, newUtilization;
  double maxUtilization, overlapArea;
  double totalCellWidth, averageCellWidth;
  double utilization;
  uint binWidth, binHeight;
  uint maxx, maxy;
  uint binCount, numRows, numCols, actualRows, actualCols;
  uint numRowsSoFar, numColsSoFar;
  uint left, right, bot, top;
  uint peakUtilBinIdx;
  uint i, j;
  uint numBins, numCells;
  int prevBinIdx, curBegin;
  bool binCreated;
  bool addToPreviousRowBin, addToPreviousColBin;

  _STEP_BEGIN("Bin construction");
  binWidth = DesignGetBinWidth();
  binHeight = DesignGetBinHeight();
  numRows = DesignGetNumBinRows();
  numCols = DesignGetNumBinCols();
  DesignGetBoundingBox(maxx, maxy);
  cellsSortedByLeft = DesignGetCellsSortedByLeft();
  cellsSortedByRight = DesignGetCellsSortedByRight();

  binCount = 0; 
  maxUtilization = 0; 
  peakUtilBinIdx = 0;
  totalUtilization = 0;
  totalNumOccupiedBins = 0;
  binCreated = false;
  numBins = DesignBins.size();
  if (numBins > 0) {
    _ASSERT_TRUE("Error: Bins already exist!!Clear bins before creating bins afresh!");
  }
  cout << "Created regular bin structure with " << " Rows: " << numRows 
       << "  Columns: " << numCols << endl << endl;
  bot = 0; top = binHeight;
  for (i = 0; i < numRows; i++) {
    left = 0; right = binWidth;
    for (j = 0; j < numCols; j++) {
      if (j == (numCols - 1)) {
	right = maxx;
      }
      if (i == (numRows - 1)) {
	top = maxy;
      }
      binPtr = new Bin(binCount, left, right, bot, top);
      DesignBins.push_back(binPtr);
      (*binPtr).BinSetNewRight(right);
      (*binPtr).BinSetNewTop(top);
      cellsOfBin =
        DesignGetCellsOfBin(binPtr, left, right, bot, top, cellsSortedByLeft,
			    cellsSortedByRight, overlapArea, totalCellWidth);
      numCells = cellsOfBin.size();      
      (*binPtr).BinSetCells(cellsOfBin);
      (*binPtr).BinSetCellArea(overlapArea);
      (*binPtr).BinComputeUtilization();
      (*binPtr).BinSetTotalCellWidth(totalCellWidth);
      (*binPtr).BinComputeAverageCellWidth();
      utilization = (*binPtr).BinGetUtilization();
      totalUtilization += utilization;
      if (maxUtilization < utilization) {
        maxUtilization = utilization;
        peakUtilBinIdx = binCount;
      }
      if (numCells > 0) {
	totalNumOccupiedBins++;
      }
      left += binWidth;
      right += binWidth;
      binCount++;
    }
    bot += binHeight;
    top += binHeight;
  }
  DesignSetMaxUtil(maxUtilization);
  DesignSetPeakUtil(maxUtilization);
  DesignSetPeakUtilBinIdx(peakUtilBinIdx);
  DesignSetBinsCreated(true);
  _STEP_END("Bin construction");
}

void
Design::DesignCreateEmptyBins()
{
  Bin *binPtr;
  uint binWidth, binHeight;
  uint maxx, maxy;
  uint binCount, numRows, numCols;
  uint left, right, bot, top;
  uint i, j;
  uint numBins, numCells;

  _STEP_BEGIN("Bin construction");
  binWidth = DesignGetBinWidth();
  binHeight = DesignGetBinHeight();
  numRows = DesignGetNumBinRows();
  numCols = DesignGetNumBinCols();
  DesignGetBoundingBox(maxx, maxy);

  binCount = 0; 
  numBins = DesignBins.size();
  if (numBins > 0) {
    _ASSERT_TRUE("Error: Bins already exist!!Clear bins before creating bins afresh!");
  }
  cout << "Created regular bin structure with " << " Rows: " << numRows 
       << "  Columns: " << numCols << endl << endl;
  bot = 0; top = binHeight;
  for (i = 0; i < numRows; i++) {
    left = 0; right = binWidth;
    for (j = 0; j < numCols; j++) {
      if (j == (numCols - 1)) {
	right = maxx;
      }
      if (i == (numRows - 1)) {
	top = maxy;
      }
      binPtr = new Bin(binCount, left, right, bot, top);
      DesignBins.push_back(binPtr);
      (*binPtr).BinSetNewRight(right);
      (*binPtr).BinSetNewTop(top);
      left += binWidth;
      right += binWidth;
      binCount++;
    }
    bot += binHeight;
    top += binHeight;
  }
  DesignSetBinsCreated(true);
  _STEP_END("Bin construction");
}

void
Design::DesignClearBins(void)
{
  Bin *binPtr;

  VECTOR_FOR_ALL_ELEMS(DesignBins, Bin*, binPtr) {
    (*binPtr).BinDeleteData();
  } END_FOR;
}

void
Design::DesignDestroyBins(void)
{
  Bin *binPtr;

  VECTOR_FOR_ALL_ELEMS(DesignBins, Bin*, binPtr) {
    delete binPtr;
  } END_FOR;
  DesignSetBinsCreated(false);
  DesignBins.clear();
}

double
Design::DesignGetPeakUtil(void)
{
  return (this->peakUtilization);
}

uint
Design::DesignGetPeakUtilBinIdx(void)
{
  return (this->peakUtilizationBinIdx);
}

double
Design::DesignGetMaxUtil(void)
{
  return (this->maxUtilization);
}

double
Design::DesignGetTotalUtil(void)
{
  return (this->totalUtilization);
}

uint
Design::DesignGetTotalNumOccBins(void)
{
  return (this->totalNumOccupiedBins);
}

double
Design::DesignGetAverageUtil(void)
{
  return (totalUtilization / ((double)totalNumOccupiedBins));
}

vector<Cell *>&
Design::DesignGetCellsToSolve(void)
{
  return cellsToSolve;
}

void
Design::DesignSetCellsToSolve(vector<Cell *> cellsToSolve)
{
  this->cellsToSolve = cellsToSolve;
}

int
Design::DesignGetNextRowBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!((binIdx + 1) % numBinCols == 0)) 
    rtv = binIdx + 1;
  
  return (rtv);
}

int
Design::DesignGetNextColBinIdx(uint binIdx)
{
  int rtv;
  uint nextColBinIdx;
  uint rowOfBin;
  
  rtv = -1;
  rowOfBin = binIdx / numBinCols;
  if (rowOfBin < (numBinRows - 1)) {
    rtv = binIdx + numBinCols;
  }
  
  return (rtv);
}

int
Design::DesignGetPrevRowBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!(binIdx % numBinCols == 0)) 
    rtv = binIdx - 1;
  
  return (rtv);
}

int
Design::DesignGetPrevColBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!(binIdx / numBinCols == 0))
    rtv = binIdx - numBinCols;
  
  return (rtv);
}

void
Design::DesignSetBinHeight(uint binHeight)
{
  this->binHeight = binHeight;
}

void
Design::DesignSetBinWidth(uint binWidth)
{
  this->binWidth = binWidth;
}

void
Design::DesignSetILRMultiple(uint ILRMultiple)
{
  this->ILRMultiple = ILRMultiple;
}

void
Design::DesignSetBinsCreated(bool binsCreated)
{
  this->binsCreated = binsCreated;
}

uint
Design::DesignGetBinHeight(void)
{
  return (binHeight);
}

uint
Design::DesignGetBinWidth(void)
{
  return (binWidth);
}

uint
Design::DesignGetILRMultiple(void)
{
  return (this->ILRMultiple);
}

bool
Design::DesignGetBinsCreated(void)
{
  return (this->binsCreated);
}

vector<Cell *>
Design::DesignGetCellsSortedByLeft(void)
{
  vector<Cell *> rtv;
  
  rtv = DesignGetCellsToSolve();
  sort(rtv.begin(), rtv.end(), cmpCellLeftPos);

  return (rtv);
}

vector<Cell *>
Design::DesignGetCellsSortedByRight(void)
{
  vector<Cell *> rtv;
  
  rtv = DesignGetCellsToSolve();
  sort(rtv.begin(), rtv.end(), cmpCellRightPos);

  return (rtv);
}

vector<Cell *>
Design::DesignGetCellsSortedByBot(void)
{
  vector<Cell *> rtv;
  
  rtv = DesignGetCellsToSolve();
  sort(rtv.begin(), rtv.end(), cmpCellBotPos);

  return (rtv);
}

void 
Design::DesignAddDelayArc(string libCell, string outputPin, string inputPin, 
			  double arcDelay)
{
  _KEY_EXISTS(libCellDelayDB, libCell) {
    map<string, map<string, double> >& cellDelays = libCellDelayDB[libCell];
    _KEY_EXISTS(cellDelays, outputPin) {
      map<string, double> &arcDelays = cellDelays[outputPin];
      _KEY_EXISTS(arcDelays, inputPin) {
	cout << "Warning: Delay for library cell (" << libCell 
	     << ") arc " << outputPin << " - " << inputPin << " being replaced" << endl;
      }
      arcDelays[inputPin] = arcDelay;
    } else {
      map<string, double> arcDelays;
      arcDelays[inputPin] = arcDelay;
      cellDelays[outputPin] = arcDelays;
    }
  } else {
    map<string, map<string, double > > cellDelays;
    map<string, double> arcDelays;
    arcDelays[inputPin] = arcDelay;
    cellDelays[outputPin] = arcDelays;
    libCellDelayDB[libCell] = cellDelays;
  }
}

double
Design::DesignGetDelayArc(string libCell, string outputPin, string inputPin)
{
  double rtv;

  rtv = -1;
  _KEY_EXISTS(libCellDelayDB, libCell) {
    map<string, map<string, double> >& cellDelays = libCellDelayDB[libCell];
    _KEY_EXISTS(cellDelays, outputPin) {
      map<string, double> &arcDelays = cellDelays[outputPin];
      _KEY_EXISTS(arcDelays, inputPin) {
	rtv = arcDelays[inputPin];
      }
    }
  }
  
  return (rtv);
}

void
Design::DesignAddPath(Path *path)
{
  DesignPaths.push_back(path);
  this->NumPaths++;
}

void
Design::DesignHideNet(Net *netPtr)
{
  string netName;
  
  netName = (*netPtr).NetGetName();
  _KEY_DOES_NOT_EXIST(DesignHiddenNets, netName) {
    (*netPtr).NetSetIsHidden(true);
    DesignNets.erase(netName);
    DesignHiddenNets[netName] = netPtr;
    this->NumNets--;
  }
}

void
Design::DesignUnhideNet(Net *netPtr)
{
  string netName;
  
  netName = (*netPtr).NetGetName();
  (*netPtr).NetSetIsHidden(false);
  _KEY_EXISTS(DesignHiddenNets, netName) {
    DesignNets[netName] = netPtr;
    DesignHiddenNets.erase(netName);
    this->NumNets++;
  }
}

void
Design::DesignHideCell(Cell *thisCell)
{
  string cellName;
  
  cellName = (*thisCell).CellGetName();
  _KEY_DOES_NOT_EXIST(DesignHiddenCells, cellName) {
    (*thisCell).CellSetIsHidden(true);
    DesignHiddenCells[cellName] = thisCell;
    this->NumHiddenCells++;
  }
}

void
Design::DesignUnhideCell(Cell *thisCell)
{
  string cellName;
  
  cellName = (*thisCell).CellGetName();
  (*thisCell).CellSetIsHidden(false);
  DesignHiddenCells.erase(cellName);
  this->NumHiddenCells--;
}

void
Design::DesignAddCellToPhysRow(Cell* cell, vector<vector<int> > &allRowBounds, 
			       vector<PhysRow*> &allPhysRows)
{
  /* Getting cell bounds */

  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  
  vector<int> Obj;
  int rowIndex;
  int cellCount;
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  bool foundPos = false;

  VECTOR_FOR_ALL_ELEMS(allRowBounds, vector<int>, Obj){
    rowIndex = i;
    if (rowType == HORIZONTAL){
      int botX = Obj[0];
      int botY = Obj[1];
      int topX = Obj[2];
      int topY = Obj[3];
      int cellXend = cellX + cellWidth;
      int cellYend = cellY + cellHeight;
      
      if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	  (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	  ((cellHeight % (topY - botY)) == 0)){
	
	  (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	  foundPos = true;
	  break;
      }
    } else if (rowType == VERTICAL){
      int botX = Obj[0];
      int botY = Obj[1];
      int topX = Obj[2];
      int topY = Obj[3];
      int cellXend = cellX + cellHeight;
      int cellYend = cellY + cellWidth;
      if ((cellX == botX) && (cellXend > botX) && 
	  ((cellHeight % (topX-botX)) == 0) && (cellY >= botY) &&
	  (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	allPhysRows[rowIndex]->PhysRowAddCellToRow(cell);
	foundPos = true;
	break;
      }
    }
  } END_FOR;
}

void
Design::DesignAddAllCellsToPhysRows(void)
{
  string CellName;
  Cell* CellPtr;
  vector<PhysRow*> allPhysRows = DesignGetRows();
  vector<vector<int> > allRowBounds;
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  /* Get Bounding boxes for all rows */
  PhysRow* Obj;  
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj){
    vector<int> v;
    Obj->PhysRowGetBoundingBox(v);
    Obj->PhysRowSetIndex(i);
    allRowBounds.push_back(v);
  } END_FOR;
  
  DESIGN_FOR_ALL_CELLS((*this), CellName, CellPtr){
    DesignAddCellToPhysRow(CellPtr, allRowBounds, allPhysRows);
  } DESIGN_END_FOR;
}

void
Design::DesignOpenFile(string FileName)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
  DesignFile.open(FileName.data(), ifstream::in);
}


void
Design::DesignCloseFile(void)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
}

int 
Design::DesignGetSingleRowHeight(void)
{
  return singleRowHeight;
}

unsigned int
Design::DesignGetNumCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumClusters(void)
{
  unsigned int rtv;
  
  rtv = this->NumClusters;
  
  return (rtv);
}

uint
Design::DesignGetNumStdCells(void)
{
  uint rtv;
  
  rtv = this->NumStdCells;
  
  return (rtv);
}

uint
Design::DesignGetNumTopCells(void)
{
  uint rtv;
  
  rtv = this->NumCells + this->NumClusters;
  rtv -= this->NumHiddenCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumNets(void)
{
  unsigned int rtv;
  
  rtv = this->NumNets;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumPaths(void)
{
  unsigned int rtv;
  
  rtv = this->NumPaths;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumFixedCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumFixedCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumTerminalCells(void)
{
  unsigned int rtv;
  
  rtv = NumTerminalCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumPhysRows(void)
{
  unsigned int rtv;
  
  rtv = this->NumPhysRows;
  
  return (rtv);
}

void
Design::DesignGetBoundingBox(uint &maxx, uint &maxy)
{
  maxx = (*this).maxx;
  maxy = (*this).maxy;
}

void
Design::DesignSetGraph(HyperGraph& thisGraph) 
{
  this->DesignGraphPtr = &thisGraph;
}

HyperGraph&
Design::DesignGetGraph(void)
{
  HyperGraph& rtv = (*(this->DesignGraphPtr));

  return (rtv);
}

double
Design::DesignGetClockPeriod(void)
{
  double rtv;
  
  rtv = clockPeriod;
  
  return (rtv);
}

void
Design::DesignSetClockPeriod(double clkPeriod)
{
  this->clockPeriod = clkPeriod;
}

void
Design::DesignUpdateChipDim(PhysRow *row)
{
  vector<int> boundingBox;
  int &left = this->preShiftLeft;
  int &right = this->preShiftRight;
  int &bot = this->preShiftBot;
  int &top = this->preShiftTop;
  int rowLeft, rowBot, rowRight, rowTop;
  (*row).PhysRowGetBoundingBox(rowLeft, rowBot, rowRight, rowTop);

  if (rowLeft < left) {
    left = rowLeft;
  }
  if (rowRight > right) {
    right = rowRight;
    maxx = right;
  } 
  if (rowBot < bot) {
    bot = rowBot;
  }
  if (rowTop > top) {
    top = rowTop;
    maxy = top;
  } 
}

void
Design::DesignShiftChipToZeroZero(void)
{
  PhysRow *PhysRowPtr;
  Cell *cellPtr;
  string cellName;
  uint RowIdx;
  int rowLeft, rowRight, rowBot, rowTop;
  int cellLeft, cellBot;
  int rowCoordinate, rowBegin;
  uint numSites, siteSpacing;
  rowOrientation orient;

  DESIGN_FOR_ALL_ROWS((*this), RowIdx, PhysRowPtr) {
    orient = (*PhysRowPtr).PhysRowGetType();
    rowCoordinate = (*PhysRowPtr).PhysRowGetCoordinate();
    rowBegin = (*PhysRowPtr).PhysRowGetRowBegin();
    siteSpacing = (*PhysRowPtr).PhysRowGetSiteSpacing();
    numSites = (*PhysRowPtr).PhysRowGetNumSites();
    if (orient == HORIZONTAL) {
      rowCoordinate -= (*this).preShiftBot;
      rowBegin -= (*this).preShiftLeft;
    } else if (orient == VERTICAL) {
      rowCoordinate -= (*this).preShiftLeft;
      rowBegin -= (*this).preShiftBot;
    } else {
      cout << "Error: Unknown row type caught. Exiting!!!" << endl;
      exit(0);
    }
    (*PhysRowPtr).PhysRowSetRowBegin(rowBegin);
    (*PhysRowPtr).PhysRowSetCoordinate(rowCoordinate);
  } DESIGN_END_FOR;
  
  DESIGN_FOR_ALL_CELLS((*this), cellName, cellPtr) {
    cellLeft = (*cellPtr).CellGetXpos();
    cellBot = (*cellPtr).CellGetYpos();
    cellLeft -= preShiftLeft;
    cellBot -= preShiftBot;
    (*cellPtr).CellSetXpos(cellLeft);
    (*cellPtr).CellSetYpos(cellBot);
  } DESIGN_END_FOR;
  
  (*this).maxx -= preShiftLeft;
  (*this).maxy -= preShiftBot;
}

void 
Design::DesignSetVarsPostRead()
{
  double &avgStdCellWidth = this->averageStdCellWidth;
  double &avgStdCellHeight = this->averageStdCellHeight;
  
  avgStdCellWidth /= this->NumStdCells;
  avgStdCellHeight /= this->NumStdCells;
}

double
Design::DesignGetAverageStdCellWidth()
{
  return (this->averageStdCellWidth);
}

double
Design::DesignGetAverageStdCellHeight()
{
  return (this->averageStdCellHeight);
}

void 
Design::DesignSetEnv(Env &thisEnv)
{
  this->DesignEnv = thisEnv;
}

Env& 
Design::DesignGetEnv(void)
{
  return DesignEnv;
}

void
Design::DesignBuildGraph(void)
{
  HyperGraph *myGraph = new HyperGraph();

  DesignCreateGraph(*myGraph);
  DesignSetGraph(*myGraph);
}

void
Design::DesignDeleteGraph(void)
{
  HyperGraph &myGraph = DesignGetGraph();
  
  delete (&myGraph);
}

void
Design::DesignRebuildGraph(void)
{
  DesignDeleteGraph();
  DesignBuildGraph();
}

void
Design::DesignInit()
{
  /* Initialize private variables */
  NumCells = 0;
  NumHiddenCells = 0;
  NumClusters = 0;
  NumClustersSeenSofar = 0;
  NumNets = 0;
  NumPaths = 0;
  NumPhysRows = 0;
  NumFixedCells = 0;
  NumTerminalCells = 0;
  NumStdCells = 0;
  NumMacroCells = 0;
  maxx = -INT_MAX;
  maxy = -INT_MAX;

  singleRowHeight = -1;
  binHeight = 0;
  binWidth = 0;
  ILRMultiple = 0;
  clockPeriod = 0.0;
  peakUtilization = 0.0;
  totalUtilization = 0.0;
  maxUtilization = 0.0;
  totalNumOccupiedBins = 0;
  peakUtilizationBinIdx = 0;
  binsCreated = false;

  preShiftLeft = 0;
  preShiftBot = 0;
  preShiftRight = -INT_MAX;
  preShiftTop = -INT_MAX;
  
  Name = "";
  DesignPath = "";
  DesignCellFileName = "";
  DesignNetFileName = "";
  DesignNetWtsFileName = "";
  DesignSclFileName = "";
  DesignPlFileName = "";
  RowBasedPlacement = false;
  
  DesignGraphPtr = NIL(HyperGraph *);
  /* Initialize public variables */
  /* empty for now as there are only maps and vectors */
}

Design::Design() 
{
  DesignInit();
}

Design::Design(string DesignPath, string DesignName, Env &designEnv)
{
  DesignInit();

  DesignSetEnv(designEnv);
  DesignReadDesign(DesignPath, DesignName);
}

/* Calculate WMax for each PhysRow */
/*
  vector<PhysRow*> allPhysRows = DesignGetRows();
  PhysRow* Obj;
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    Obj->PhysRowCalculateWMax();
  }END_FOR;
*/
# if 0
void
Design::DesignAddCellToPhysRow(Cell *cell)
{
  /* Getting cell bounds */

  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  
  vector< unsigned int> Obj;
  int rowIndex;
  unsigned int subRowIndex;
  int cellCount;
  rowOrientation rowType = allPhysRows[0]->PhysRowGetType();
  
  VECTOR_FOR_ALL_ELEMS(allRowBounds, vector<unsigned int>, Obj){
    rowIndex = i;
    if (Obj.size() > 4){
      if (rowType == HORIZONTAL){
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	
	for (int i = 0; i < Obj.size(); i+=4){
	  
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
 
	  /* Cell found in subrow */
	  if((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	     (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	     ((cellHeight % (topY - botY)) == 0)){
	    subRowIndex = (i % 4);
	    //cout<<"subRowIndex="<<subRowIndex<<endl;
	    (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	    //cout<<"Added "<<cellCount++<<"cell"<<endl;
	    break; /* No more checks required */
	  }
	}
      }
      else if(rowType == VERTICAL){
	int cellXend = cellX + cellHeight;
	int cellYend = cellY + cellWidth;
	
	for(int i = 0; i < Obj.size(); i += 4){
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
	  
	  if((cellX == botX) && (cellXend > botX) && 
	     ((cellHeight % (topX - botX)) == 0) && (cellY >= botY) && 
	     (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	    subRowIndex = (i % 4);
	    (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	    //cout<<"Added "<<cellCount++<<"cell"<<endl;
	    break;
	  }
	}
      }
    }
    /* Indicates one subRow present in the entire row */
    
    else if ((Obj.size() == 4)){
      if (rowType == HORIZONTAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	
	if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	   (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	   ((cellHeight % (topY - botY)) == 0)){
	  
	  //cout<<"Will add cell: "<<cell->CellGetName()<<endl;
	  //cout<<"rowIndex :"<<rowIndex<<endl;
	  //cout<<allPhysRows[rowIndex]->PhysRowGetCoordinate()<<endl;
	  (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	  
	  //cout<<"Added "<<cellCount++<<"cell"<<endl;
	}
      }
      else if (rowType == VERTICAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellHeight;
	int cellYend = cellY + cellWidth;
	if ((cellX == botX) && (cellXend > botX) && 
	   ((cellHeight % (topX-botX)) == 0) && (cellY >= botY) &&
	    (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	  allPhysRows[rowIndex]->PhysRowAddCellToRow(cell);
	  //cout<<"Added "<<cellCount++<<"cell"<<endl;
	}
      }
    }
  } END_FOR;
}

# endif
