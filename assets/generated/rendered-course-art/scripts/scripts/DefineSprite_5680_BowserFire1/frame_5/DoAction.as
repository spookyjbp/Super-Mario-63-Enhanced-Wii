count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,90,-13,3,7,"");
         }
         else
         {
            _root.MarioHit(1,90,-13,-3,7,"");
         }
      }
      count++;
      if(count > 20)
      {
         this.removeMovieClip(this);
      }
   }
};
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("Explosion");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
