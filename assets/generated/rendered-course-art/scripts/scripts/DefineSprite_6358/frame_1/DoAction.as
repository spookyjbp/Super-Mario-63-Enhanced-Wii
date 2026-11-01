stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && count++ > 3)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.air == false && _root.Course.Char.yspeed > -3 && _root.Course.Char.attack == false)
      {
         _root.PlayMessage("Bwahahahahaha!! Mario, you may have made it this far in my trap, but you\'ll never get past my Evil Elevator!!");
         play();
         _root.Course.Char.yspeed = 0;
         _root.Course.Char.xspeed = 0;
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "bosscinema";
         onEnterFrame = function()
         {
            if(_root.PauseGame == false)
            {
               _root.playpause = true;
            }
         };
      }
   }
   count = Math.min(count,5);
};
