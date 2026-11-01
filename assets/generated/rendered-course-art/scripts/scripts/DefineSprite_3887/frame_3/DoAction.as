stop();
_X = startX;
_Y = startY;
_root.Course.Platforms["KingBombombGround" + depth]._y = _Y;
_root.Course.Platforms["KingBombombGround" + depth]._x = _X;
onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.attack == false && _root.Course.Char.air == false)
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "bosscinema";
      _root.Course.Char._xscale = 100;
      _root.playpause = true;
      onEnterFrame = function()
      {
         _root.Camcenter = this;
         _root.PauseGame = false;
         count = 0;
         _root.StopBGsong();
         _root.PlayMessage("Mario... I remember you... You can\'t grab me and throw me this time, so I\'ll destroy you with my bombs!!");
         clip.gotoAndStop(5);
         onEnterFrame = function()
         {
            _root.playpause = true;
            count++;
            if(_root.KeyZ() && count > 5)
            {
               _root.StopMessage();
               count = 0;
               onEnterFrame = function()
               {
                  count++;
                  if(count > 5)
                  {
                     _root.Course.Char.attack = false;
                     _root.playpause = false;
                     onEnterFrame = null;
                     _root.StopBGsong();
                     _root.StartBoss64Music();
                     _root.Course.Edge.gotoAndStop(2);
                     _root.Course.CamEdge.gotoAndStop(2);
                     nextFrame();
                  }
               };
            }
         };
      };
   }
};
