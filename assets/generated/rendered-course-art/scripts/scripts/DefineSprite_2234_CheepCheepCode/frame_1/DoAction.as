if(turncount == undefined)
{
   turncount = 96;
}
if(idlespeed == undefined)
{
   idlespeed = 2;
}
if(chasespeed == undefined)
{
   chasespeed = 0.5;
}
if(hungry == undefined)
{
   hungry = true;
}
if(scale == undefined)
{
   scale = 1;
}
wide = 9;
tall = 8;
_xscale = scale * 100;
_yscale = scale * 100;
restartvars = function()
{
   stopchasing = 0;
   chase = false;
   xspeed = 0;
   yspeed = 0;
   Direction = 1;
   count = random(96) + 1;
   frame = "swim";
   clip.gotoAndStop(frame);
   clip.frame.gotoAndPlay(random(19) + 1);
};
restartvars();
saveY = _Y;
saveX = _X;
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
HitTest = function(ground, X, Y)
{
   if(_root.Course[ground].hitTest(X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      return true;
   }
   return false;
};
