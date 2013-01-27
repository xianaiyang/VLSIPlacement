# include <Flags.h>

/* File having set and clear functions for each flag added to the 
   enum list */

bool CellIsClustered(Cell *cellPtr)
{
  bool rtv;

  void *obj = (void *)cellPtr;
  
  //  rtv = FlagsDefGetFlag(CELL_IS_CLUSTERED_FLAG, obj);
  rtv = (*cellPtr).CellIsClustered();

  return (rtv);
}

void CellSetIsClustered(Cell *cellPtr)
{
  // void *obj = (void *)cellPtr;
  
  //FlagsDefSetFlag(CELL_IS_CLUSTERED_FLAG, obj);
  (*cellPtr).CellSetIsClustered(true);
}

void CellClearIsClustered(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearFlag(CELL_IS_CLUSTERED_FLAG, obj);
}

void CellClearAllIsClustered(void)
{
  FlagsDefClearAllObjs(CELL_IS_CLUSTERED_FLAG);
}

bool NetIsHidden(Net *netPtr)
{
  bool rtv;

  void *obj = (void *)netPtr;
  
  rtv = FlagsDefGetFlag(NET_IS_HIDDEN_FLAG, obj);
  
  return (rtv);
}

void NetSetIsHidden(Net *netPtr)
{
  void *obj = (void *)netPtr;
  
  FlagsDefSetFlag(NET_IS_HIDDEN_FLAG, obj);
}

void NetClearIsHidden(Net *netPtr)
{
  void *obj = (void *)netPtr;

  FlagsDefClearFlag(NET_IS_HIDDEN_FLAG, obj);
}

void NetClearAllIsHidden(Net *netPtr)
{
  FlagsDefClearAllObjs(NET_IS_HIDDEN_FLAG);
}

void PinSetOriginalPin(Pin *thisPin, Pin *originalPin)
{
  void *obj = (void *)thisPin;
  void *ptr = (void *)originalPin;

  FlagsDefSetPtr(PIN_ORIGINAL_PIN_FLAG, obj, ptr);
}

void* PinGetOriginalPin(Pin *thisPin)
{
  void *obj = (void *)thisPin;
  void *ptr = FlagsDefGetPtr(PIN_ORIGINAL_PIN_FLAG, obj);
  
  return (ptr);
}

void PinClearOriginalPin(Pin *thisPin)
{
  void *obj = (void *)thisPin;
  
  FlagsDefClearPtr(PIN_ORIGINAL_PIN_FLAG, obj);
}

void PinClearAllOriginalPin(void)
{
  FlagsDefClearAllObjs(PIN_ORIGINAL_PIN_FLAG);
}

