count = 0;
_root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y,_yscale:35,_xscale:35,time:10});
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,90,-3,5,7,"");
         }
         else
         {
            _root.MarioHit(1,90,-3,-5,7,"");
         }
      }
      count++;
      if(count > 20)
      {
         this.removeMovieClip(this);
      }
   }
};
