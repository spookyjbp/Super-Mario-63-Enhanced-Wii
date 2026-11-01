_root.PlayingLevelDesigner = false;
_root.bgsong.stop();
gotoAndStop("Courseframe");
if(_root.TotalStars == 0)
{
   _root.CurrentPlayer = "Mario";
   _root.CheatCastleRestore = true;
   gotoAndStop("Intro");
}
else
{
   _root.CheatCastleRestore = false;
   if(_root.Star[41] == false && _root.BowserKey1 == true)
   {
      StartChar("8-E3-1",0,0,0,0,100,_root.CurrentPlayer,"","StarIn",true);
      onEnterFrame = function()
      {
         CharCode();
      };
   }
   else
   {
      _root.CharXscale = -100;
      StartChar("Castle",-1100,100,-1100,100,100,_root.CurrentPlayer,"","StarIn");
      onEnterFrame = function()
      {
         CharCode();
      };
   }
}
