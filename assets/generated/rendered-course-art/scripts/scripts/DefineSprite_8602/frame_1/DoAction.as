gridframe = 100;
if(_root.GridYSize == undefined)
{
   _root.GridYSize = 40;
   _root.GridXSize = 40;
}
MaxY = 999;
MaxX = 999;
MinY = 16;
MinX = 24;
Y = _root.GridYSize;
X = _root.GridXSize;
Check = function(clip)
{
   if(hold == clip)
   {
      if(this[clip].hitTest(_root._xmouse,_root._ymouse,false))
      {
         clicktime++;
         if(clicktime > 12 && (Math.round(clicktime / 2) == clicktime / 2 || clicktime > 120))
         {
            this["Function" + clip]();
         }
      }
      else
      {
         clicktime = 0;
         hold = undefined;
      }
   }
   if(this[clip].hitTest(_root._xmouse,_root._ymouse,false))
   {
      this[clip]._xscale += (150 - this[clip]._xscale) / 4;
   }
   else
   {
      this[clip]._xscale += (100 - this[clip]._xscale) / 4;
   }
   this[clip]._yscale = this[clip]._xscale;
};
CheckClick = function(clip)
{
   if(this[clip].hitTest(_root._xmouse,_root._ymouse,false))
   {
      this["Function" + clip]();
      hold = clip;
      clicktime = 0;
   }
};
CheckUp = function()
{
   hold = undefined;
};
Functionup = function()
{
   if(Y < MaxY - 1)
   {
      Y += 1;
      i = 0;
      while(i < _root.LDItemNum)
      {
         _root.LDItemArray[i][2] = Number(_root.LDItemArray[i][2]) + 32;
         if(_root.LDItemArray[i][0] == "39" || _root.LDItemArray[i][0] == "40")
         {
            _root.LDItemArray[i][4] = Number(_root.LDItemArray[i][4]) + 32;
         }
         _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
         _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
         i++;
      }
      i = 0;
      while(i < _root.TileArray.length)
      {
         _root.TileArray[i].unshift(_root.BlankTile);
         i++;
      }
   }
};
Functiondp = function()
{
   if(Y < MaxY - 1)
   {
      Y += 1;
      i = 0;
      while(i < _root.TileArray.length)
      {
         _root.TileArray[i].push(_root.BlankTile);
         i++;
      }
   }
};
Functionlp = function()
{
   if(X < MaxX - 1)
   {
      i = 0;
      while(i < _root.LDItemNum)
      {
         _root.LDItemArray[i][1] = Number(_root.LDItemArray[i][1]) + 32;
         if(_root.LDItemArray[i][0] == "39" || _root.LDItemArray[i][0] == "40")
         {
            _root.LDItemArray[i][3] = Number(_root.LDItemArray[i][3]) + 32;
         }
         _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
         _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
         i++;
      }
      X += 1;
      array = [];
      i = 0;
      while(i < Y)
      {
         array.push(_root.BlankTile);
         i++;
      }
      _root.TileArray.unshift(array);
   }
};
Functionrp = function()
{
   if(X < MaxX - 1)
   {
      X += 1;
      array = [];
      i = 0;
      while(i < Y)
      {
         array.push(_root.BlankTile);
         i++;
      }
      _root.TileArray.push(array);
   }
};
Functionum = function()
{
   if(Y > MinY + 1)
   {
      i = 0;
      while(i < _root.LDItemNum)
      {
         _root.LDItemArray[i][2] = Number(_root.LDItemArray[i][2]) - 32;
         if(_root.LDItemArray[i][0] == "39" || _root.LDItemArray[i][0] == "40")
         {
            _root.LDItemArray[i][4] = Number(_root.LDItemArray[i][4]) - 32;
         }
         _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
         _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
         i++;
      }
      Y -= 1;
      i = 0;
      while(i < _root.TileArray.length)
      {
         _root.TileArray[i].shift(_root.BlankTile);
         i++;
      }
   }
};
Functiondm = function()
{
   if(Y > MinY + 1)
   {
      Y -= 1;
      i = 0;
      while(i < _root.TileArray.length)
      {
         _root.TileArray[i].pop(_root.BlankTile);
         i++;
      }
   }
};
Functionlm = function()
{
   if(X > MinX + 1)
   {
      i = 0;
      while(i < _root.LDItemNum)
      {
         _root.LDItemArray[i][1] = Number(_root.LDItemArray[i][1]) - 32;
         if(_root.LDItemArray[i][0] == "39" || _root.LDItemArray[i][0] == "40")
         {
            _root.LDItemArray[i][3] = Number(_root.LDItemArray[i][3]) - 32;
         }
         _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
         _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
         i++;
      }
      X -= 1;
      array = [];
      i = 0;
      while(i < Y)
      {
         array.push(_root.BlankTile);
         i++;
      }
      _root.TileArray.shift(array);
   }
};
Functionrm = function()
{
   if(X > MinX + 1)
   {
      X -= 1;
      array = [];
      i = 0;
      while(i < Y)
      {
         array.push(_root.BlankTile);
         i++;
      }
      _root.TileArray.pop(array);
   }
};
onEnterFrame = function()
{
   trace(_root.TileArray.length);
   trace(_root.TileArray[0].length);
   Y = Math.max(Math.min(Y,MaxY),MinY);
   X = Math.max(Math.min(X,MaxX),MinX);
   gfx.clip._xscale = 100 + 1.5 * (X - 20);
   gfx.clip._yscale = 100 + 1.5 * (Y - 20);
   xx = "x: " + X;
   yy = "y: " + Y;
   _root.GridYSize = Y;
   _root.GridXSize = X;
   Check("up");
   Check("dp");
   Check("lp");
   Check("rp");
   Check("um");
   Check("dm");
   Check("lm");
   Check("rm");
};
onMouseDown = function()
{
   CheckClick("up");
   CheckClick("dp");
   CheckClick("lp");
   CheckClick("rp");
   CheckClick("um");
   CheckClick("dm");
   CheckClick("lm");
   CheckClick("rm");
};
onMouseUp = function()
{
   CheckUp();
};
