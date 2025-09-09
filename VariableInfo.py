def GetProperDecimalString(dx_original):
  
  dx = dx_original
  dec = 1 
  for i in range(0,10):
    newdx = float("%1.6f"%(dx*10))
    belowOnePart = newdx-int(newdx)
    dx = newdx
    if belowOnePart==0:
      dec = i+1
      break

  format_string = f"%1.{dec}f"
  ret = format_string % dx_original
  return ret

class VariableInfo:

  def __init__(self, Name, Expr="", Latex="", xMin=0., xMax=100., dx=1, CustomBinning=[], BinNormWidth=-1, Unit="", IsSpillVar=False, ytitle=""):

    self.Name = Name
    self.Expr = Expr
    self.Latex = Latex
    self.Unit = Unit
    self.xMin = xMin
    self.xMax = xMax
    self.dx = dx
    self.nx = round( (xMax-xMin)/dx )
    self.IsSpillVar = IsSpillVar
    self.ytitle = ytitle

    self.CustomBinning = CustomBinning
    self.BinNormWidth = BinNormWidth

    if self.IsSpillVar:
      self.EventUnit = "Spills"
    else:
      self.EventUnit = "Events"

    if len(CustomBinning)>0:
      self.xMin = self.CustomBinning[0]
      self.xMax = self.CustomBinning[-1]
      self.nx = len(self.CustomBinning)-1

  def GetNRebin(self, current_dx):
    current_dx = round(current_dx, 5)
    if current_dx<self.dx:
      #print("self.dx = ", self.dx, "current_dx = ", current_dx, " -> nrebin = ", self.dx/current_dx, int(self.dx/current_dx))
      return round(self.dx/current_dx)
    else:
      return 1

  def GetXaxisTitle(self):

    if self.Unit=="":
      return self.Latex
    else:
      return "%s (%s)"%(self.Latex, self.Unit)

  def GetYaxisTitle(self):

    if self.ytitle!="":
      return self.ytitle

    if len(self.CustomBinning)==0:
      binSizeString = GetProperDecimalString(self.dx)
      if self.Unit=="":
        return "%s/%s"%(self.EventUnit, binSizeString)
      else:
        return "%s/(%s%s)"%(self.EventUnit, binSizeString,self.Unit)
    else:
      if self.BinNormWidth>0:
        binSizeString = GetProperDecimalString(self.BinNormWidth)
        if self.Unit=="":
          return "%s/%s"%(self.EventUnit, binSizeString)
        else:
          return "%s/(%s%s)"%(self.EventUnit, binSizeString,self.Unit)
      else:
        return "%s per bin"%(self.EventUnit)

  def GetBinArray(self):

    if len(self.CustomBinning)>0:
      return self.CustomBinning
    else:
      out = []
      for i in range(0, self.nx):
        out.append( self.xMin + self.dx*i )
      out.append(self.xMax)
      return out


