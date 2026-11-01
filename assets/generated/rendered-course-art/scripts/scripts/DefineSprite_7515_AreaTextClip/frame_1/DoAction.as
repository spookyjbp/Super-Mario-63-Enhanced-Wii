if(frame == undefined)
{
   frame = 1;
}
gotoAndStop(frame);
if(txt == undefined)
{
   txt = "Area Name Clip";
}
if(_root.AreaTextAppear !== true && _root.Starnamenum !== 0)
{
   this.removeMovieClip();
}
if(_root.Starnamenum !== 0)
{
   _root.AreaTextAppear = false;
}
bg._alpha = 0;
stop();
