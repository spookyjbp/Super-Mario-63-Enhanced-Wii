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
if(powerhit == undefined)
{
   powerhit = false;
}
seeing = false;
see = false;
hurt = false;
frame = "Idle";
if(xspeed == undefined && yspeed == undefined)
{
   xspeed = 0;
   yspeed = 0;
}
wide = 10 * scale;
tall = 25 * scale;
air = false;
if(tumble == undefined)
{
   tumble = false;
   tumblerot = 0;
}
_xscale = scale * 100;
_yscale = scale * 100;
if(explodetimer == undefined)
{
   explodetimer = 0;
}
if(exploding == undefined)
{
   exploding = false;
}
this.createEmptyMovieClip("soundclip",this.getNextHighestDepth());
if(attached == true)
{
   coinappear = true;
}
deadcount = 0;
if(tumble == false && attached !== true)
{
   coinappear = true;
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
   saveY = _Y;
   saveX = _X;
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
      explodetimer = 0;
      exploding = false;
   };
}
