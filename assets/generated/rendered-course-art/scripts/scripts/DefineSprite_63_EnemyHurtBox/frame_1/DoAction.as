count = 0;
_visible = false;
if(time == undefined)
{
   time = 1;
}
onEnterFrame = function()
{
   if(count >= time)
   {
      this.removeMovieClip(this);
   }
   count++;
};
