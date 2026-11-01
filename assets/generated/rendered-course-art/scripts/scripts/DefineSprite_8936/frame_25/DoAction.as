widt = 250;
heigh = 230;
xnum = Math.round(_root.LDItemArray[itemnum][1]);
ynum = Math.round(_root.LDItemArray[itemnum][2]);
xspeednum = _root.LDItemArray[itemnum][3];
yspeednum = _root.LDItemArray[itemnum][4];
xlengthnum = _root.LDItemArray[itemnum][5];
ylengthnum = _root.LDItemArray[itemnum][6];
xoffsetnum = _root.LDItemArray[itemnum][7];
yoffsetnum = _root.LDItemArray[itemnum][8];
xdirection = _root.LDItemArray[itemnum][9];
ydirection = _root.LDItemArray[itemnum][10];
accel = _root.LDItemArray[itemnum][11];
sizevar = _root.LDItemArray[itemnum][12];
touchngo = Boolean(Number(_root.LDItemArray[itemnum][13]));
disapearcount = _root.LDItemArray[itemnum][14];
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
   clip.clip.DirectionX = String(xdirection);
   clip.clip.DirectionY = String(ydirection);
   clip.clip.distanceX = Number(xlengthnum);
   clip.clip.distanceY = Number(ylengthnum);
   clip.clip.accelX = Number(accel);
   clip.clip.accelY = Number(accel);
   clip.clip.speedX = Number(xspeednum);
   clip.clip.speedY = Number(yspeednum);
   clip.clip.Xoffstart = Number(xoffsetnum);
   clip.clip.Yoffstart = Number(yoffsetnum);
   clip.clip.scale = Number(sizevar);
   clip.clip.touchngo = Boolean(touchngo);
   clip.clip.disapearcount = Number(disapearcount);
   clip.clip.Start();
};
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
};
ApplyF = function()
{
   _root.LDItemArray[itemnum][3] = xspeednum;
   _root.LDItemArray[itemnum][4] = yspeednum;
   _root.LDItemArray[itemnum][5] = xlengthnum;
   _root.LDItemArray[itemnum][6] = ylengthnum;
   _root.LDItemArray[itemnum][7] = xoffsetnum;
   _root.LDItemArray[itemnum][8] = yoffsetnum;
   _root.LDItemArray[itemnum][9] = xdirection;
   _root.LDItemArray[itemnum][10] = ydirection;
   _root.LDItemArray[itemnum][11] = accel;
   _root.LDItemArray[itemnum][12] = sizevar;
   _root.LDItemArray[itemnum][13] = Number(touchngo);
   _root.LDItemArray[itemnum][14] = disapearcount;
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
