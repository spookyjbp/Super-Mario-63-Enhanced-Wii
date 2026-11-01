stop();
if(_root.CreditsQuiet !== true)
{
   started = false;
   onEnterFrame = function()
   {
      distancevolume = Math.sqrt(Math.pow(_root.currentCamX - (_X + _parent._x),2) + Math.pow(_root.currentCamY - (_Y + _parent._y),2));
      distancevolume *= range;
      distancevolume = Math.min(100,distancevolume);
      distancevolume = Math.max(100 - maxvolume,distancevolume);
      distancevolume -= 100;
      distancevolume *= -1;
      if(distancevolume >= 1)
      {
         if(started == false)
         {
            soundclip = new Sound(this);
            soundclip.attachSound(sound);
            soundclip.start(0,1);
            soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
            soundclip.setPan(panX);
            soundclip.onSoundComplete = function()
            {
               soundclip.attachSound(sound);
               soundclip.start(0,1);
               soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
               soundclip.setPan(panX);
            };
         }
         started = true;
         soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
         panX = _root.currentCamX - (_X + _parent._x);
         panX /= -2;
         panX = Math.min(100,panX);
         panX = Math.max(-100,panX);
         soundclip.setPan(panX);
      }
      else
      {
         panX = 0;
         if(started == true)
         {
            soundclip.stop();
            soundclip.onSoundComplete = null;
         }
         started = false;
      }
   };
}
