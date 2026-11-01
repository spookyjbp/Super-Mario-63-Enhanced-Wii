_visible = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      _X = _X - _xscale / 100 * 5.3125;
      if(this.body.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == false)
      {
         if(_xscale < 0)
         {
            _root.MarioHit(1,90,-4,1,10,"Mario-Squish",true);
         }
         else
         {
            _root.MarioHit(1,90,-4,-1,10,"Mario-Squish",true);
         }
      }
   }
   else
   {
      stop();
   }
};
