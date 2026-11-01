stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X + _parent._x,_y:_Y + _parent._y,_yscale:30,_xscale:30,time:1});
      if(this.box.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.air == true && _root.Invincible !== true && _root.Invisible !== true)
      {
         if(_root.Course.Char._x > _X + _parent._x + _parent._x)
         {
            _root.MarioHit(2,90,-5,3,7,"Mario-Squish",true);
         }
         else
         {
            _root.MarioHit(2,90,-5,-3,7,"Mario-Squish",true);
         }
      }
      else if(this.box.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.blink <= 0 && _root.Invincible !== true && _root.Invisible !== true)
      {
         _root.MarioHit(2,90,0,0,0,"Mario-Squish");
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "Squish";
      }
   }
};
