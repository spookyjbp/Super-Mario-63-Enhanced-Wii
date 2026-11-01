bnum = random(30) + 30;
bcount = 0;
btype = random(6) + 1;
if(_root.CreditsQuiet !== true)
{
   onEnterFrame = function()
   {
      bcount++;
      if(bcount > bnum)
      {
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("Birdy" + btype);
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(Math.round(_root.MarioVolume / 2));
         bcount = 0;
         btype = random(6) + 1;
         bnum = random(50) + 15;
      }
   };
}
