stop();
changemusic = true;
trace(_root.LastItemGot);
count = 0;
onEnterFrame = function()
{
   _root.Course.Char.xspeed = 0;
   _root.Course.Char.yspeed = - _root.gravity;
   _root.PauseGame = false;
   count++;
   if(count > 32)
   {
      if(changemusic !== false)
      {
         _root.PaintingSound2();
      }
      if(_root.LastItemGot == "gotStar")
      {
         gotoAndStop("gotstar");
         play();
      }
      if(_root.LastItemGot == "oldStar")
      {
         gotoAndStop("oldstar");
         play();
      }
      if(_root.LastItemGot == "Key")
      {
         gotoAndStop("gotitem");
         play();
      }
      if(_root.LastItemGot == "Exit")
      {
         gotoAndStop("exit");
         play();
      }
      if(_root.LastItemGot == "GameOver")
      {
         gotoAndStop("GameOver");
         play();
      }
      if(_root.LastItemGot == "BowserKey1")
      {
         gotoAndStop("bowserkey1");
         play();
      }
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
   }
};
