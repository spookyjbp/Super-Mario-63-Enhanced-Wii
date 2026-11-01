onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Dive";
      _root.Course.Char.xspeed = -15;
      _root.Course.Char.yspeed = -7;
      _root.StopBGsong();
      onEnterFrame = function()
      {
         _root.Camcenter = _parent._parent;
         _root.PauseGame = false;
      };
      gotoAndStop("play");
      play();
   }
};
