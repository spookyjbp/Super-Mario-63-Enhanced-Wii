ItemCountArray = [43,34,45];
Tab = 0;
_parent.movedoffset = 0;
Reload = function()
{
   i = 1;
   w = -18.5;
   h = 18.5;
   Selected = undefined;
   q = 0;
   while(q < 50)
   {
      if(this["LDSBItem" + i]._x !== undefined)
      {
         this["LDSBItem" + i].removeMovieClip();
      }
      if(i <= ItemCountArray[Tab])
      {
         this.attachMovie("LDSBItem","LDSBItem" + i,this.getNextHighestDepth(),{_y:h,_x:w,num:i + Tab * 50});
         this["LDSBItem" + i].gotoAndStop(i + Tab * 50);
         w *= -1;
         if(w < 0)
         {
            h += 37.5;
         }
      }
      i++;
      q++;
   }
};
Reload();
