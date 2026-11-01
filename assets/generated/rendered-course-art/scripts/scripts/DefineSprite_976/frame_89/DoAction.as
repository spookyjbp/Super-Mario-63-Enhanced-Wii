onEnterFrame = function()
{
   _root.playpause = true;
   _root.Camcenter = _parent;
   alphacount -= 5;
   alphacount = Math.min(alphacount,100);
   this.attachMovie("ChargeLight","ChargeLight" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0,_rotation:random(360),_alpha:alphacount});
   _root.PauseGame = false;
};
