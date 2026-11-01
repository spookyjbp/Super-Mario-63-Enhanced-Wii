if(scale == undefined)
{
   scale = 1;
}
if(walkspeed == undefined)
{
   walkspeed = 0.5;
}
if(movedirection == undefined)
{
   randomnum = random(2);
   if(randomnum == 0)
   {
      movedirection = "right";
   }
   else
   {
      movedirection = "left";
   }
}
else
{
   randomnum = 0;
}
if(WalkDistance == undefined)
{
   WalkDistance = 100;
}
restartvars = function()
{
   deadcount = 0;
   powerhit = false;
   seeing = false;
   see = false;
   hurt = false;
   frame = "Idle";
   xspeed = 0;
   yspeed = 0;
   wide = 10 * scale;
   tall = 25 * scale;
   air = false;
   tumble = false;
   tumblerot = 0;
   _xscale = scale * 100;
   _yscale = scale * 100;
   bounce = false;
};
restartvars();
saveY = _Y;
saveX = _X;
this.createEmptyMovieClip("soundclip",this.getNextHighestDepth());
if(attached !== true)
{
   if(_root.Course.coincountnum == undefined)
   {
      _root.Course.coincountnum = 0;
   }
   else
   {
      _root.Course.coincountnum++;
   }
   if(_root["coindata" + _root.playingcourse][0] == undefined)
   {
      _root["coindata" + _root.playingcourse] = [""];
   }
   coinnum = _root.Course.coincountnum;
   if(_root["coindata" + _root.playingcourse][coinnum] == 1 && _root["coindata" + _root.playingcourse][coinnum] !== undefined)
   {
   }
}
