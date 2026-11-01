if(_root.LevelDesignerMenu == true)
{
   restartvar = false;
   this.createEmptyMovieClip("line",this.getNextHighestDepth());
   onEnterFrame = function()
   {
      if(restartvar == true)
      {
         restart();
         onEnterFrame = function()
         {
            doortype = _root.LDItemArray[num][5];
            door.gotoAndStop(Number(_root.LDItemArray[num][5]));
            clip._x = Number(_root.LDItemArray[num][3]) - _X;
            clip._y = Number(_root.LDItemArray[num][4]) - _Y;
            line.clear();
            line.lineStyle(1,16777215,50,true);
            line.beginFill();
            line.moveTo(0,0);
            line.lineTo(clip._x,clip._y);
            line.endFill();
         };
      }
      restartvar = true;
   };
   restart = function()
   {
      if(num !== undefined)
      {
         clip._x = Number(_root.LDItemArray[num][3]) - _X;
         clip._y = Number(_root.LDItemArray[num][4]) - _Y;
      }
   };
}
else
{
   clip._visible = false;
}
