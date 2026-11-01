count = 0;
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("Explode9");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      tellTarget(_parent)
      {
         myColor = new Color(_parent);
         colorTrans = new Object();
         colorTrans = {ra:100 - _parent.hp * 10 - this.count * 5,rb:_parent.hp * 25 + this.count * 5,ga:100 - _parent.hp * 10 - this.count * 5,gb:0,ba:100 - _parent.hp * 10 - this.count * 5,bb:0,aa:100,ab:0};
         myColor.setTransform(colorTrans);
      }
      _alpha = _alpha - 2.5;
      if(_parent._xscale > 0)
      {
         _parent._xscale += 3;
      }
      else
      {
         _parent._xscale -= 3;
      }
      _parent._yscale += 3;
      play();
   }
   else
   {
      stop();
   }
};
