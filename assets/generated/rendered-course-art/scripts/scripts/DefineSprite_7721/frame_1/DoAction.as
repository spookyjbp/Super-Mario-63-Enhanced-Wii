TileCountArray = [45,32,50,48,45,47];
TileCountArray.push(44,41,49,48,50,94);
Tab = 0;
_parent.movedoffset = 0;
Reload = function()
{
   i = 1;
   w = -18.5;
   h = 18.5;
   Selected = undefined;
   q = 0;
   while(q < 100)
   {
      if(this["LDSBTile" + i]._x !== undefined)
      {
         this["LDSBTile" + i].removeMovieClip();
      }
      if(i <= TileCountArray[Tab])
      {
         this.attachMovie("LDSBTile","LDSBTile" + i,this.getNextHighestDepth(),{_y:h,_x:w,num:i});
         this["LDSBTile" + i].gotoAndStop(i + Tab * 50);
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
