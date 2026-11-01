count = 0;
_root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y,_yscale:100,_xscale:100,time:12});
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(2,90,-9,9,7,"");
         }
         else
         {
            _root.MarioHit(2,90,-9,-9,7,"");
         }
      }
      count++;
      if(count > 20)
      {
         this.removeMovieClip(this);
      }
   }
};
