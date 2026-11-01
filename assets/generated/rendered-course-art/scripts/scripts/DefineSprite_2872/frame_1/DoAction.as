if(_root.Star[39] == true)
{
   _visible = false;
}
else
{
   onEnterFrame = function()
   {
      if(_root.Star[39] == true)
      {
         _alpha = _alpha - 5;
         if(_alpha <= 0)
         {
            _visible = false;
            onEnterFrame = null;
         }
      }
   };
}
