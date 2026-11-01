Mousedown = false;
onEnterFrame = function()
{
   if(_root.Star[_root.Playinglevelnum * 3 + 51 + starnum] == true)
   {
      gotstar = true;
   }
   else
   {
      gotstar = false;
   }
   if(starnum == 1 || _root.Star[_root.Playinglevelnum * 3 + 51 + starnum] == true || _root.Star[_root.Playinglevelnum * 3 + 51 + (starnum - 1)] == true)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,true))
      {
         _root.Selected = starnum;
      }
      if(_root.Selected == starnum)
      {
         _root.starname = _root.StarNameArray[_root.Playinglevelnum * 3 + 51 + starnum];
         _root.stardescription = _root.StarDescArray[_root.Playinglevelnum * 3 + 51 + starnum];
         nextFrame();
      }
      else
      {
         prevFrame();
      }
   }
   else
   {
      gotoAndStop(1);
   }
   if(Mousedown == true || Key.isDown(32) || _root.KeyZ() || _root.KeyX())
   {
      if(starnum == 1 || _root.Star[_root.Playinglevelnum * 3 + 51 + starnum] == true || _root.Star[_root.Playinglevelnum * 3 + 51 + (starnum - 1)] == true)
      {
         if(_root.Selected == starnum && _root.StartCourse == false)
         {
            _root.Starnum = starnum;
            _root.StartCourse = true;
         }
      }
   }
   Mousedown = false;
};
onMouseDown = function()
{
   if(_root.StartCourse !== true && this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      Mousedown = true;
   }
};
