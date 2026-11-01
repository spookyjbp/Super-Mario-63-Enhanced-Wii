widt = 240;
heigh = 205;
rot = 0;
xs = 100;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
dir = _root.LDItemArray[itemnum][3];
rot = _root.LDItemArray[itemnum][4];
speed = Number(_root.LDItemArray[itemnum][5]);
waittime = Number(_root.LDItemArray[itemnum][6]);
offset = Number(_root.LDItemArray[itemnum][7]);
fra = Number(_root.LDItemArray[itemnum][8]);
chase = Boolean(Number(_root.LDItemArray[itemnum][9]));
OEF = function()
{
   if(rot >= 360)
   {
      rot -= 360;
   }
   if(rot < 0)
   {
      rot += 360;
   }
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
   _root.LDItemArray[itemnum][3] = dir;
   _root.LDItemArray[itemnum][4] = rot;
   if(Boolean(Number(_root.LDItemArray[itemnum][3])) == true)
   {
      _root.Scene2["Item" + itemnum]._xscale = 100;
   }
   else
   {
      _root.Scene2["Item" + itemnum]._xscale = -100;
   }
   _root.Scene2["Item" + itemnum]._rotation = _root.LDItemArray[itemnum][4];
   clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
};
ApplyF = function()
{
   _root.LDItemArray[itemnum][5] = Number(speed);
   _root.LDItemArray[itemnum][6] = Number(waittime);
   _root.LDItemArray[itemnum][7] = Number(offset);
   _root.LDItemArray[itemnum][8] = Number(fra);
   _root.LDItemArray[itemnum][9] = Number(chase);
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
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
