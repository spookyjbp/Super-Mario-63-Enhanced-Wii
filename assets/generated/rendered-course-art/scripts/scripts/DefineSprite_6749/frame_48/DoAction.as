counter = 0;
onEnterFrame = function()
{
   counter += 0.3;
   counter = Math.min(counter,5);
   _root.Camspeed = 7 - counter;
   _root.Camcenter = _parent.Target2;
   _root.PauseGame = false;
};
