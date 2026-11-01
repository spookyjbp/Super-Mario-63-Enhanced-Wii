widt = 250;
heigh = 220;
xnum = Math.round(_root.LDItemArray[itemnum][1]);
ynum = Math.round(_root.LDItemArray[itemnum][2]);
speed = _root.LDItemArray[itemnum][3];
plcount = _root.LDItemArray[itemnum][4];
radius = _root.LDItemArray[itemnum][5];
offset = _root.LDItemArray[itemnum][6];
sizevar = _root.LDItemArray[itemnum][7];
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
PlayF = function()
{
   clip.clip.speed = Number(speed);
   clip.clip.num = Number(plcount);
   clip.clip.distance = Number(radius);
   clip.clip.angle = Number(offset);
   clip.clip.scale = Number(sizevar);
   clip.clip._xscale = 100 / (Number(radius) / 50);
   clip.clip._yscale = 100 / (Number(radius) / 50);
   clip.clip.Start();
};
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
};
ApplyF = function()
{
   _root.LDItemArray[itemnum][1] = Math.round(xnum);
   _root.LDItemArray[itemnum][2] = Math.round(ynum);
   _root.LDItemArray[itemnum][3] = speed;
   _root.LDItemArray[itemnum][4] = plcount;
   _root.LDItemArray[itemnum][5] = radius;
   _root.LDItemArray[itemnum][6] = offset;
   _root.LDItemArray[itemnum][7] = sizevar;
   _root.RestartItems();
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
CancelF = function()
{
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
