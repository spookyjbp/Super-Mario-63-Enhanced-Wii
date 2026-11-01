_name = "1";
stopnum = _parent.stopnum;
onEnterFrame = function()
{
   if(_parent._currentframe == _parent.stopnum)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent.mousemove == true)
      {
         _parent.hl = _name;
      }
      else if(_parent.hl == _name && _parent.mousemove == true)
      {
         _parent.hl = undefined;
      }
      if(cl == true)
      {
         rotspeed -= 1;
         scaly = Math.max(-50,scaly - 1.2);
         _xscale = Math.max(20,_xscale + scaly);
         _alpha = _alpha - 3;
         _rotation = _rotation + rotspeed;
      }
      if(_parent.hl == _name && cl !== true)
      {
         _xscale = _xscale + (125 - _xscale) / 5;
         txt._xscale += (115 - txt._xscale) / 8;
         gotoAndStop(2);
      }
      else if(cl !== true)
      {
         _xscale = _xscale + (100 - _xscale) / 5;
         txt._xscale += (100 - txt._xscale) / 8;
         gotoAndStop(1);
      }
      _yscale = _xscale;
      txt._yscale = txt._xscale;
      if(_root.KeyZ() || _root.KeyX() || _root.KeySPACE())
      {
         if(cl !== true && _parent.hl == _name)
         {
            _parent.SAVE();
            rotspeed = 5;
            cl = true;
            scaly = 14;
         }
      }
   }
};
onMouseDown = function()
{
   if(_parent._currentframe == _parent.stopnum && _parent.clicked == false)
   {
      if(cl !== true && this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         _parent.clicked = true;
         _parent.SAVE();
         rotspeed = 5;
         cl = true;
         scaly = 14;
      }
   }
};
stop();
