widt = 150;
heigh = 150;
xspeed = _root.LDItemArray[itemnum][3];
yspeed = _root.LDItemArray[itemnum][4];
xdirection = _root.LDItemArray[itemnum][5];
OEF = function()
{
   _root.LDItemArray[itemnum][3] = xspeed;
   _root.LDItemArray[itemnum][4] = yspeed;
   _root.LDItemArray[itemnum][5] = xdirection;
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
};
