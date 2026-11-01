hit = false;
_root.AreaTextClipF("Top of the Castle:\r<i>The Staircase</i>",1);
_root.Starnamenum = 0;
onEnterFrame = function()
{
   if((_root.TotalStars < 32 || _root.CurrentPlayer == "Luigi") && _root.CheatCastleUnlock == false)
   {
      if(_root.Course.Char._x - _root.Course.BackGFX._x > 120)
      {
         if(hit == false)
         {
            _root.StartNewMusicAndIntroNoVar("EndlessStairs-Repeat");
         }
         hit = true;
      }
      else
      {
         if(hit == true)
         {
            _root.StartNewMusicAndIntroNoVar("InsidetheCastleWalls");
         }
         hit = false;
      }
      if(this.endlessbox.hitTest(_root.Course.Char.charClip.feet))
      {
         _root.Course.Char._x -= 240;
         _root.Course.Char._y += 240;
         _root.MoveAllPlats(-240,240);
      }
   }
};
stop();
