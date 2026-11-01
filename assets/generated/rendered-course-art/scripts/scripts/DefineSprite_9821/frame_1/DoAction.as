if(speed == undefined)
{
   speed = -3;
}
onEnterFrame = function()
{
   this._rotation += speed;
};
