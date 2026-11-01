_Y = _root._ymouse;
_X = _root._xmouse;
newY = _root._ymouse;
newX = _root._xmouse;
oldX = newX;
oldY = newY;
moveY = 0;
moveX = 0;
mousepress = false;
_root.MouseDown = false;
if(_root.WiiMode == false)
{
   _alpha = 0;
}
onEnterFrame = function()
{
   if(_root.WiiMode == true)
   {
      newY = _root._ymouse;
      newX = _root._xmouse;
      moveY = newY - oldY;
      moveX = newX - oldX;
      oldX = newX;
      oldY = newY;
      repeats = Math.ceil((Math.abs(moveY) + Math.abs(moveX)) / 50);
      i = 0;
      while(i < repeats)
      {
         _X = _X + moveX / repeats;
         _Y = _Y + moveY / repeats;
         _root.attachMovie("trail","trail" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_X,_y:_Y,_rotation:Math.random() * 12 - Math.random() * 12 + _root.radiansToAngle(Math.atan2(- moveX,moveY))});
         i++;
      }
      _Y = _root._ymouse;
      _X = _root._xmouse;
      _root.mousemoveY = moveY;
      _root.mousemoveX = moveX;
      _alpha = 100;
   }
   else
   {
      _X = 275;
      _Y = 200;
      _alpha = 0;
   }
};
onMouseDown = function()
{
   _root.MouseDown = true;
};
onMouseUp = function()
{
   _root.MouseDown = false;
};
