_visible = false;
if(t == undefined)
{
   t = "Y";
}
if(amount == undefined)
{
   amount = 100;
}
if(t == "X")
{
   amount *= -1;
}
if(speed == undefined || speed < 1)
{
   speed = 15;
}
if(FindTrigger == undefined)
{
   FindTrigger = function()
   {
      Trigger = this.hitTest(_root.Course.Char.charClip.body);
   };
}
_root.CamYchangeVar = 0;
_root.CamXchangeVar = 0;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      FindTrigger();
      lastcount = count;
      if(Trigger)
      {
         count += (Math.abs(amount) - count) / speed;
      }
      else
      {
         count -= count / speed;
      }
      if(amount > 0)
      {
         _root["Cam" + t + "changeVar"] += count - lastcount;
      }
      else if(amount < 0)
      {
         _root["Cam" + t + "changeVar"] -= count - lastcount;
      }
   }
   _root.CamYchange = _root.CamYchangeVar + 50;
   _root.CamXchange = _root.CamXchangeVar;
};
