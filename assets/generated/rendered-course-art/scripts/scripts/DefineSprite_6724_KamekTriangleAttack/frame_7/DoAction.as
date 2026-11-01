_root.Course.BackGFX.attachMovie("BombombCode","Bombomb" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y,battleArena:true,attached:true});
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      if(count > 37)
      {
         this.removeMovieClip(this);
      }
   }
};
