stop();
count = 0;
if(_root.BossCheckpoint == false)
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false && _root.playpause == false && count++ > 3)
      {
         if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.air == false && _root.Course.Char.yspeed > -3 && _root.Course.Char.attack == false)
         {
            _root.CinemaMessageFrame = 2;
            _root.CinemaMessage = "<b><i>Kamek:</i></b> Gah! " + _root.CurrentPlayer + " What are you doing here?!? You might mess up my Master\'s plans, so I\'ll have to kill you now!";
            play();
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "bosscinema";
            _root.playpause = true;
            onEnterFrame = function()
            {
               _root.Camcenter = this;
               _root.PauseGame = false;
            };
         }
      }
      count = Math.min(count,5);
   };
}
else
{
   gotoAndStop("m7");
   _root.Course.CamEdge.gotoAndStop(2);
   _root.Course.Platforms.gotoAndStop(2);
   _root.Course.BackGFX.gotoAndStop(4);
   _root.Course.FrontGFX.gotoAndStop(3);
}
