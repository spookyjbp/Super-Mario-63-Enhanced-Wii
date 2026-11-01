_root.Course.Char.charClip.frame.gotoAndPlay("fling");
if(trackclip !== undefined)
{
   _root.Course.BackGFX[trackclip].play();
}
_root.Course.Char.yspeed = yblast;
_root.Course.Char.xspeed = xblast;
count = 20;
onEnterFrame = function()
{
   _root.CamYchange = yblast * 2 / Math.sqrt(Math.abs(21 - count));
   _root.CamXchange = xblast * 2 / Math.sqrt(Math.abs(21 - count));
   if(_root.PauseGame == false)
   {
      count *= 0.8;
      star.base._rotation += count;
      star2.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
      play();
   }
   else
   {
      stop();
   }
};
