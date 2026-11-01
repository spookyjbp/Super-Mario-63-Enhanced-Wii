stop();
onEnterFrame = function()
{
   txt = _root.ItemNameArray[_parent.moved.Selected];
   if(_parent.moved.Selected !== undefined)
   {
      gotoAndStop(_parent.moved.Selected + 1);
   }
   else
   {
      gotoAndStop(1);
   }
};
