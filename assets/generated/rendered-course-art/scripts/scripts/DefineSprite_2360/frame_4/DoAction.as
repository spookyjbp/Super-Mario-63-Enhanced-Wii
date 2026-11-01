if(number == undefined)
{
   number = 1;
}
else
{
   number++;
   if(number > 3)
   {
      number = 0;
   }
}
if(_root.CreditsQuiet !== true)
{
   if(_parent._parent.distancevolume > 60)
   {
      this["walksound" + number] = new Sound(this);
      this["walksound" + number].attachSound("Goomba-Walk");
      this["walksound" + number].start(0,1);
      this["walksound" + number].setVolume(_root.MarioVolume / 100 * _parent._parent.distancevolume);
      this["walksound" + number].setPan(_parent._parent.panX);
   }
   else if(_parent._parent.distancevolume > 30)
   {
      this["walksound" + number] = new Sound(this);
      this["walksound" + number].attachSound("Goomba-Walk2");
      this["walksound" + number].start(0,1);
      this["walksound" + number].setVolume(_root.MarioVolume / 100 * _parent._parent.distancevolume);
      this["walksound" + number].setPan(_parent._parent.panX);
   }
}
