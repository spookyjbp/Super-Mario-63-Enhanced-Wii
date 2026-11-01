played = false;
onEnterFrame = function()
{
   AL();
   if(_root.Course.BackGFX.inroom == true && _alpha > 0)
   {
      if(played == false)
      {
         _root.StartNewMusicAndIntroNoVar("BowserPlan-Intro","BowserPlan-Repeat");
      }
      played = true;
      _root.Course.Platforms.gotoAndStop(2);
      AL = function()
      {
         if(_alpha <= 0)
         {
            _alpha = 0;
            AL = null;
         }
         else
         {
            _alpha = _alpha - 2;
         }
      };
   }
   else if(_root.Course.BackGFX.inroom !== true && _alpha < 100)
   {
      if(played == true)
      {
         _root.StartNewMusicAndIntroNoVar("InsidetheCastleWalls");
      }
      played = false;
      _root.Course.Platforms.gotoAndStop(1);
      AL = function()
      {
         if(_alpha >= 100)
         {
            _alpha = 100;
            AL = null;
         }
         else
         {
            _alpha = _alpha + 2;
         }
      };
   }
};
