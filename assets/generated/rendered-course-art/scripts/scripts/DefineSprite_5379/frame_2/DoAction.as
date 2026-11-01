stop();
if(cosine == undefined)
{
   cosine = -0.5;
}
if(tall == undefined)
{
   tall = 0.3;
}
if(speed == undefined)
{
   speed = 0.04;
}
if(Yoffstart == undefined)
{
   Yoffstart = 0;
}
saveY = _Y;
i = 0;
while(i < Yoffstart)
{
   cosine += speed;
   currentspeedY = Math.cos(cosine) * tall;
   _Y = _Y + currentspeedY;
   _Y = _Y - (_Y - saveY) / 50;
   i++;
}
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("TriangleDippingPLGround","TriangleDippingPLGround" + depth,depth,{_x:this._x,_y:this._y});
tellTarget(_root.Course.Platforms["TriangleDippingPLGround" + depth])
{
   landedon = false;
   lessthan = false;
   morethan = false;
}
currentspeedY = 0;
distancecountY = speedY + Yoffstart;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      cosine += speed;
      currentspeedY = Math.cos(cosine) * tall;
      _Y = _Y + currentspeedY;
      _Y = _Y - (_Y - saveY) / 50;
      tellTarget(_root.Course.Platforms["TriangleDippingPLGround" + depth])
      {
         if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.currentspeedY + 3) * _root.coursescale / 100 + _root.Course._y,false) && _root.Course.Char.yspeed > -3)
         {
            _root.Course.Char._y += this.currentspeedY;
            if(landedon == false)
            {
            }
            landedon = true;
         }
         else
         {
            if(landedon == true)
            {
               _root.Course.Char.yspeed += this.currentspeedY;
            }
            landedon = false;
         }
         _Y = this._y;
         _X = this._x;
      }
   }
};
