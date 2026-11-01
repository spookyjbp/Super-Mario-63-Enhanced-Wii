count = 0;
_root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 20,_yscale:50,_xscale:35,time:8,_rotation:_rotation});
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(2,90,-7,5,10,"Mario-Squish",true);
         }
         else
         {
            _root.MarioHit(2,90,-7,-5,10,"Mario-Squish",true);
         }
      }
      count++;
      if(count > 20)
      {
         this.removeMovieClip(this);
      }
   }
};
