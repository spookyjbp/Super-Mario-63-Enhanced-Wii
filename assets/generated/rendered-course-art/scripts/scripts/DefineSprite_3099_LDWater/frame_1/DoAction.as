if(WaterDeepness == undefined)
{
   WaterDeepness = 400;
}
if(WaterLength == undefined)
{
   WaterLength = 500;
}
if(polygons == undefined)
{
   polygons = Math.round(WaterLength / 10) + 1;
}
if(_root.BGquality == "LOW")
{
   polygons = Math.round(polygons * 0.5);
}
if(_root.BGquality == "MEDIUM")
{
   polygons = Math.round(polygons * 0.75);
}
if(randomwave == undefined)
{
   randomwave = 1;
}
if(waterstength == undefined)
{
   waterstength = 0.3;
}
if(redcolor == undefined)
{
   redcolor = 40;
}
if(greencolor == undefined)
{
   greencolor = 128;
}
if(bluecolor == undefined)
{
   bluecolor = 215;
}
if(alpha == undefined)
{
   alpha = 50;
}
if(graphics == undefined)
{
   graphics = false;
}
