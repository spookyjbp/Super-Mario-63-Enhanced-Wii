widt = 250;
heigh = 255;
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
blocktype = _root.LDItemArray[itemnum][13];
rotdirection = _root.LDItemArray[itemnum][14];
rotspeed = _root.LDItemArray[itemnum][15];
rotwait = _root.LDItemArray[itemnum][16];
quake = _root.LDItemArray[itemnum][17];
clip.clip._xscale = Number(sizevar);
clip.clip._yscale = Number(sizevar);
clip.clip.gotoAndStop(Number(blocktype));
blocknum = 1;
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
   clip.clip.blocktype = Number(blocktype);
   clip.clip.rotdirection = String(rotdirection);
   clip.clip.rotspeed = Number(rotspeed);
   clip.clip.rotwait = Number(rotwait);
   clip.clip.quake = Boolean(Number(quake));
   clip.clip.gotoAndStop(Number(blocktype));
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
   _root.LDItemArray[itemnum][13] = blocktype;
   _root.LDItemArray[itemnum][14] = rotdirection;
   _root.LDItemArray[itemnum][15] = rotspeed;
   _root.LDItemArray[itemnum][16] = rotwait;
   _root.LDItemArray[itemnum][17] = quake;
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
