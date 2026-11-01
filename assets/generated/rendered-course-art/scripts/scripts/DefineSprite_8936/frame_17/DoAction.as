widt = 150;
heigh = 160;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
speed = Number(_root.LDItemArray[itemnum][3]);
rot = Number(_root.LDItemArray[itemnum][4]);
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
   _root.LDItemArray[itemnum][3] = Number(speed);
   _root.LDItemArray[itemnum][4] = Number(rot);
   clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
};
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
