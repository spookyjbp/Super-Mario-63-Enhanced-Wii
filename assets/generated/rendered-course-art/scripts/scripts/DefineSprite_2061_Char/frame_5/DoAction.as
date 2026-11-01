if(started == undefined)
{
   started = false;
}
stop();
onEnterFrame = function()
{
   if(_root.KeySPACE() && _root.PauseGame == false && _root.playpause == false && _root.Power > 0 && _root.WaterAmount > 0 && (_root.Fluddpow == "H" || _root.Fluddpow == "T") && (_root.Course.Char.attack == false || _root.Course.Char.attack == true && (_root.Course.Char.attackFrame == "Dive" || _root.Course.Char.attackFrame == "frontflip")))
   {
      if(started == false)
      {
         watersoundclip = new Sound(this);
         watersoundclip.attachSound("fluddrepeat");
         watersoundclip.start(0,1);
         watersoundclip.setVolume(_root.MarioVolume / 1.5);
         watersoundclip.onSoundComplete = function()
         {
            watersoundclip.attachSound("fluddrepeat");
            watersoundclip.start(0,1);
            watersoundclip.setVolume(_root.MarioVolume / 1.5);
            watersoundclip.setPan(panX);
         };
      }
      started = true;
   }
   else
   {
      if(started == true)
      {
         watersoundclip.stop();
      }
      started = false;
      watersoundclip.onSoundComplete = null;
   }
};
