stop();
onEnterFrame = function()
{
   if(this.hitTest(_root.Course.Char.charClip.head) && _root.Course.Char.yspeed < 0)
   {
      _root.Course.Char.yspeed = 3;
      play();
   }
};
