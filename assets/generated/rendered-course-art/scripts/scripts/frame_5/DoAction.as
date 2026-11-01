_root.EraseLevel = function()
{
   _root.TileArray = [];
   _root.GridXSize = 50;
   _root.GridYSize = 30;
   MaxX = _root.GridXSize;
   MaxY = _root.GridYSize;
   _root.LDItemArray = [["1",64,832,0,0,"Right"]];
   X = 0;
   while(X < MaxX)
   {
      _root.TileArray.push([""]);
      Y = 0;
      while(Y < MaxY)
      {
         _root.TileArray[X][Y] = "100";
         if(Y == MaxY - 2)
         {
            _root.TileArray[X][Y] = "101";
         }
         if(Y == MaxY - 1)
         {
            _root.TileArray[X][Y] = "103";
         }
         Y++;
      }
      X++;
   }
   _root.LDMusic = 1;
   _root.LDCourseBG = 1;
   _root.LDCourseName = "My Level";
   _root.LDOpenNextWindow = "Info";
   if(LDRelayIntructions !== true)
   {
      LDRelayIntructions = true;
      _root.LDOpenNextWindow = "Inst";
   }
};
_root.LDItemNumRotatable = function(anum)
{
   anum2 = Number(LevelItems[anum][0]);
   if(anum2 < 28 || anum2 >= 50 && anum2 <= 87 || anum2 == 38 || anum2 == 39 || anum2 == 40)
   {
      return false;
   }
   return true;
};
_root.LDItemNumFlippable = function(anum)
{
   anum2 = Number(LevelItems[anum][0]);
   if((anum2 <= 50 || anum2 >= 71 && anum2 !== 72) && anum2 !== 18 && anum2 !== 19 && anum2 !== 37)
   {
      return false;
   }
   return true;
};
_root.ItemNumRotatable = function(anum)
{
   anum2 = Number(LDItemArray[anum][0]);
   if(anum2 < 28 || anum2 >= 50 && anum2 <= 87 || anum2 == 38 || anum2 == 39 || anum2 == 40 || anum2 == 43)
   {
      return false;
   }
   return true;
};
_root.ItemNumFlippable = function(anum)
{
   anum2 = Number(LDItemArray[anum][0]);
   if((anum2 <= 50 || anum2 >= 71 && anum2 !== 72) && anum2 !== 18 && anum2 !== 19 && anum2 !== 37)
   {
      return false;
   }
   return true;
};
_root.ItemPlatform = function(anum)
{
   anum2 = Number(_root.LevelItems[anum][0]);
   if(anum2 == 40 || anum2 == 41 || anum2 == 42 || anum2 >= 129 && anum2 < 138)
   {
      return true;
   }
   return false;
};
_root.ItemBPlatform = function(anum)
{
   anum2 = Number(_root.LevelItems[anum][0]);
   if(anum2 >= 28 && anum2 < 37 || anum2 == 112 || anum2 == 113 || anum2 == 121 || anum2 == 122 || anum2 == 138 || anum2 == 139 || anum2 == 10)
   {
      return true;
   }
   return false;
};
_root.ItemFrontGFX = function(anum)
{
   anum2 = Number(LDItemArray[anum][0]);
   if(anum2 >= 140 && anum2 <= 143)
   {
      return true;
   }
   return false;
};
_root.ItemFrontGFXBack = function(anum)
{
   anum2 = Number(LDItemArray[anum][0]);
   if(anum2 >= 28 && anum2 <= 36 || anum2 >= 102 && anum2 <= 105 || anum2 == 108 || anum2 >= 110 && anum2 <= 118 || anum2 == 121 || anum2 == 122 || anum2 == 124 || anum2 == 125 || anum2 == 128 || anum2 >= 129 && anum2 <= 137 || anum2 == 144 || anum2 == 145)
   {
      return true;
   }
   return false;
};
_root.CreateLevelDesigner = function()
{
   if(_root.LDMenuFrame == "2")
   {
      _root.LDOpenNextWindow = "Otherlevel";
   }
   _root.StopBGsong();
   _root.SongIntro = "";
   _root.SongRepeat = "";
   _root.PlayingLevelDesigner = true;
   _root.LevelDesignerMenu = true;
   _root.PauseGame = false;
   _root.playpause = false;
   gotoAndStop("LevelDesigner");
   _root.MouseDown = false;
   _root.MenuAccess = false;
   _root.Zoom = 1;
   _root.PlaceMode = "TileSelect";
   _root.PlacingTile = undefined;
   _root.BlankTile = "100";
   if(_root.ScrollSpeed == undefined)
   {
      _root.ScrollSpeed = 15;
   }
   tilesX = Math.ceil(15 * _root.Zoom) + 1;
   tilesY = Math.ceil(10 * _root.Zoom) + 2;
   _root.createEmptyMovieClip("Scene2",_root.getNextHighestDepth());
   _root.createEmptyMovieClip("Scene",_root.getNextHighestDepth());
   _root.attachMovie("LDMenu","M",_root.getNextHighestDepth());
   X = 0;
   while(X < tilesX)
   {
      Y = 0;
      while(Y < tilesY)
      {
         _root.Scene.attachMovie("LDTile","Tile" + X + "_" + Y,_root.Scene.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
         Y++;
      }
      X++;
   }
   MaxX = _root.GridXSize;
   MaxY = _root.GridYSize;
   _root.LDItemNum = _root.LDItemArray.length;
   i = 0;
   while(i < _root.LDItemNum)
   {
      _root.Scene2.attachMovie("LDItem","Item" + i,_root.Scene2.getNextHighestDepth());
      _root.Scene2["Item" + i].gotoAndStop(_root.LDItemArray[i][0]);
      _root.Scene2["Item" + i].num = i;
      _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
      _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
      if(_root.ItemNumRotatable(i) || _root.ItemNumFlippable(i))
      {
         if(Boolean(Number(_root.LDItemArray[i][3])) == true)
         {
            _root.Scene2["Item" + i]._xscale = 100;
         }
         else
         {
            _root.Scene2["Item" + i]._xscale = -100;
         }
         if(_root.ItemNumRotatable(i))
         {
            _root.Scene2["Item" + i]._rotation = _root.LDItemArray[i][4];
         }
      }
      i++;
   }
   _root.LDcentercam();
   xspeed = 0;
   yspeed = 0;
   changeTilesX = 0;
   changeTilesX = 0;
   onEnterFrame = function()
   {
      if(_root.LDOpenNextWindow !== "")
      {
         trace(_root.LDOpenNextWindow);
         M.Window1.swapDepths(frontwindow);
         M.Window1.gotoAndStop("blank");
         M.Window1.frame = _root.LDOpenNextWindow;
         M.Window1.gotoAndStop(M.Window1.frame);
         M.Window1._x = 50 + random(10);
         M.Window1._y = 20 + random(10);
         _root.LDOpenNextWindow = "";
      }
      MaxX = _root.GridXSize;
      MaxY = _root.GridYSize;
      if(Key.isDown(33) && !Key.isDown(34) || Key.isDown(187) && !Key.isDown(189))
      {
         _root.Zoom -= 0.01;
         changezoom = true;
      }
      if(Key.isDown(34) && !Key.isDown(33) || Key.isDown(189) && !Key.isDown(187))
      {
         _root.Zoom += 0.01;
         changezoom = true;
      }
      _root.Zoom = Math.min(_root.Zoom,1.5);
      _root.Zoom = Math.max(_root.Zoom,0.75);
      if(changezoom == true)
      {
         _root.Scene._xscale = 100 / Zoom;
         _root.Scene._yscale = 100 / Zoom;
         _root.Scene2._xscale = 100 / Zoom;
         _root.Scene2._yscale = 100 / Zoom;
         if(Math.ceil(15 * _root.Zoom) + 1 !== tilesX || tilesY !== Math.ceil(10 * _root.Zoom) + 2)
         {
            X = 0;
            while(X < tilesX)
            {
               Y = 0;
               while(Y < tilesY)
               {
                  _root.Scene["Tile" + X + "_" + Y].removeMovieClip();
                  Y++;
               }
               X++;
            }
            tilesX = Math.ceil(15 * _root.Zoom) + 1;
            tilesY = Math.ceil(10 * _root.Zoom) + 2;
            X = 0;
            while(X < tilesX)
            {
               Y = 0;
               while(Y < tilesY)
               {
                  _root.Scene.attachMovie("LDTile","Tile" + X + "_" + Y,_root.Scene.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
                  Y++;
               }
               X++;
            }
         }
      }
      if(Key.isDown(39) && !Key.isDown(37))
      {
         xspeed += ((- _root.ScrollSpeed) * 3 - xspeed) / 10;
      }
      if(Key.isDown(37) && !Key.isDown(39))
      {
         xspeed += (_root.ScrollSpeed * 3 - xspeed) / 10;
      }
      if(Key.isDown(38) && !Key.isDown(40))
      {
         yspeed += (_root.ScrollSpeed * 3 - yspeed) / 10;
      }
      if(Key.isDown(40) && !Key.isDown(38))
      {
         yspeed += ((- _root.ScrollSpeed) * 3 - yspeed) / 10;
      }
      xspeed *= 0.85;
      yspeed *= 0.85;
      MouseXspeed = _root.Scene._xmouse + _root.Scene._x - oldmouseX;
      MouseYspeed = _root.Scene._ymouse + _root.Scene._y - oldmouseY;
      mouseX = oldmouseX;
      mouseY = oldmouseY;
      repeats = Math.floor((Math.abs(xspeed) + Math.abs(yspeed) + Math.abs(MouseXspeed) + Math.abs(MouseYspeed)) / 30) + 1;
      if(!(_root.MouseDown == true && _root.MenuAccess !== true))
      {
         repeats = 1;
      }
      i = 0;
      while(i < repeats)
      {
         viewX = Math.min(viewX,320);
         viewY = Math.min(viewY,272);
         viewX = Math.max(viewX,(- MaxX) * 32 + 320);
         viewY = Math.max(viewY,(- MaxY) * 32 + 32);
         offX = Math.round(viewX) % 32;
         offY = Math.round(viewY) % 32;
         X2 = Math.floor((- Math.round(viewX - offX)) / 32);
         Y2 = Math.floor((- Math.round(viewY - offY)) / 32);
         if(_root.MouseDown == true && _root.MenuAccess !== true && _root.PlaceMode == "Tile")
         {
            Xmouse = Math.round((mouseX - offX + 16) / 32);
            Ymouse = Math.round((mouseY - offY + 16) / 32);
            _root.Scene["Tile" + Xmouse + "_" + Ymouse]._alpha = 100;
            if(!(X2 + Xmouse < 0 || Y2 + Ymouse < 0 || X2 + Xmouse > MaxX - 1 || Y2 + Ymouse > MaxY - 1))
            {
               if(!Key.isDown(16))
               {
                  if(_root.PlacingTile !== undefined)
                  {
                     _root.TileArray[X2 + Xmouse][Y2 + Ymouse] = _root.PlacingTile;
                  }
               }
               else
               {
                  _root.TileArray[X2 + Xmouse][Y2 + Ymouse] = _root.BlankTile;
               }
            }
            _root.Scene["Tile" + Xmouse + "_" + Ymouse].gotoAndStop(_root.TileArray[X2 + Xmouse][Y2 + Ymouse]);
            if(X2 + Xmouse < 0 || Y2 + Ymouse < 0 || X2 + Xmouse > MaxX - 1 || Y2 + Ymouse > MaxY - 1)
            {
               _root.Scene["Tile" + Xmouse + "_" + Ymouse].gotoAndStop("Bounds");
            }
         }
         mouseX += MouseXspeed / repeats;
         mouseY += MouseYspeed / repeats;
         viewX += xspeed / repeats;
         viewY += yspeed / repeats;
         i++;
      }
      oldmouseX = _root.Scene._xmouse + _root.Scene._x;
      oldmouseY = _root.Scene._ymouse + _root.Scene._y;
      viewX = Math.min(viewX,320);
      viewY = Math.min(viewY,272);
      viewX = Math.max(viewX,(- MaxX) * 32 + 320);
      viewY = Math.max(viewY,(- MaxY) * 32 + 32);
      offX = Math.round(viewX) % 32;
      offY = Math.round(viewY) % 32;
      X2 = Math.floor((- Math.round(viewX - offX)) / 32);
      Y2 = Math.floor((- Math.round(viewY - offY)) / 32);
      _root.Scene._x = offX / (100 / _root.Scene._xscale);
      _root.Scene._y = offY / (100 / _root.Scene._xscale);
      changeTilesX += Math.abs(xspeed);
      changeTilesY += Math.abs(yspeed);
      while(changeTilesX > 32)
      {
         changeTilesX -= 16;
         changed = true;
      }
      while(changeTilesY > 32)
      {
         changeTilesY -= 16;
         changed = true;
      }
      while(changeTilesX < -32)
      {
         changeTilesX += 16;
         changed = true;
      }
      while(changeTilesY < -32)
      {
         changeTilesY += 16;
         changed = true;
      }
      changed = true;
      if(changed !== false)
      {
         X = 0;
         while(X < tilesX)
         {
            Y = 0;
            while(Y < tilesY)
            {
               _root.Scene["Tile" + X + "_" + Y].gotoAndStop(_root.TileArray[X2 + X][Y2 + Y]);
               if(X2 + X < 0 || Y2 + Y < 0 || X2 + X > MaxX - 1 || Y2 + Y > MaxY - 1)
               {
                  _root.Scene["Tile" + X + "_" + Y].gotoAndStop("Bounds");
               }
               Y++;
            }
            X++;
         }
      }
      changed = false;
      Xmouse = Math.round((_root.Scene._xmouse + _root.Scene._x - offX + 16) / 32);
      Ymouse = Math.round((_root.Scene._ymouse + _root.Scene._y - offY + 16) / 32);
      oldtile._alpha = 100;
      if(_root.MenuAccess !== true && _root.PlaceMode == "Tile")
      {
         _root.Scene["Tile" + Xmouse + "_" + Ymouse]._alpha = 50;
         oldtile = _root.Scene["Tile" + Xmouse + "_" + Ymouse];
      }
      if(_root.MenuAccess !== true && _root.PlaceMode == "Item")
      {
         M.item._visible = true;
         M.item._alpha = 75;
         M.item.gotoAndStop(_root.PlacingItem);
         M.item._x = _root._xmouse;
         M.item._y = _root._ymouse;
         if(Key.isDown(17))
         {
            M.item._x = (Math.round((_root._xmouse * Zoom - viewX) / 8) * 8 + viewX) / Zoom;
            M.item._y = (Math.round((_root._ymouse * Zoom - viewY) / 8) * 8 + viewY) / Zoom;
         }
         M.item._xscale = 100 / Zoom;
         M.item._yscale = 100 / Zoom;
         myColor = new Color(M.item);
         colorTrans = new Object();
         colorTrans = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
         if(String(_root.PlacingItem) == "43")
         {
            tryx = Math.round((_root._xmouse * Zoom - viewX) / 32) * 32;
            M.item._x = (Math.round((_root._xmouse * Zoom - viewX) / 32) * 32 + viewX) / Zoom;
            shortest = 1000;
            h = 0;
            while(h < LDItemNum)
            {
               if(LDItemArray[h][0] == "43")
               {
                  dx = Math.abs(LDItemArray[h][1] - tryx);
                  if(dx < shortest)
                  {
                     shortest = dx;
                  }
               }
               h++;
            }
            if(tryx < 600 || tryx > _root.GridXSize * 32 - 600 || shortest < 600)
            {
               colorTrans = {ra:100,rb:100,ga:50,gb:0,ba:50,bb:0,aa:100,ab:0};
            }
         }
         myColor.setTransform(colorTrans);
      }
      else
      {
         M.item._x = -200;
         M.item._visible = false;
      }
      if(_root.MenuAccess !== true && _root.PlaceMode == "Item" && _root.MousePressed == true)
      {
         Itemplaced = true;
         if(String(_root.PlacingItem) == "1" && _root.LDItemNum >= 1)
         {
            h = 0;
            while(h < LDItemNum)
            {
               if(LDItemArray[h][0] == "1")
               {
                  i = h;
                  _root.LDItemArray[i] = [String(_root.PlacingItem),Math.round(_root._xmouse * Zoom - viewX),Math.round(_root._ymouse * Zoom - viewY)];
                  break;
               }
               h++;
            }
         }
         else if(String(_root.PlacingItem) == "43")
         {
            shortest = 1000;
            h = 0;
            while(h < LDItemNum)
            {
               if(LDItemArray[h][0] == "43")
               {
                  dx = Math.abs(LDItemArray[h][1] - tryx);
                  if(dx < shortest)
                  {
                     shortest = dx;
                  }
               }
               h++;
            }
            if(tryx < 600 || tryx > _root.GridXSize * 32 - 600 || shortest < 600)
            {
               Itemplaced = false;
            }
            else
            {
               i = _root.LDItemNum;
               _root.LDItemNum = _root.LDItemNum + 1;
               _root.LDItemArray.push([String(_root.PlacingItem),Math.round(_root._xmouse * Zoom - viewX),Math.round(_root._ymouse * Zoom - viewY)]);
               _root.Scene2.attachMovie("LDItem","Item" + i,_root.Scene2.getNextHighestDepth());
            }
         }
         else
         {
            i = _root.LDItemNum;
            _root.LDItemNum = _root.LDItemNum + 1;
            _root.LDItemArray.push([String(_root.PlacingItem),Math.round(_root._xmouse * Zoom - viewX),Math.round(_root._ymouse * Zoom - viewY)]);
            _root.Scene2.attachMovie("LDItem","Item" + i,_root.Scene2.getNextHighestDepth());
         }
         if(Itemplaced == true)
         {
            _root.Scene2["Item" + i].gotoAndStop(_root.LDItemArray[i][0]);
            _root.Scene2["Item" + i].num = i;
            if(Key.isDown(17))
            {
               _root.LDItemArray[i][1] = Math.round(_root.LDItemArray[i][1] / 8) * 8;
               _root.LDItemArray[i][2] = Math.round(_root.LDItemArray[i][2] / 8) * 8;
            }
            _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
            _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
            trace(LDItemArray[i][0]);
            if(_root.ItemNumRotatable(i))
            {
               _root.LDItemArray[i][3] = 1;
               _root.LDItemArray[i][4] = 0;
            }
            if(_root.ItemNumFlippable(i))
            {
               _root.LDItemArray[i][3] = 1;
            }
            if(LDItemArray[i][0] == "1")
            {
               _root.LDItemArray[i][3] = 0;
               _root.LDItemArray[i][4] = 0;
               _root.LDItemArray[i][5] = "Right";
            }
            if(LDItemArray[i][0] == "7")
            {
               _root.LDItemArray[i][3] = 3;
               _root.LDItemArray[i][4] = 0;
               _root.LDItemArray[i][5] = 30;
               _root.LDItemArray[i][6] = 0;
               _root.LDItemArray[i][7] = 0;
               _root.LDItemArray[i][8] = 0;
               _root.LDItemArray[i][9] = "Right";
               _root.LDItemArray[i][10] = "none";
               _root.LDItemArray[i][11] = 0.5;
               _root.LDItemArray[i][12] = 1;
               _root.LDItemArray[i][13] = Number(false);
               _root.LDItemArray[i][14] = 25;
            }
            if(LDItemArray[i][0] == "8")
            {
               _root.LDItemArray[i][3] = 2;
               _root.LDItemArray[i][4] = 2;
               _root.LDItemArray[i][5] = 50;
               _root.LDItemArray[i][6] = 0;
               _root.LDItemArray[i][7] = 1;
            }
            if(LDItemArray[i][0] == "9")
            {
               _root.LDItemArray[i][3] = 0;
               _root.LDItemArray[i][4] = 0;
               _root.LDItemArray[i][5] = 0;
               _root.LDItemArray[i][6] = 0;
               _root.LDItemArray[i][7] = 0;
               _root.LDItemArray[i][8] = 0;
               _root.LDItemArray[i][9] = "Right";
               _root.LDItemArray[i][10] = "none";
               _root.LDItemArray[i][11] = 0.5;
               _root.LDItemArray[i][12] = 80;
               _root.LDItemArray[i][13] = 1;
               _root.LDItemArray[i][14] = "Left";
               _root.LDItemArray[i][15] = 3;
               _root.LDItemArray[i][16] = 72;
               _root.LDItemArray[i][17] = Number(true);
            }
            if(LDItemArray[i][0] == "18")
            {
               _root.LDItemArray[i][3] = 2;
               _root.LDItemArray[i][4] = 0;
            }
            if(LDItemArray[i][0] == "19")
            {
               _root.LDItemArray[i][3] = 2;
               _root.LDItemArray[i][4] = 0;
            }
            if(LDItemArray[i][0] == "37")
            {
               _root.LDItemArray[i][3] = 15;
               _root.LDItemArray[i][4] = 0;
            }
            if(LDItemArray[i][0] == "38")
            {
               _root.LDItemArray[i][3] = 0;
               _root.LDItemArray[i][4] = 500;
               _root.LDItemArray[i][5] = 400;
            }
            if(LDItemArray[i][0] == "39")
            {
               _root.LDItemArray[i][3] = _root.LDItemArray[i][1];
               _root.LDItemArray[i][4] = _root.LDItemArray[i][2];
               _root.LDItemArray[i][5] = 1;
            }
            if(LDItemArray[i][0] == "40")
            {
               _root.LDItemArray[i][3] = _root.LDItemArray[i][1];
               _root.LDItemArray[i][4] = _root.LDItemArray[i][2];
            }
            if(LDItemArray[i][0] == "71")
            {
               _root.LDItemArray[i][3] = "Both";
               _root.LDItemArray[i][4] = 0;
               _root.LDItemArray[i][5] = 3;
               _root.LDItemArray[i][6] = 100;
               _root.LDItemArray[i][7] = 0;
               _root.LDItemArray[i][8] = 1;
               _root.LDItemArray[i][9] = 0;
            }
            if(LDItemArray[i][0] == "73")
            {
               _root.LDItemArray[i][3] = "This is a sign...";
            }
            if(LDItemArray[i][0] == "74")
            {
               _root.LDItemArray[i][3] = 20;
            }
            if(LDItemArray[i][0] == "78" || LDItemArray[i][0] == "79" | LDItemArray[i][0] == "80" || LDItemArray[i][0] == "81")
            {
               _root.LDItemArray[i][3] = 30;
            }
            if(_root.ItemNumRotatable(i) || _root.ItemNumFlippable(i))
            {
               if(Boolean(Number(_root.LDItemArray[i][3])) == true)
               {
                  _root.Scene2["Item" + i]._xscale = 100;
               }
               else
               {
                  _root.Scene2["Item" + i]._xscale = -100;
               }
               if(_root.ItemNumRotatable(i))
               {
                  _root.Scene2["Item" + i]._rotation = _root.LDItemArray[i][4];
               }
            }
            if(!Key.isDown(16))
            {
               _root.PlaceMode = "ItemSelect";
            }
            if(Number(LDItemArray[i][0]) >= 7 && Number(LDItemArray[i][0]) < 28 || Number(LDItemArray[i][0]) == 37)
            {
               _root.RestartItems();
            }
         }
      }
      _root.Scene2["Item" + olditemselected]._alpha = 100;
      if((_root.PlaceMode == "ItemSelect" || _root.PlaceMode == "ItemErase") && MouseDown == false)
      {
         mousex = _root._xmouse * Zoom - viewX;
         mousey = _root._ymouse * Zoom - viewY;
         mouseoverArray = [];
         i = LDItemNum - 1;
         while(i >= 0)
         {
            if(Math.abs(mousex - LDItemArray[i][1]) < _root.Scene2["Item" + i]._width && Math.abs(mousey - LDItemArray[i][2]) < _root.Scene2["Item" + i]._height)
            {
               if(_root.Scene2["Item" + i].hitTest(_root._xmouse,_root._ymouse,true) && !_root.Scene2["Item" + i].nomouse.hitTest(_root._xmouse,_root._ymouse,true))
               {
                  mouseoverArray.push(i);
               }
            }
            i--;
         }
         if(mouseoverArray.length > 0)
         {
            closest = 300;
            i = 0;
            while(i < mouseoverArray.length)
            {
               d = Math.sqrt((mousex - LDItemArray[mouseoverArray[i]][1]) * (mousex - LDItemArray[mouseoverArray[i]][1]) + (mousey - LDItemArray[mouseoverArray[i]][2]) * (mousey - LDItemArray[mouseoverArray[i]][2]));
               if(d < closest)
               {
                  closest = d;
                  closestnum = mouseoverArray[i];
               }
               i++;
            }
            olditemselected = closestnum;
            _root.Scene2["Item" + olditemselected]._alpha = 50;
         }
         else
         {
            olditemselected = "";
         }
      }
      if(Key.isDown(46))
      {
         if(_root.LastItemClicked !== "" && String(_root.LDItemArray[_root.LastItemClicked][0]) !== "1")
         {
            _root.DeleteItem(_root.LastItemClicked);
            _root.LastItemClicked = "";
            M.Window1.frame = "";
            M.Window1.gotoAndStop("blank");
            M.Window1._y = 1000;
         }
         else if(_root.PlaceMode == "ItemErase" && olditemselected !== "" && String(_root.LDItemArray[olditemselected][0]) !== "1")
         {
            _root.DeleteItem(olditemselected);
            _root.LastItemClicked = "";
         }
      }
      if(MousePressed == false && MouseDown == true)
      {
         _root.LastItemClicked = "";
      }
      if(_root.PlaceMode == "ItemSelect" && MouseDown == true && olditemselected !== "")
      {
         if(MousePressed == false)
         {
            _root.LastItemClicked = olditemselected;
            _root.Scene2["Item" + olditemselected]._alpha = 75;
            if(String(_root.LDItemArray[olditemselected][0]) !== "43")
            {
               _root.LDItemArray[olditemselected][1] = Math.round(_root._xmouse * Zoom - viewX - selectx);
               _root.LDItemArray[olditemselected][2] = Math.round(_root._ymouse * Zoom - viewY - selecty);
               if(Key.isDown(17))
               {
                  _root.LDItemArray[olditemselected][1] = Math.round(_root.LDItemArray[olditemselected][1] / 8) * 8;
                  _root.LDItemArray[olditemselected][2] = Math.round(_root.LDItemArray[olditemselected][2] / 8) * 8;
               }
               _root.Scene2["Item" + olditemselected]._x = _root.LDItemArray[olditemselected][1];
               _root.Scene2["Item" + olditemselected]._y = _root.LDItemArray[olditemselected][2];
               if(_root.ItemNumRotatable(olditemselected) || _root.ItemNumFlippable(olditemselected))
               {
                  if(Boolean(Number(_root.LDItemArray[olditemselected][3])) == true)
                  {
                     _root.Scene2["Item" + olditemselected]._xscale = 100;
                  }
                  else
                  {
                     _root.Scene2["Item" + olditemselected]._xscale = -100;
                  }
                  if(_root.ItemNumRotatable(olditemselected))
                  {
                     _root.Scene2["Item" + olditemselected]._rotation = _root.LDItemArray[olditemselected][4];
                  }
               }
            }
         }
         else
         {
            selectx = _root._xmouse * Zoom - viewX - _root.Scene2["Item" + olditemselected]._x;
            selecty = _root._ymouse * Zoom - viewY - _root.Scene2["Item" + olditemselected]._y;
            trace("Open item Window");
            M.Window1.swapDepths(frontwindow);
            M.Window1.gotoAndStop("blank");
            trace(_root.ItemNumRotatable(olditemselected));
            if(_root.ItemNumRotatable(olditemselected) == true)
            {
               M.Window1.frame = "ItemView2";
            }
            else if(_root.ItemNumFlippable(olditemselected))
            {
               M.Window1.frame = "ItemView3";
            }
            else
            {
               M.Window1.frame = "ItemView";
            }
            if(_root.LDItemArray[olditemselected][0] == 1)
            {
               M.Window1.frame = "ItemView0";
            }
            if(_root.LDItemArray[olditemselected][0] == 7)
            {
               M.Window1.frame = "GreenPLView";
            }
            if(_root.LDItemArray[olditemselected][0] == 8)
            {
               M.Window1.frame = "GreenPLView2";
            }
            if(_root.LDItemArray[olditemselected][0] == 9)
            {
               M.Window1.frame = "BlockView";
            }
            if(_root.LDItemArray[olditemselected][0] == 18 || _root.LDItemArray[olditemselected][0] == 19 || _root.LDItemArray[olditemselected][0] == 37)
            {
               M.Window1.frame = "ItemFlippingPL";
            }
            if(_root.LDItemArray[olditemselected][0] == 38)
            {
               M.Window1.frame = "ItemWater";
            }
            if(_root.LDItemArray[olditemselected][0] == 39)
            {
               M.Window1.frame = "ItemDoor";
            }
            if(_root.LDItemArray[olditemselected][0] == 40)
            {
               M.Window1.frame = "ItemPipe";
            }
            if(_root.LDItemArray[olditemselected][0] == 71)
            {
               M.Window1.frame = "ItemBill";
            }
            if(_root.LDItemArray[olditemselected][0] == 73)
            {
               M.Window1.frame = "ItemSign";
            }
            if(_root.LDItemArray[olditemselected][0] == 74 || _root.LDItemArray[olditemselected][0] == 78 || _root.LDItemArray[olditemselected][0] == 79 || _root.LDItemArray[olditemselected][0] == 80 || _root.LDItemArray[olditemselected][0] == 81)
            {
               M.Window1.frame = "ItemSwitch";
            }
            M.Window1.itemnum = olditemselected;
            M.Window1._x = _root._xmouse - 15;
            if(_root._ymouse > 200)
            {
               M.Window1._y = _root._ymouse - 170;
            }
            else
            {
               M.Window1._y = _root._ymouse + 20;
            }
         }
      }
      if(_root.PlaceMode == "ItemErase" && MouseDown == true && olditemselected !== "" && String(_root.LDItemArray[olditemselected][0]) !== "1")
      {
         if(MousePressed == true)
         {
            _root.DeleteItem(olditemselected);
            _root.LastItemClicked = "";
         }
      }
      _root.Scene2._x = viewX / Zoom;
      _root.Scene2._y = viewY / Zoom;
      changezoom = false;
      _root.MousePressed = false;
      if(Key.isDown(17) && Key.isDown(13))
      {
         _root.TestLevel();
      }
   };
   _root.M.onMouseDown = function()
   {
      if(_root.MenuAccess !== true)
      {
         _root.MousePressed = true;
         _root.MouseDown = true;
      }
   };
   _root.M.onMouseUp = function()
   {
      _root.MouseDown = false;
   };
};
_root.DeleteItem = function(num)
{
   _root.LDItemArray[num] = _root.LDItemArray[_root.LDItemNum - 1];
   _root.LDItemArray.pop();
   _root.LDItemNum = _root.LDItemNum - 1;
   _root.Scene2["Item" + _root.LDItemNum].removeMovieClip();
   i = num;
   i = 0;
   while(i < _root.LDItemNum)
   {
      _root.Scene2["Item" + i].removeMovieClip();
      _root.Scene2.attachMovie("LDItem","Item" + i,_root.Scene2.getNextHighestDepth());
      _root.Scene2["Item" + i].gotoAndStop(_root.LDItemArray[i][0]);
      _root.Scene2["Item" + i].num = i;
      _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
      _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
      if(_root.ItemNumRotatable(i) || _root.ItemNumFlippable(i))
      {
         if(Boolean(Number(_root.LDItemArray[i][3])) == true)
         {
            _root.Scene2["Item" + i]._xscale = 100;
         }
         else
         {
            _root.Scene2["Item" + i]._xscale = -100;
         }
         if(_root.ItemNumRotatable(i))
         {
            _root.Scene2["Item" + i]._rotation = _root.LDItemArray[i][4];
         }
      }
      i++;
   }
};
_root.RestartItems = function()
{
   i = 0;
   while(i < _root.LDItemNum)
   {
      _root.Scene2["Item" + i].removeMovieClip();
      _root.Scene2.attachMovie("LDItem","Item" + i,_root.Scene2.getNextHighestDepth());
      _root.Scene2["Item" + i].gotoAndStop(_root.LDItemArray[i][0]);
      _root.Scene2["Item" + i].num = i;
      _root.Scene2["Item" + i]._x = _root.LDItemArray[i][1];
      _root.Scene2["Item" + i]._y = _root.LDItemArray[i][2];
      if(_root.ItemNumRotatable(i) || _root.ItemNumFlippable(i))
      {
         if(Boolean(Number(_root.LDItemArray[i][3])) == true)
         {
            _root.Scene2["Item" + i]._xscale = 100;
         }
         else
         {
            _root.Scene2["Item" + i]._xscale = -100;
         }
         if(_root.ItemNumRotatable(i))
         {
            _root.Scene2["Item" + i]._rotation = _root.LDItemArray[i][4];
         }
      }
      i++;
   }
};
_root.CompactNumbers = function(array2)
{
   array = [];
   var _loc3_ = 0;
   var _loc1_;
   while(_loc3_ < array2.length)
   {
      _loc1_ = 0;
      while(_loc1_ < array2[_loc3_].length)
      {
         array.push(array2[_loc3_][_loc1_]);
         _loc1_ = _loc1_ + 1;
      }
      _loc3_ = _loc3_ + 1;
   }
   var _loc2_ = 0;
   var _loc6_ = String(array[0]);
   var _loc5_ = "";
   i = 0;
   while(i < array.length)
   {
      if(array[i] !== "100")
      {
         char1 = Math.floor(array[i] / 75);
         char2 = array[i] - char1 * 75;
         if(String(array[i]) !== String(array[i + 1]))
         {
            _loc5_ += String.fromCharCode(char1 + 49) + String.fromCharCode(char2 + 49);
            if(_loc2_ > 0)
            {
               _loc5_ += "*" + (_loc2_ + 1) + "*";
            }
            _loc2_ = 0;
         }
         else
         {
            _loc2_ = _loc2_ + 1;
         }
      }
      else if(String(array[i]) !== String(array[i + 1]))
      {
         _loc5_ += "0";
         if(_loc2_ > 0)
         {
            _loc5_ += "*" + (_loc2_ + 1) + "*";
         }
         _loc2_ = 0;
      }
      else
      {
         _loc2_ = _loc2_ + 1;
      }
      _loc6_ = String(array[i]);
      i++;
   }
   if(_loc2_ > 1)
   {
      if(_loc6_ !== "100")
      {
         char1 = Math.floor(_loc6_ / 75);
         char2 = _loc6_ - char1 * 75;
         _loc5_ += String.fromCharCode(char1 + 49) + String.fromCharCode(char2 + 49) + "*" + _loc2_ + "*";
      }
      else
      {
         _loc5_ += "0*" + _loc2_ + "*";
      }
   }
   return _loc5_;
};
_root.ExpandNumbers = function(st)
{
   var _loc3_ = [];
   _loc3_[0] = [];
   h = 0;
   j = 0;
   i = 0;
   while(i < st.length)
   {
      if(st.charAt(i) == "0")
      {
         i -= 1;
         _loc3_[h].push("100");
         if(st.charAt(i + 2) == "*")
         {
            z = i + 3;
            numstring = "";
            while(st.charAt(z) !== "*")
            {
               numstring += String(st.charAt(z));
               z++;
            }
            z = 0;
            while(z < Number(numstring) - 1)
            {
               if(_loc3_[h].length >= _root.GridYSize)
               {
                  h++;
                  _loc3_.push([]);
               }
               _loc3_[h].push("100");
               z++;
            }
            i += 2 + numstring.length;
         }
      }
      else
      {
         _loc3_[h].push((st.charCodeAt(i) - 49) * 75 + (st.charCodeAt(i + 1) - 49));
         if(st.charAt(i + 2) == "*")
         {
            z = i + 3;
            numstring = "";
            while(st.charAt(z) !== "*")
            {
               numstring += String(st.charAt(z));
               z++;
            }
            z = 0;
            while(z < Number(numstring) - 1)
            {
               if(_loc3_[h].length >= _root.GridYSize)
               {
                  h++;
                  _loc3_.push([]);
               }
               _loc3_[h].push((st.charCodeAt(i) - 49) * 75 + (st.charCodeAt(i + 1) - 49));
               z++;
            }
            i += 2 + numstring.length;
         }
      }
      if(_loc3_[h].length >= _root.GridYSize)
      {
         h++;
         _loc3_.push([]);
      }
      i += 2;
   }
   return _loc3_;
};
_root.GetCode = function()
{
   time = getTimer();
   code = String(_root.GridXSize) + "x" + String(_root.GridYSize);
   code += "~" + _root.CompactNumbers(_root.TileArray);
   code += "~";
   i = 0;
   while(i < _root.LDItemArray.length)
   {
      if(i !== 0)
      {
         code += "|";
      }
      code += "" + _root.LDItemArray[i];
      i++;
   }
   code += "~" + _root.LDMusic;
   code += "~" + _root.LDCourseBG;
   code += "~" + escape(_root.LDCourseName);
   trace("Timed: " + (getTimer() - time) + " ms");
   return code;
};
_root.LoadCourseFromCode = function(code)
{
   trace("Load from Code");
   _root.RemoveLevelDesigner();
   _root.TileArray = [];
   _root.GridXSize = 50;
   _root.GridYSize = 30;
   _root.LDItemArray = [];
   avar = "";
   code = code.split("~");
   _root.GridXSize = Number(code[0].split("x")[0]);
   _root.GridYSize = Number(code[0].split("x")[1]);
   _root.LDMusic = Number(code[3]);
   _root.LDCourseBG = Number(code[4]);
   _root.LDCourseName = unescape(String(code[5]));
   _root.TileArray = _root.ExpandNumbers(code[1]);
   _root.LDItemArray = code[2].split("|");
   i = 0;
   while(i < _root.LDItemArray.length)
   {
      _root.LDItemArray[i] = _root.LDItemArray[i].split(",");
      i++;
   }
   MaxX = _root.GridXSize;
   MaxY = _root.GridYSize;
   _root.CreateLevelDesigner();
};
_root.LoadCourseFromSave = function(filenum)
{
   _root.LoadLDCourse(filenum);
   _root.LoadCourseFromCode(_root.GetCode());
};
_root.LDcentercam = function()
{
   _root.Zoom = 1;
   _root.changezoom = true;
   _root.viewX = 0;
   _root.viewY = 0;
   i = 0;
   while(i < _root.LDItemNum)
   {
      if(_root.LDItemArray[i][0] == "1")
      {
         _root.viewX = - _root.LDItemArray[i][1] + 160;
         _root.viewY = - _root.LDItemArray[i][2] + 136;
         break;
      }
      i++;
   }
};
_root.LDDeleteF = function()
{
   _root.RemoveLevelDesigner();
   _root.EraseLevel();
   _root.CreateLevelDesigner();
};
_root.RemoveLevelDesigner = function()
{
   _root.Scene.removeMovieClip();
   _root.Scene2.removeMovieClip();
   _root.M.removeMovieClip();
};
_root.RemoveLevelDesignerToCastle = function()
{
};
_root.LDSongIntroArray = ["","PrincessPeachesCastle-Intro","Bombomb Battlefield Intro","Secret Course - Intro","SuperMarioGalaxyPurpleCoin-Intro","Rainbow Ride - Intro","Boos Mansion - Intro","HazyMazeCave-Intro","Snowmans Land - Intro","Melty Molten Galaxy - Intro","Gritzy Desert (SSBB) Intro","BowserTrapIntro","Boss Music 64 - Intro","Boss Music","BuoyBaseGalaxy(Dry)-Intro","Airship Theme Intro","Super Mario Galaxy Bowser Battle 2 - Intro","Super Mario Galaxy Bowser Battle - Intro","InsidetheCastleWalls"];
_root.LDSongRepeatArray = ["","PrincessPeachesCastle-Repeat","Bombomb Battlefield Repeat","Secret Course - Repeat","SuperMarioGalaxyPurpleCoin-Repeat","Rainbow Ride - Repeat","Boos Mansion - Repeat","HazyMazeCave-Rock","Snowmans Land - Repeat","Melty Molten Galaxy - Repeat","Gritzy Desert (SSBB) Repeat","BowserTrapRepeat","Boss Music 64 - Repeat","Boss Music","BuoyBaseGalaxy(Dry)-Repeat","Airship Theme Repeat","Super Mario Galaxy Bowser Battle 2 - Repeat","Super Mario Galaxy Bowser Battle - Repeat","InsidetheCastleWalls"];
_root.TestLevel = function()
{
   startX = 80;
   startY = 80;
   _root.LevelItems = deepObjectCopy(_root.LDItemArray);
   i = 0;
   while(i < LevelItems.length)
   {
      if(LevelItems[i][0] == "1")
      {
         startX = LevelItems[i][1];
         startY = LevelItems[i][2];
         startXspeed = LevelItems[i][3];
         startYspeed = LevelItems[i][4];
         if(LevelItems[i][5] == "Right")
         {
            _root.CharXscale = -100;
         }
         else
         {
            _root.CharXscale = 100;
         }
      }
      i++;
   }
   _root.Fluddpow = "";
   _root.Restartcoins();
   _root.OrangeBlockPLCount = 0;
   PlayLevelDesignerLevel(startX,startY,startXspeed,startYspeed,"Fromwhite",true);
   _root.PlayMusicAndIntro();
};
_root.PlayLevelDesignerLevel = function(MarioX, MarioY, MarioXspeed, MarioYspeed, transition, messageboolean)
{
   _root.LevelDesignerMenu = false;
   _root.RemoveLevelDesigner();
   _root.LDRedCoin = 0;
   _root.LDSilverStar = 0;
   _root.LevelTiles = deepObjectCopy(_root.TileArray);
   _root.LevelItems = deepObjectCopy(_root.LDItemArray);
   _root.LevelXsize = deepObjectCopy(_root.GridXSize);
   _root.LevelYsize = deepObjectCopy(_root.GridYSize);
   _root.LevelItemNum = _root.LDItemNum;
   i = 0;
   while(i < _root.LDItemNum)
   {
      if(_root.LDItemArray[i][0] == "3")
      {
         _root.LDRedCoin = _root.LDRedCoin + 1;
      }
      if(_root.LDItemArray[i][0] == "5")
      {
         _root.LDSilverStar = _root.LDSilverStar + 1;
      }
      i++;
   }
   startX = MarioX;
   startY = MarioY;
   startXspeed = MarioXspeed;
   startYspeed = MarioYspeed;
   LevelSplit = false;
   LevelSplitX = [];
   i = 0;
   while(i < _root.LDItemNum)
   {
      if(_root.LDItemArray[i][0] == "43")
      {
         LevelSplit = true;
         LevelSplitX.push(_root.LDItemArray[i][1]);
      }
      i++;
   }
   CamEdgeRightXoff = 0;
   CamEdgeLeftXoff = 0;
   if(LevelSplit == true)
   {
      LevelSplitX.push(0);
      LevelSplitX.push(_root.GridXSize * 32);
      LevelSplitX.sort(Array.NUMERIC);
      _root.LevelTiles = [];
      startBlock = 0;
      leftof = true;
      i = 1;
      while(i < LevelSplitX.length)
      {
         if(startX > LevelSplitX[i - 1] && startX < LevelSplitX[i])
         {
            startBlock = i;
            break;
         }
         i++;
      }
      trace("Start Block " + startBlock);
      newWidth = Math.floor(LevelSplitX[startBlock] / 32);
      if(startBlock == 0)
      {
         leftWidth = 0;
      }
      else
      {
         leftWidth = Math.floor(LevelSplitX[startBlock - 1] / 32);
      }
      if(startBlock > 1)
      {
         CamEdgeLeftXoff = 96;
      }
      if(startBlock < LevelSplitX.length - 1)
      {
         CamEdgeRightXoff = -64;
      }
      _root.LevelItems = deepObjectCopy(_root.LDItemArray);
      X = leftWidth;
      while(X < newWidth)
      {
         _root.LevelTiles.push([""]);
         Y = 0;
         while(Y < _root.GridYSize)
         {
            _root.LevelTiles[X - leftWidth][Y] = _root.TileArray[X][Y];
            Y++;
         }
         X++;
      }
      _root.LevelXsize = newWidth - leftWidth;
      RightWarpX = (newWidth - leftWidth) * 32;
      LeftWarpX = 0;
      startX = Number(startX) - leftWidth * 32;
      if(startBlock > 1)
      {
         startX = Math.max(startX,96);
      }
      if(startBlock < LevelSplitX.length - 1)
      {
         startX = Math.min(startX,(newWidth - leftWidth) * 32 - 96);
      }
      i = 0;
      while(i < _root.LevelItemNum)
      {
         _root.LevelItems[i][1] = Number(_root.LevelItems[i][1]) - leftWidth * 32;
         if(_root.LevelItems[i][0] == "39" || _root.LevelItems[i][0] == "40")
         {
            _root.LevelItems[i][3] = Number(_root.LevelItems[i][3]) - leftWidth * 32;
         }
         i++;
      }
      newArray = [];
      i = 0;
      while(i < _root.LevelItemNum)
      {
         if(Number(_root.LevelItems[i][1]) > -100 && Number(_root.LevelItems[i][1]) < (newWidth - leftWidth) * 32 + 100)
         {
            newArray.push(_root.LevelItems[i]);
         }
         i++;
      }
      _root.LevelItems = newArray;
      _root.LevelItemNum = _root.LevelItems.length;
      _root.LevelYsize = _root.GridYSize;
   }
   _root.LevelDesignerCloseBackground = "";
   _root.LevelDesignerMidBackground = "";
   _root.LevelDesignerBackground = "";
   _root.LevelDesignerFarBackground = "";
   switch(_root.LDCourseBG)
   {
      case 1:
         _root.LevelDesignerBackground = "GrassandHillsBG";
         _root.LevelDesignerFarBackground = "CloudBackground";
         break;
      case 2:
         _root.LevelDesignerBackground = "GrassandHillsBG";
         _root.LevelDesignerFarBackground = "1-FarBG";
         break;
      case 3:
         _root.LevelDesignerFarBackground = "SecretLevelBG";
         break;
      case 4:
         _root.LevelDesignerFarBackground = "MovingCloudsBackground";
         break;
      case 5:
         _root.LevelDesignerBackground = "3-BG";
         _root.LevelDesignerFarBackground = "3-farBG";
         break;
      case 6:
         _root.LevelDesignerFarBackground = "2-farBG";
         break;
      case 7:
         _root.LevelDesignerFarBackground = "4-farBG";
         break;
      case 8:
         _root.LevelDesignerCloseBackground = "5-5CloseBG";
         _root.LevelDesignerMidBackground = "5-5MidBG";
         _root.LevelDesignerBackground = "5-5BG";
         _root.LevelDesignerFarBackground = "5-5FarBG";
         break;
      case 9:
         _root.LevelDesignerCloseBackground = "M1-1CloseBG";
         _root.LevelDesignerMidBackground = "M1-1MidBG";
         _root.LevelDesignerFarBackground = "M1-Background";
         break;
      case 10:
         _root.LevelDesignerFarBackground = "M1-Background";
         break;
      case 11:
         _root.LevelDesignerFarBackground = "4-farBG";
         _root.LevelDesignerCloseBackground = "8-UpperInsideCloseBG";
         _root.LevelDesignerMidBackground = "8-UpperInsideMidBG";
         break;
      case 12:
         _root.LevelDesignerFarBackground = "4-farBG";
         _root.LevelDesignerMidBackground = "LD-CastleLavaCloseBG";
         break;
      case 13:
         _root.LevelDesignerFarBackground = "SpaceFarBG";
         break;
      case 14:
         _root.LevelDesignerFarBackground = "4-farBG";
         _root.LevelDesignerCloseBackground = "8-InsideCloseBG";
         _root.LevelDesignerMidBackground = "8-InsideFarBG";
         break;
      case 15:
         _root.LevelDesignerFarBackground = "LD-Black";
   }
   trace("startX = " + startX);
   _root.StartChar("LevelDesigner",startX,startY,startX,startY,100,_root.CurrentPlayer,_root.Fluddpow,transition,false);
   if(LevelSplit == true)
   {
      _root.LDhighestcoinrestart = startBlock;
      _root.playingcourse = "LevelDesigner" + startBlock;
   }
   _root.Course.Platforms.createEmptyMovieClip("invisground",_root.Course.Platforms.getNextHighestDepth());
   X = 0;
   while(X < _root.LevelXsize)
   {
      Y = 0;
      while(Y < _root.LevelYsize)
      {
         casevar = Number(LevelTiles[X][Y]);
         _root.LDAttachTile(X,Y,casevar);
         Y++;
      }
      X++;
   }
   _root.Course.CamEdge.beginFill(3355443);
   _root.Course.CamEdge.moveTo(-32,-16);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32,-16);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32,-300);
   _root.Course.CamEdge.lineTo(-32,-300);
   _root.Course.CamEdge.lineTo(-32,-16);
   _root.Course.CamEdge.endFill();
   _root.Course.CamEdge.beginFill(3355443);
   _root.Course.CamEdge.moveTo(-16 + CamEdgeLeftXoff,-32);
   _root.Course.CamEdge.lineTo(-16 + CamEdgeLeftXoff,_root.LevelYsize * 32);
   _root.Course.CamEdge.lineTo(-300,_root.LevelYsize * 32);
   _root.Course.CamEdge.lineTo(-300,-32);
   _root.Course.CamEdge.lineTo(-16 + CamEdgeLeftXoff,-32);
   _root.Course.CamEdge.endFill();
   _root.Course.CamEdge.beginFill(3355443);
   _root.Course.CamEdge.moveTo(-32,_root.LevelYsize * 32 - 48);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32,_root.LevelYsize * 32 - 48);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32,_root.LevelYsize * 32 + 300);
   _root.Course.CamEdge.lineTo(-32,_root.LevelYsize * 32 + 300);
   _root.Course.CamEdge.lineTo(-32,_root.LevelYsize * 32 - 48);
   _root.Course.CamEdge.endFill();
   _root.Course.CamEdge.beginFill(3355443);
   _root.Course.CamEdge.moveTo(_root.LevelXsize * 32 - 48 + CamEdgeRightXoff,0);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32 - 48 + CamEdgeRightXoff,_root.LevelYsize * 32);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32 + 300,_root.LevelYsize * 32);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32 + 300,0);
   _root.Course.CamEdge.lineTo(_root.LevelXsize * 32 - 48 + CamEdgeRightXoff,0);
   _root.Course.CamEdge.endFill();
   _root.Course.Platforms.beginFill(65280);
   _root.Course.Platforms.moveTo(-32,-32);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32,-32);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32,-50);
   _root.Course.Platforms.lineTo(-32,-50);
   _root.Course.Platforms.lineTo(-32,-32);
   _root.Course.Platforms.endFill();
   _root.Course.Platforms.beginFill(65280);
   _root.Course.Platforms.moveTo(-32,-32);
   _root.Course.Platforms.lineTo(-32,_root.LevelYsize * 32 + 64);
   _root.Course.Platforms.lineTo(-50,_root.LevelYsize * 32 + 64);
   _root.Course.Platforms.lineTo(-50,-32);
   _root.Course.Platforms.lineTo(-32,-32);
   _root.Course.Platforms.endFill();
   _root.Course.Platforms.beginFill(65280);
   _root.Course.Platforms.moveTo(_root.LevelXsize * 32 - 32,-32);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32 - 32,_root.LevelYsize * 32 + 64);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32 + 28,_root.LevelYsize * 32 + 64);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32 + 28,-32);
   _root.Course.Platforms.lineTo(_root.LevelXsize * 32 - 32,-32);
   _root.Course.Platforms.endFill();
   _root.Course.Edge.beginFill(16711680);
   _root.Course.Edge.moveTo(-32,_root.LevelYsize * 32 + 64);
   _root.Course.Edge.lineTo(_root.LevelXsize * 32,_root.LevelYsize * 32 + 64);
   _root.Course.Edge.lineTo(_root.LevelXsize * 32,_root.LevelYsize * 32 + 500);
   _root.Course.Edge.lineTo(-32,_root.LevelYsize * 32 + 500);
   _root.Course.Edge.lineTo(-32,_root.LevelYsize * 32 - 48);
   _root.Course.Edge.endFill();
   _root.LDAttachItems();
   ydiff = _root.LevelYsize * 0.75 * 32;
   xdiff = _root.LevelXsize * 16;
   _root.BGCourse.BG._x += xdiff / 10;
   _root.BGCourse.BG._y += ydiff / 10;
   _root.MidBackground.BG._x += xdiff / 5;
   _root.MidBackground.BG._y += ydiff / 5;
   _root.BGCloseCourse.BG._x += xdiff / 2;
   _root.BGCloseCourse.BG._y += ydiff / 2;
   if(messageboolean == true)
   {
      _root.AreaTextClipF(_root.LDCourseName,12 + _root.LDCourseName.length);
   }
   _root.SongIntro = _root.LDSongIntroArray[_root.LDMusic];
   _root.SongRepeat = _root.LDSongRepeatArray[_root.LDMusic];
   _root.LDount = 0;
   warp = false;
   onEnterFrame = function()
   {
      _root.LDount = _root.LDount + 1;
      if(_root.LDount == 6)
      {
         _root.Course.Char.xspeed = Number(startXspeed);
         _root.Course.Char.yspeed = Number(startYspeed);
      }
      CharCode();
      if(_root.PauseGame == false)
      {
         if(_root.OrangeBlockPLCount > 0)
         {
            if(Math.round(_root.OrangeBlockPLCount) % (32 * Math.round(_root.OrangeBlockPLCount / 32)) == 0)
            {
               _root.Mariosound = new Sound(this);
               _root.Mariosound.attachSound("Tick");
               _root.Mariosound.start(0,1);
               _root.Mariosound.setVolume(_root.MarioVolume);
            }
            _root.OrangeBlockPLCount = _root.OrangeBlockPLCount - 1;
         }
      }
      if(Key.isDown(8))
      {
         _root.newstar = false;
         _root.RemoveCourse();
         _root.CreateLevelDesigner();
         _root.Mariosound.onSoundComplete = null;
      }
      if(LevelSplit == true && _root.LDount > 12 && warp == false)
      {
         if(_root.Course.Char._x - _root.Course.BackGFX._x > RightWarpX - 64 && startBlock < LevelSplitX.length - 1)
         {
            trace("Right warp Y=" + (_root.Course.Char._y - _root.Course.BackGFX._y));
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.attackFrame = "none";
            _root.attachMovie("righttransition","righttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
            warp = true;
            warpnum = 1;
         }
         else if(_root.Course.Char._x - _root.Course.BackGFX._x < LeftWarpX + 64 && startBlock > 1)
         {
            trace("Left warp");
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.attackFrame = "none";
            _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
            warp = true;
            warpnum = 2;
         }
      }
      if(_root.warpframe == true)
      {
         newY = _root.Course.Char._y - _root.Course.BackGFX._y;
         newX = _root.Course.Char._x - _root.Course.BackGFX._x + leftWidth * 32;
         if(warpnum == 1)
         {
            newX = Math.max(newX,LevelSplitX[startBlock] + 96);
            _root.RemoveCourse();
            _root.PlayLevelDesignerLevel(newX,newY,0,0,"righttransition2",false);
         }
         else
         {
            newX = Math.min(newX,LevelSplitX[startBlock - 1] - 96);
            _root.RemoveCourse();
            _root.PlayLevelDesignerLevel(newX,newY,0,0,"lefttransition2",false);
         }
         _root.warpframe = false;
      }
   };
};
_root.LDAttachItems = function()
{
   i = 0;
   while(i < _root.LevelItemNum)
   {
      if(_root.LDItemNumFlippable(i))
      {
         if(Boolean(Number(_root.LevelItems[i][3])) == true)
         {
            xscale = -100;
         }
         else
         {
            xscale = 100;
         }
      }
      if(_root.LevelItems[i][0] !== "1")
      {
         if(_root.LevelItems[i][0] == "2")
         {
            _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "3")
         {
            _root.Course.BackGFX.attachMovie("RedCoin","RedCoin" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "4")
         {
            _root.Course.BackGFX.attachMovie("BlueCoin","BlueCoin" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "5")
         {
            _root.Course.BackGFX.attachMovie("SilverStar","SilverStar" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "6")
         {
            _root.Course.BackGFX.attachMovie("ShineSpriteClip","ShineSpriteClip" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],LevelDesigner:true});
         }
         else if(_root.LevelItems[i][0] == "7")
         {
            if(Boolean(Number(_root.LevelItems[i][13])) == false)
            {
               _root.Course.BackGFX.attachMovie("MovingGreenPlatform","MovingGreenPlatform" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],DirectionX:String(_root.LevelItems[i][9]),DirectionY:String(_root.LevelItems[i][10]),distanceX:Number(_root.LevelItems[i][5]),distanceY:Number(_root.LevelItems[i][6]),accelX:Number(_root.LevelItems[i][11]),accelY:Number(_root.LevelItems[i][11]),speedX:Number(_root.LevelItems[i][3]),speedY:Number(_root.LevelItems[i][4]),Xoffstart:Number(_root.LevelItems[i][7]),Yoffstart:Number(_root.LevelItems[i][8]),scale:_root.LevelItems[i][12]});
            }
            else
            {
               _root.Course.BackGFX.attachMovie("TouchMovingGreenPlatform","MovingGreenPlatform" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],DirectionX:String(_root.LevelItems[i][9]),DirectionY:String(_root.LevelItems[i][10]),distanceX:Number(_root.LevelItems[i][5]),distanceY:Number(_root.LevelItems[i][6]),accelX:Number(_root.LevelItems[i][11]),accelY:Number(_root.LevelItems[i][11]),speedX:Number(_root.LevelItems[i][3]),speedY:Number(_root.LevelItems[i][4]),Xoffstart:Number(_root.LevelItems[i][7]),Yoffstart:Number(_root.LevelItems[i][8]),scale:_root.LevelItems[i][12],disapearcount:Number(_root.LevelItems[i][14])});
            }
         }
         else if(_root.LevelItems[i][0] == "8")
         {
            num = 0;
            while(num < Number(_root.LevelItems[i][4]))
            {
               _root.Course.BackGFX.attachMovie("GreenCirclePlatform","GreenCirclePlatform" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],speed:Number(_root.LevelItems[i][3]),distance:Number(_root.LevelItems[i][5]),angle:Number(_root.LevelItems[i][6]) + num * (360 / Number(_root.LevelItems[i][4])),scale:Number(_root.LevelItems[i][7])});
               num++;
            }
         }
         else if(_root.LevelItems[i][0] == "9")
         {
            _root.Course.BackGFX.attachMovie("RotatingBlockPlat","RotatingBlockPlat" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],DirectionX:String(_root.LevelItems[i][9]),DirectionY:String(_root.LevelItems[i][10]),distanceX:Number(_root.LevelItems[i][5]),distanceY:Number(_root.LevelItems[i][6]),accelX:Number(_root.LevelItems[i][11]),accelY:Number(_root.LevelItems[i][11]),speedX:Number(_root.LevelItems[i][3]),speedY:Number(_root.LevelItems[i][4]),Xoffstart:Number(_root.LevelItems[i][7]),Yoffstart:Number(_root.LevelItems[i][8]),scale:Number(_root.LevelItems[i][12]),blocktype:Number(_root.LevelItems[i][13]),rotdirection:String(_root.LevelItems[i][14]),rotspeed:Number(_root.LevelItems[i][15]),rotwait:Number(_root.LevelItems[i][16]),quake:Boolean(Number(_root.LevelItems[i][17]))});
         }
         else if(_root.LevelItems[i][0] == "11")
         {
            _root.Course.BackGFX.attachMovie("FallingLog","FallingLog" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "12")
         {
            _root.Course.BackGFX.attachMovie("TippingLog","TippingLog" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:1});
         }
         else if(_root.LevelItems[i][0] == "13")
         {
            _root.Course.BackGFX.attachMovie("TippingLog","TippingLog" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:2});
         }
         else if(_root.LevelItems[i][0] == "14")
         {
            _root.Course.BackGFX.attachMovie("TippingLog","TippingLog" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:3});
         }
         else if(_root.LevelItems[i][0] == "15")
         {
            _root.Course.BackGFX.attachMovie("TippingLog","TippingLog" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:4});
         }
         else if(_root.LevelItems[i][0] == "16")
         {
            _root.Course.FrontGFX.attachMovie("WaterPLclip","WaterPLclip" + i,_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "17")
         {
            _root.Course.FrontGFX.attachMovie("WaterPLclip","WaterPLclip" + i,_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],Scale:2});
         }
         else if(_root.LevelItems[i][0] == "18")
         {
            _root.Course.BackGFX.attachMovie("FlippingPL","FlippingPL" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],speed:Number(_root.LevelItems[i][3]),_rotation:Number(_root.LevelItems[i][4])});
         }
         else if(_root.LevelItems[i][0] == "19")
         {
            _root.Course.BackGFX.attachMovie("FlippingPL","FlippingPL" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],speed:Number(_root.LevelItems[i][3]),_rotation:Number(_root.LevelItems[i][4]),scale:2});
         }
         else if(_root.LevelItems[i][0] == "20")
         {
            _root.Course.BackGFX.attachMovie("PendulumCode","PendulumCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "21")
         {
            _root.Course.BackGFX.attachMovie("PendulumCode","PendulumCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],angle:-70});
         }
         else if(_root.LevelItems[i][0] == "22")
         {
            _root.Course.BackGFX.attachMovie("PendulumArrow","PendulumArrow" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "23")
         {
            _root.Course.BackGFX.attachMovie("PendulumArrow","PendulumArrow" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],speed:-0.8});
         }
         else if(_root.LevelItems[i][0] == "24")
         {
            _root.Course.BackGFX.attachMovie("RotatingTriangle","RotatingTriangle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "25")
         {
            _root.Course.BackGFX.attachMovie("RotatingTriangle","RotatingTriangle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],rotdirection:"Right"});
         }
         else if(_root.LevelItems[i][0] == "26")
         {
            _root.Course.BackGFX.attachMovie("RotatingTriangle","RotatingTriangle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],rotwait:0});
         }
         else if(_root.LevelItems[i][0] == "27")
         {
            _root.Course.BackGFX.attachMovie("RotatingTriangle","RotatingTriangle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],rotwait:0,rotdirection:"Right"});
         }
         else if(_root.LevelItems[i][0] == "37")
         {
            _root.Course.BackGFX.attachMovie("SlingStarBackGFX","SlingStarBackGFX" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],xblast:Number((- Math.cos((Number(_root.LevelItems[i][4]) + 90) * 3.141592653589793 / 180)) * Number(_root.LevelItems[i][3])),yblast:Number((- Math.sin((Number(_root.LevelItems[i][4]) + 90) * 3.141592653589793 / 180)) * Number(_root.LevelItems[i][3]) - Number(_root.LevelItems[i][3]) / 4),_rotation:Number(_root.LevelItems[i][4])});
         }
         else if(_root.LevelItems[i][0] == "38")
         {
            _root.Course.Water.attachMovie("LDWater","LDWater" + i,_root.Course.Water.getNextHighestDepth(),{_x:Number(_root.LevelItems[i][1]),_y:Number(_root.LevelItems[i][2]) - 16,_rotation:Number(_root.LevelItems[i][3]),WaterLength:Number(_root.LevelItems[i][4]),WaterDeepness:Number(_root.LevelItems[i][5])});
         }
         else if(_root.LevelItems[i][0] == "39")
         {
            trace("MainDoorClip");
            _root.Course.BackGFX.attachMovie("MainDoorClip","MainDoorClip" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:Number(_root.LevelItems[i][1]),_y:Number(_root.LevelItems[i][2]) - 16,walk:"warppipe",doorplace:"XY",placeX:Number(_root.LevelItems[i][3]),placeY:Number(_root.LevelItems[i][4]),doortype:_root.LevelItems[i][5]});
         }
         else if(_root.LevelItems[i][0] == "40")
         {
            trace("Warp Pipe Top");
            _root.Course.BackGFX.attachMovie("InvisibleDoor","InvisibleDoor" + i,_root.Course.BackGFX.getNextHighestDepth(),{_x:Number(_root.LevelItems[i][1]),_y:Number(_root.LevelItems[i][2]) - 16,walk:"warppipe",doorplace:"XY",placeX:Number(_root.LevelItems[i][3]),placeY:Number(_root.LevelItems[i][4])});
            _root.Course.FrontGFX.attachMovie("LDItem","Item" + i,- (_root.LevelItems.length + 1) + i);
            _root.Course.FrontGFX["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.FrontGFX["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.FrontGFX["Item" + i]._y = _root.LevelItems[i][2];
         }
         else if(_root.LevelItems[i][0] == "41")
         {
            _root.Course.FrontGFX.attachMovie("LDItem","Item" + i,- (_root.LevelItems.length + 1) + i);
            _root.Course.FrontGFX["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.FrontGFX["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.FrontGFX["Item" + i]._y = _root.LevelItems[i][2];
            if(Boolean(Number(_root.LevelItems[i][3])) == true)
            {
               _root.Course.FrontGFX["Item" + i]._xscale = 100;
            }
            else
            {
               _root.Course.FrontGFX["Item" + i]._xscale = -100;
            }
            _root.Course.FrontGFX["Item" + i]._rotation = _root.LevelItems[i][4];
         }
         else if(_root.LevelItems[i][0] == "42")
         {
            _root.Course.FrontGFX.attachMovie("LDItem","Item" + i,- (_root.LevelItems.length + 1) + i);
            _root.Course.FrontGFX["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.FrontGFX["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.FrontGFX["Item" + i]._y = _root.LevelItems[i][2];
            if(Boolean(Number(_root.LevelItems[i][3])) == true)
            {
               _root.Course.FrontGFX["Item" + i]._xscale = 100;
            }
            else
            {
               _root.Course.FrontGFX["Item" + i]._xscale = -100;
            }
            _root.Course.FrontGFX["Item" + i]._rotation = _root.LevelItems[i][4];
         }
         else if(_root.LevelItems[i][0] == "51")
         {
            _root.Course.BackGFX.attachMovie("GoombaCode","Goomba" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "52")
         {
            _root.Course.BackGFX.attachMovie("GoombaCode","Goomba" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:2,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "53")
         {
            _root.Course.BackGFX.attachMovie("GoombaCode","Goomba" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:0.5,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "54")
         {
            _root.Course.BackGFX.attachMovie("ShyGuyCode","Shyguy" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "55")
         {
            _root.Course.BackGFX.attachMovie("ShyGuyCode","Shyguy" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],State:"Flying",_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "56")
         {
            _root.Course.BackGFX.attachMovie("KoopaCode","Koopa" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],underwater:true,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "57")
         {
            _root.Course.BackGFX.attachMovie("KoopaCode","Koopa" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],underwater:true,State:"Flying",_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "58")
         {
            _root.Course.BackGFX.attachMovie("KoopaCode","Koopa" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],underwater:true,State:"Shell",_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "59")
         {
            _root.Course.BackGFX.attachMovie("BombombCode","Bobomb" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "60")
         {
            _root.Course.BackGFX.attachMovie("BullyCode","Bully" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "61")
         {
            _root.Course.BackGFX.attachMovie("BigBullyCode","BigBully" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],restart:true,scale:2,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "62")
         {
            _root.Course.BackGFX.attachMovie("CheepCheepCode","CheepCheep" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "63")
         {
            _root.Course.BackGFX.attachMovie("CheepCheepCode","CheepCheep" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:2,chasespeed:0.7,idlespeed:3,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "64")
         {
            _root.Course.FrontGFX.attachMovie("GhostCode","Ghost" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "65")
         {
            _root.Course.FrontGFX.attachMovie("GhostCode","Ghost" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],scale:2,_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "66")
         {
            _root.Course.BackGFX.attachMovie("PenguinCode","Penguin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "67")
         {
            _root.Course.BackGFX.attachMovie("PenguinCode","Penguin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],State:"Flying",WalkDistance:0,walkspeed:0,movedirection:"left",_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "68")
         {
            _root.Course.FrontGFX.attachMovie("GoonieCode","Goonie" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "69")
         {
            _root.Course.FrontGFX.attachMovie("SkeletonGoonieCode","SkeletonGoonie" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "70")
         {
            _root.Course.FrontGFX.attachMovie("SkeletonGoonieCode","SkeletonGoonie" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale,frame:"run",runspeed:3});
         }
         else if(_root.LevelItems[i][0] == "71")
         {
            if(_root.LevelItems[i][3] == "Left")
            {
               onlyleft = true;
               onlyright = false;
            }
            if(_root.LevelItems[i][3] == "Both")
            {
               onlyleft = true;
               onlyright = true;
            }
            if(_root.LevelItems[i][3] == "Right")
            {
               onlyleft = false;
               onlyright = true;
            }
            _root.Course.FrontGFX.attachMovie("BulletBillLauncher","SkeletonGoonie" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale,_rotation:_root.LevelItems[i][4],onlyleft:onlyleft,onlyright:onlyright,lockedside:true,speed:_root.LevelItems[i][5],waittime:_root.LevelItems[i][6],offset:_root.LevelItems[i][7],frame:_root.LevelItems[i][8],chase:Boolean(Number(_root.LevelItems[i][9])),eat:Boolean(Number(_root.LevelItems[i][9]))});
         }
         else if(_root.LevelItems[i][0] == "72")
         {
            _root.Course.BackGFX.attachMovie("ButterflyCode","ButterflyCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],_xscale:xscale});
         }
         else if(_root.LevelItems[i][0] == "73")
         {
            _root.Course.BackGFX.attachMovie("CodedSign","CodedSign" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],signmessage:unescape(_root.LevelItems[i][3])});
         }
         else if(_root.LevelItems[i][0] == "74")
         {
            switchdepth = _root.Course.BackGFX.getNextHighestDepth();
            _root.Course.BackGFX.attachMovie("SwitchClip","SwitchClip" + switchdepth,_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
            _root.Course.BackGFX["SwitchClip" + switchdepth].Tduration = Number(_root.LevelItems[i][3]);
            _root.Course.BackGFX["SwitchClip" + switchdepth].Switchfunction = function()
            {
               _root.Mariosound = new Sound(this);
               _root.Mariosound.attachSound("Tick");
               _root.Mariosound.start(0,1);
               _root.Mariosound.setVolume(_root.MarioVolume);
               _root.OrangeBlockPLCount = 32 * this.Tduration;
               this.onEnterFrame = function()
               {
                  if(_root.OrangeBlockPLCount <= 0)
                  {
                     this.play();
                  }
               };
            };
         }
         else if(_root.LevelItems[i][0] == "75")
         {
            _root.Course.BackGFX.attachMovie("SmallWaterBottle","SmallWaterBottle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "76")
         {
            _root.Course.BackGFX.attachMovie("WaterBottle","WaterBottle" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "77")
         {
            _root.Course.BackGFX.attachMovie("1UpCode","1UpCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "78")
         {
            _root.Course.BackGFX.attachMovie("CapWingCode","CapWingCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],Time:Number(_root.LevelItems[i][3])});
         }
         else if(_root.LevelItems[i][0] == "79")
         {
            _root.Course.BackGFX.attachMovie("CapInvisCode","CapInvisCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],Time:Number(_root.LevelItems[i][3])});
         }
         else if(_root.LevelItems[i][0] == "80")
         {
            _root.Course.BackGFX.attachMovie("CapMetalCode","CapMetalCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],Time:Number(_root.LevelItems[i][3])});
         }
         else if(_root.LevelItems[i][0] == "81")
         {
            _root.Course.BackGFX.attachMovie("CapStarmanCode","CapStarmanCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2],Time:Number(_root.LevelItems[i][3])});
         }
         else if(_root.LevelItems[i][0] == "82")
         {
            _root.Course.BackGFX.attachMovie("HoverFluddBox","HoverFluddBox" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "83")
         {
            _root.Course.BackGFX.attachMovie("RocketNozzleBox","RocketNozzleBox" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.LevelItems[i][0] == "84")
         {
            _root.Course.BackGFX.attachMovie("TurboNozzleBox","TurboNozzleBox" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.LevelItems[i][1],_y:_root.LevelItems[i][2]});
         }
         else if(_root.ItemFrontGFX(i) || _root.ItemFrontGFXBack(i))
         {
            if(_root.ItemFrontGFXBack(i))
            {
               _root.Course.FrontGFX.attachMovie("LDItem","Item" + i,- (_root.LevelItems.length + 1) + i);
            }
            else
            {
               _root.Course.FrontGFX.attachMovie("LDItem","Item" + i,_root.Course.FrontGFX.getNextHighestDepth());
            }
            _root.Course.FrontGFX["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.FrontGFX["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.FrontGFX["Item" + i]._y = _root.LevelItems[i][2];
            if(_root.LDItemNumRotatable(i) || _root.LDItemNumFlippable(i))
            {
               if(Boolean(Number(_root.LevelItems[i][3])) == true)
               {
                  _root.Course.FrontGFX["Item" + i]._xscale = 100;
               }
               else
               {
                  _root.Course.FrontGFX["Item" + i]._xscale = -100;
               }
               if(_root.LDItemNumRotatable(i))
               {
                  _root.Course.FrontGFX["Item" + i]._rotation = _root.LevelItems[i][4];
               }
            }
         }
         else
         {
            _root.Course.BackGFX.attachMovie("LDItem","Item" + i,_root.Course.BackGFX.getNextHighestDepth());
            _root.Course.BackGFX["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.BackGFX["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.BackGFX["Item" + i]._y = _root.LevelItems[i][2];
            if(_root.LDItemNumRotatable(i) || _root.LDItemNumFlippable(i))
            {
               if(Boolean(Number(_root.LevelItems[i][3])) == true)
               {
                  _root.Course.BackGFX["Item" + i]._xscale = 100;
               }
               else
               {
                  _root.Course.BackGFX["Item" + i]._xscale = -100;
               }
               if(_root.LDItemNumRotatable(i))
               {
                  _root.Course.BackGFX["Item" + i]._rotation = _root.LevelItems[i][4];
               }
            }
         }
         if(_root.ItemPlatform(i))
         {
            trace(_root.LevelItems[i][0]);
            _root.Course.Platforms.attachMovie("LDItemPlatforms","Item" + i,_root.Course.Platforms.getNextHighestDepth());
            _root.Course.Platforms["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.Platforms["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.Platforms["Item" + i]._y = _root.LevelItems[i][2];
            if(_root.LDItemNumRotatable(i) || _root.LDItemNumFlippable(i))
            {
               if(Boolean(Number(_root.LevelItems[i][3])) == true)
               {
                  _root.Course.Platforms["Item" + i]._xscale = 100;
               }
               else
               {
                  _root.Course.Platforms["Item" + i]._xscale = -100;
               }
               if(_root.LDItemNumRotatable(i))
               {
                  _root.Course.Platforms["Item" + i]._rotation = _root.LevelItems[i][4];
               }
            }
         }
         if(_root.ItemBPlatform(i))
         {
            _root.Course.BPlatforms.attachMovie("LDItemBPlatforms","Item" + i,_root.Course.BPlatforms.getNextHighestDepth());
            _root.Course.BPlatforms["Item" + i].gotoAndStop(_root.LevelItems[i][0]);
            _root.Course.BPlatforms["Item" + i]._x = _root.LevelItems[i][1];
            _root.Course.BPlatforms["Item" + i]._y = _root.LevelItems[i][2];
            if(_root.LDItemNumRotatable(i) || _root.LDItemNumFlippable(i))
            {
               if(Boolean(Number(_root.LevelItems[i][3])) == true)
               {
                  _root.Course.BPlatforms["Item" + i]._xscale = 100;
               }
               else
               {
                  _root.Course.BPlatforms["Item" + i]._xscale = -100;
               }
               if(_root.LDItemNumRotatable(i))
               {
                  _root.Course.BPlatforms["Item" + i]._rotation = _root.LevelItems[i][4];
               }
            }
         }
      }
      i++;
   }
};
_root.LDAttachTile = function(X, Y, casevar)
{
   attachGFX = "Front";
   if(casevar == 100)
   {
      attachGFX = "none";
   }
   else if(casevar == 219 || casevar == 606 || casevar == 607 || casevar == 608 || casevar == 609 || casevar == 610 || casevar == 611 || casevar == 612 || casevar == 627 || casevar == 628 || casevar == 634 || casevar == 635 || casevar == 639 || casevar == 640 || casevar == 641 || casevar == 642 || casevar == 643 || casevar == 644 || casevar == 661 || casevar == 662 || casevar == 663 || casevar == 664 || casevar == 665 || casevar == 666 || casevar == 667 || casevar == 668 || casevar == 669 || casevar == 670 || casevar == 671 || casevar == 687 || casevar == 688 || casevar == 689 || casevar == 700 || casevar == 730 || casevar == 731 || casevar == 735 || casevar == 738)
   {
      attachGFX = "Back";
   }
   else if(casevar == 101 || casevar >= 103 && casevar < 107 || casevar == 117 || casevar == 118 || casevar == 121 || casevar == 122 || casevar == 129 || casevar == 130 || casevar == 133 || casevar == 134 || casevar == 135 || casevar == 136 || casevar == 201 || casevar == 210 || casevar == 228 || casevar == 237 || casevar == 246 || casevar == 251 || casevar == 260 || casevar == 265 || casevar == 270 || casevar == 275 || casevar == 280 || casevar >= 285 && casevar <= 296 && casevar !== 293 || casevar == 301 || casevar >= 303 && casevar < 307 || casevar == 317 || casevar == 318 || casevar == 321 || casevar == 322 || casevar == 329 || casevar == 330 || casevar == 333 || casevar == 334 || casevar == 351 || casevar == 353 || casevar == 354 || casevar == 397 || casevar == 401 || casevar == 403 || casevar == 404 || casevar == 405 || casevar == 406 || casevar == 417 || casevar == 418 || casevar == 421 || casevar == 422 || casevar == 433 || casevar == 434 || casevar == 443 || casevar == 444 || casevar == 453 || casevar == 454 || casevar == 459 || casevar == 460 || casevar == 470 || casevar == 473 || casevar == 476 || casevar == 479 || casevar == 485 || casevar == 486 || casevar == 491 || casevar == 495 || casevar == 501 || casevar == 514 || casevar == 527 || casevar == 538 || casevar == 539 || casevar == 551 || casevar == 564 || casevar == 571 || casevar == 576 || casevar == 581 || casevar == 586 || casevar == 593 || casevar == 594 || casevar == 437 || casevar == 438 || casevar == 443 || casevar == 444 || casevar == 601 || casevar == 622 || casevar == 629 || casevar == 636 || casevar == 637 || casevar == 638 || casevar == 645 || casevar == 650 || casevar == 651 || casevar == 656 || casevar == 672 || casevar == 677 || casevar == 682 || casevar == 690 || casevar == 695 || casevar == 705 || casevar == 706 || casevar == 715 || casevar == 720 || casevar == 725 || casevar == 732 || casevar == 733 || casevar == 734 || casevar == 736 || casevar == 737 || casevar == 429 || casevar == 430 || casevar == 482 || casevar == 739 || casevar == 740)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 102 || casevar == 202 || casevar == 211 || casevar == 220 || casevar == 229 || casevar == 238 || casevar == 247 || casevar == 252 || casevar == 261 || casevar == 266 || casevar == 271 || casevar == 276 || casevar == 281 || casevar == 302 || casevar == 352 || casevar == 402 || casevar == 472 || casevar == 475 || casevar == 478 || casevar == 481 || casevar == 484 || casevar == 487 || casevar == 491 || casevar == 502 || casevar == 515 || casevar == 528 || casevar == 540 || casevar == 552 || casevar == 565 || casevar == 572 || casevar == 577 || casevar == 582 || casevar == 587 || casevar == 595 || casevar == 602 || casevar == 623 || casevar == 630 || casevar == 646 || casevar == 652 || casevar == 657 || casevar == 673 || casevar == 678 || casevar == 683 || casevar == 691 || casevar == 696 || casevar == 701 || casevar == 707 || casevar == 716 || casevar == 721 || casevar == 726 || casevar == 741)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 137 || casevar == 203 || casevar == 212 || casevar == 221 || casevar == 230 || casevar == 239 || casevar == 248 || casevar == 253 || casevar == 262 || casevar == 267 || casevar == 272 || casevar == 277 || casevar == 282 || casevar == 337 || casevar == 375 || casevar == 461 || casevar == 471 || casevar == 474 || casevar == 477 || casevar == 480 || casevar == 483 || casevar == 488 || casevar == 492 || casevar == 503 || casevar == 516 || casevar == 529 || casevar == 541 || casevar == 553 || casevar == 566 || casevar == 573 || casevar == 578 || casevar == 583 || casevar == 588 || casevar == 596 || casevar == 603 || casevar == 624 || casevar == 631 || casevar == 647 || casevar == 653 || casevar == 658 || casevar == 674 || casevar == 679 || casevar == 684 || casevar == 692 || casevar == 697 || casevar == 702 || casevar == 708 || casevar == 717 || casevar == 722 || casevar == 727 || casevar == 742)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 107 || casevar == 109 || casevar == 115 || casevar == 307 || casevar == 309 || casevar == 315 || casevar == 335 || casevar == 370 || casevar == 407 || casevar == 409 || casevar == 413 || casevar == 415 || casevar == 547)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32 + 16,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32 + 16,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32 + 16,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 108 || casevar == 110 || casevar == 116 || casevar == 308 || casevar == 310 || casevar == 316 || casevar == 336 || casevar == 369 || casevar == 408 || casevar == 410 || casevar == 414 || casevar == 416 || casevar == 546)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32 - 16,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32 - 16,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 204 || casevar == 213 || casevar == 222 || casevar == 231 || casevar == 240 || casevar == 249 || casevar == 254 || casevar == 263 || casevar == 268 || casevar == 273 || casevar == 278 || casevar == 283 || casevar == 489 || casevar == 494 || casevar == 504 || casevar == 517 || casevar == 530 || casevar == 554 || casevar == 567 || casevar == 570 || casevar == 574 || casevar == 579 || casevar == 585 || casevar == 597 || casevar == 436 || casevar == 604 || casevar == 625 || casevar == 632 || casevar == 648 || casevar == 654 || casevar == 659 || casevar == 675 || casevar == 680 || casevar == 685 || casevar == 693 || casevar == 698 || casevar == 703 || casevar == 709 || casevar == 718 || casevar == 723 || casevar == 728 || casevar == 436 || casevar == 743)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 205 || casevar == 214 || casevar == 223 || casevar == 232 || casevar == 241 || casevar == 250 || casevar == 255 || casevar == 264 || casevar == 274 || casevar == 269 || casevar == 274 || casevar == 279 || casevar == 284 || casevar == 490 || casevar == 493 || casevar == 505 || casevar == 518 || casevar == 531 || casevar == 555 || casevar == 568 || casevar == 569 || casevar == 575 || casevar == 580 || casevar == 584 || casevar == 598 || casevar == 435 || casevar == 605 || casevar == 626 || casevar == 633 || casevar == 649 || casevar == 655 || casevar == 660 || casevar == 676 || casevar == 681 || casevar == 686 || casevar == 694 || casevar == 699 || casevar == 704 || casevar == 710 || casevar == 719 || casevar == 724 || casevar == 729 || casevar == 435 || casevar == 744)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 127 || casevar == 131 || casevar == 206 || casevar == 215 || casevar == 224 || casevar == 233 || casevar == 242 || casevar == 256 || casevar == 327 || casevar == 331 || casevar == 358 || casevar == 366 || casevar == 427 || casevar == 431 || casevar == 506 || casevar == 519 || casevar == 532 || casevar == 556 || casevar == 427 || casevar == 431 || casevar == 711)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 128 || casevar == 132 || casevar == 207 || casevar == 216 || casevar == 225 || casevar == 234 || casevar == 243 || casevar == 257 || casevar == 328 || casevar == 332 || casevar == 357 || casevar == 365 || casevar == 428 || casevar == 432 || casevar == 507 || casevar == 520 || casevar == 533 || casevar == 557 || casevar == 428 || casevar == 432 || casevar == 712)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 123 || casevar == 125 || casevar == 208 || casevar == 217 || casevar == 226 || casevar == 235 || casevar == 244 || casevar == 253 || casevar == 258 || casevar == 323 || casevar == 325 || casevar == 356 || casevar == 364 || casevar == 423 || casevar == 425 || casevar == 508 || casevar == 521 || casevar == 534 || casevar == 558 || casevar == 423 || casevar == 425 || casevar == 713)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 124 || casevar == 126 || casevar == 209 || casevar == 218 || casevar == 227 || casevar == 236 || casevar == 245 || casevar == 259 || casevar == 260 || casevar == 324 || casevar == 326 || casevar == 355 || casevar == 363 || casevar == 424 || casevar == 426 || casevar == 509 || casevar == 522 || casevar == 535 || casevar == 559 || casevar == 424 || casevar == 426 || casevar == 714)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.moveTo(X * 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 111 || casevar == 113 || casevar == 311 || casevar == 313 || casevar == 544 || casevar == 591)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32 + 16,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32 + 16,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32 + 16,Y * 32 - 32 + 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 112 || casevar == 114 || casevar == 312 || casevar == 314 || casevar == 545 || casevar == 592)
   {
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32 - 16,Y * 32 - 32 + 16);
      _root.Course.Platforms.lineTo(X * 32 - 16,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32 + 16);
      _root.Course.Platforms.endFill();
      attachGFX = "Front";
   }
   else if(casevar == 613 || casevar == 614 || casevar == 615 || casevar == 616 || casevar == 617 || casevar == 618 || casevar == 619 || casevar == 620 || casevar == 621 || casevar == 689)
   {
      _root.Course.BPlatforms.attachMovie("LDTilePlatforms","Tile" + X + "_" + Y,_root.Course.BPlatforms.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
      _root.Course.BPlatforms["Tile" + X + "_" + Y].gotoAndStop(_root.LevelTiles[X][Y]);
      attachGFX = "Back";
   }
   else if(casevar == 293)
   {
      _root.Course.Platforms.invisground.beginFill(65280);
      _root.Course.Platforms.invisground.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.invisground.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.invisground.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.invisground.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.invisground.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.invisground.endFill();
      trace("Attached invisground");
      attachGFX = "Front";
   }
   else if(casevar >= 150 && casevar <= 180)
   {
      if(casevar < 156)
      {
         _type = 1;
      }
      else if(casevar < 161)
      {
         _type = 2;
      }
      else if(casevar < 166)
      {
         _type = 3;
      }
      else if(casevar < 171)
      {
         _type = 4;
      }
      else if(casevar < 176)
      {
         _type = 5;
      }
      else if(casevar < 181)
      {
         _type = 6;
      }
      if((casevar - 1) % 5 == 0)
      {
         _coinnum = "none";
      }
      else if((casevar - 1) % 5 == 1)
      {
         _coinnum = "3coins";
      }
      else if((casevar - 1) % 5 == 2)
      {
         _coinnum = "5coins";
      }
      else if((casevar - 1) % 5 == 3)
      {
         _coinnum = "10coins";
      }
      else if((casevar - 1) % 5 == 4)
      {
         _coinnum = "30coins";
      }
      _root.Course.BackGFX.attachMovie("BreakableBox","Tile" + X + "_" + Y,_root.Course.BackGFX.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32,blocknum:_type,stuffinside:_coinnum});
      attachGFX = "none";
   }
   else if(casevar == 181)
   {
      attachGFX = "Front";
      _root.Course.BackGFX.attachMovie("CannonClip","Cannon" + X + "_" + Y,_root.Course.BackGFX.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16,cannonsetup:true,side:"Both"});
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.endFill();
   }
   else if(casevar == 182)
   {
      attachGFX = "Back";
      _root.Course.Platforms.attachMovie("LDTilePlatforms","Tile" + X + "_" + Y,_root.Course.Platforms.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
      _root.Course.Platforms["Tile" + X + "_" + Y].gotoAndStop(_root.LevelTiles[X][Y]);
   }
   else
   {
      _root.Course.Platforms.attachMovie("LDTilePlatforms","Tile" + X + "_" + Y,_root.Course.Platforms.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
      _root.Course.Platforms["Tile" + X + "_" + Y].gotoAndStop(_root.LevelTiles[X][Y]);
      attachGFX = "Back";
   }
   if(casevar >= 219 && casevar <= 227 || casevar == 629 || casevar == 630 || casevar == 631 || casevar == 632 || casevar == 633 || casevar >= 678 && casevar <= 689 || casevar >= 696 && casevar <= 699)
   {
      attachGFX = "Back";
   }
   if(casevar >= 101 && casevar <= 149 || casevar >= 300 && casevar <= 485 || casevar >= 500 && casevar <= 563 || casevar == 738)
   {
      if(attachGFX !== "none")
      {
         attachGFX = "Front";
      }
   }
   if(casevar >= 470 && casevar <= 485 || casevar >= 435 && casevar <= 442 || casevar >= 156 && casevar <= 160)
   {
      _root.Course.Ice.beginFill(65280);
      _root.Course.Ice.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Ice.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Ice.lineTo(X * 32,Y * 32);
      _root.Course.Ice.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Ice.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Ice.endFill();
   }
   if(casevar == 536)
   {
      attachGFX = "Front";
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 16);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 16);
      _root.Course.Platforms.endFill();
      _root.Course.Lava.beginFill(65280);
      _root.Course.Lava.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Lava.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Lava.lineTo(X * 32,Y * 32);
      _root.Course.Lava.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Lava.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Lava.endFill();
   }
   if(casevar == 537)
   {
      attachGFX = "Front";
      _root.Course.Platforms.beginFill(65280);
      _root.Course.Platforms.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Platforms.lineTo(X * 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Platforms.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Platforms.endFill();
      _root.Course.Lava.beginFill(65280);
      _root.Course.Lava.moveTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Lava.lineTo(X * 32,Y * 32 - 32);
      _root.Course.Lava.lineTo(X * 32,Y * 32);
      _root.Course.Lava.lineTo(X * 32 - 32,Y * 32);
      _root.Course.Lava.lineTo(X * 32 - 32,Y * 32 - 32);
      _root.Course.Lava.endFill();
   }
   if(attachGFX == "Front")
   {
      _root.Course.FrontGFX.attachMovie("LDTile","Tile" + X + "_" + Y,_root.Course.FrontGFX.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
      _root.Course.FrontGFX["Tile" + X + "_" + Y].gotoAndStop(_root.LevelTiles[X][Y]);
   }
   else if(attachGFX == "Back")
   {
      _root.Course.BackGFX.attachMovie("LDTile","Tile" + X + "_" + Y,_root.Course.BackGFX.getNextHighestDepth(),{_x:X * 32 - 16,_y:Y * 32 - 16});
      _root.Course.BackGFX["Tile" + X + "_" + Y].gotoAndStop(_root.LevelTiles[X][Y]);
   }
};
