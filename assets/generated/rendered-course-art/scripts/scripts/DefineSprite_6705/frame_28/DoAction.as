count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      tellTarget(_parent)
      {
         myColor = new Color(_parent);
         colorTrans = new Object();
         colorTrans = {ra:100 - _parent.hp * 10,rb:_parent.hp * 25 + this.count * 5,ga:100 - _parent.hp * 10,gb:0,ba:100 - _parent.hp * 10,bb:0,aa:100,ab:0};
         myColor.setTransform(colorTrans);
      }
      if(_parent._xscale > 0)
      {
         _parent._xscale += 2;
      }
      else
      {
         _parent._xscale -= 2;
      }
      _parent._yscale += 2;
      play();
   }
   else
   {
      stop();
   }
};
