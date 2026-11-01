if(rotation == undefined)
{
   rotation = _rotation;
}
if(otherstar == undefined)
{
   depth = _root.Course.FrontGFX.getNextHighestDepth();
   otherstar = "Slingstar" + depth;
   _root.Course.FrontGFX.attachMovie("SlingStarFrontGFX","Slingstar" + depth,depth,{_x:_X,_y:_Y,_rotation:_rotation,_xscale:_xscale,_yscale:_yscale});
}
