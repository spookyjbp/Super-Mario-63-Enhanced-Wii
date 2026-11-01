Mode = "Off";
if(_root.CheatStarToggle == true)
{
   Mode = "On";
}
else
{
   Mode = "Off";
}
al = 50;
sc = 75;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      al = 100;
      sc = 100;
   }
   else
   {
      al = 50;
      sc = 75;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _alpha = _alpha + (al - _alpha) / 2;
};
onMouseDown = function()
{
   if(sc == 100)
   {
      if(ON.sc == 100 && OFF.sc == 100)
      {
         _xscale = 110;
         _yscale = 110;
         if(Mode == "On")
         {
            Mode = "Off";
            _root.CheatStarToggle = false;
         }
         else
         {
            Mode = "On";
            _root.CheatStarToggle = true;
         }
      }
   }
};
