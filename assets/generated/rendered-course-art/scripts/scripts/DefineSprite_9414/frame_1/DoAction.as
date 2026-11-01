ywave = random(90);
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
onEnterFrame = function()
{
   if(_currentframe > 5)
   {
      gotoAndStop(_currentframe - 4);
   }
   if(_parent.Mode == "copying")
   {
      if(_parent.HL == file)
      {
         normscale = 50;
         largescale = 75;
         if(_currentframe < 6)
         {
            gotoAndStop(_currentframe + 4);
         }
      }
      else if(_parent.HL == undefined)
      {
         normscale = 100;
         largescale = 125;
      }
      else
      {
         normscale = 105;
         largescale = 135;
      }
   }
   else if(_parent.HL == file)
   {
      normscale = 135;
      largescale = 145;
   }
   else if(_parent.HL == undefined)
   {
      normscale = 100;
      largescale = 125;
   }
   else
   {
      normscale = 75;
      largescale = 90;
   }
   if(bg.hitTest(_root._xmouse,_root._ymouse,true) && _parent.Mode !== "deleting")
   {
      hl = true;
   }
   else
   {
      hl = false;
   }
   if(hl == true && cl !== true)
   {
      this._xscale += (largescale - this._xscale) / 4;
      r += (largescale - this._xscale) / 15;
      txt._xscale += (100 - txt._xscale) / 4;
      ywave -= 0.03;
   }
   else if(cl !== true)
   {
      this._xscale += (normscale - this._xscale) / 4;
      r += (normscale - this._xscale) / 15;
      txt._xscale += (80 - txt._xscale) / 4;
   }
   this._yscale = this._xscale;
   txt._yscale = txt._xscale;
   ywave += 0.03;
   rwave += 0.025;
   this._y = Y + Math.sin(ywave) * 5.5;
   this._rotation = R + r + Math.sin(rwave) * _xscale / 20;
};
onMouseDown = function()
{
   if(hl == true && cl !== true && (_parent._currentframe < 2 || _parent._currentframe > 22) && _root.m.MenuRedTrans._x == undefined)
   {
      if(_parent.Mode !== "copying")
      {
         _parent.HL = file;
      }
      else if(_parent.Mode == "copying")
      {
         if(_root["File" + file + "TotalStars"] == 0 && file !== _parent.HL)
         {
            _parent.Mode = "";
            depth = _root.m.getNextHighestDepth();
            _root.m.attachMovie("MenuGreenTrans","MenuGreenTrans" + depth,depth,{trans:true,_x:0,_y:0,file1:file,file2:_parent.HL});
            _parent.HL = undefined;
         }
         else if(file !== _parent.HL)
         {
            _parent._parent.Message("Can Only Copy into an Empty File..","red");
         }
      }
   }
};
Delete = function()
{
   _parent.HL = undefined;
   em.gotoAndPlay(2);
};
gotoAndStop(frame + 1);
