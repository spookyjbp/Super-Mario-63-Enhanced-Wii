if(_root.LevelDesignerMenu == true)
{
   _alpha = 50;
}
else
{
   _visible = false;
   onEnterFrame = function()
   {
      if(_root.OrangeBlockPLCount > 0)
      {
         if(_root.OrangeBlockPLCount > 1920 || Math.round(_root.OrangeBlockPLCount / Math.round(_root.OrangeBlockPLCount / 16)) % 2 == 0)
         {
            _visible = true;
         }
         else
         {
            _visible = false;
         }
      }
   };
}
