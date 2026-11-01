stop();
onEnterFrame = function()
{
   txt = _root["TileNameArray" + _parent.moved.Tab][_parent.moved.Selected];
   if(_parent.moved.Selected !== undefined)
   {
      gotoAndStop(_parent.moved.Selected + _parent.moved.Tab * 50 + 1);
   }
   else
   {
      gotoAndStop(1);
   }
};
