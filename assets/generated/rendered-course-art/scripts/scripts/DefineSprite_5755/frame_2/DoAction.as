stop();
count = 0;
bomb.Explode();
soundclip = new Sound(this);
soundclip.attachSound("Explosion");
soundclip.start(0,1);
soundclip.setVolume(_root.MarioVolume);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(scale > 0)
         {
            _root.MarioHit(2,90,-9,9,7,"Mario-Squish",true);
         }
         else
         {
            _root.MarioHit(2,90,-9,-9,7,"Mario-Squish",true);
         }
      }
      count++;
      if(count > 30)
      {
         gotoAndStop(1);
      }
   }
};
