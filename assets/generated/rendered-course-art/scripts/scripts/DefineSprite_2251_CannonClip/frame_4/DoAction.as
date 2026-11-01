onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.Course.Char._x = _X + _parent._x;
   _root.Course.Char._y = _Y + _parent._y;
   _root.Course.Char.attack = true;
   _root.Course.Char.xspeed = 0;
   _root.Course.Char.yspeed = 0;
   _root.Course.Char.attackFrame = "none";
};
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("cannon1");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
