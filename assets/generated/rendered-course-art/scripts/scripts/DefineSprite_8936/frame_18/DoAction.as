widt = 240;
heigh = 140;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
rot = Number(_root.LDItemArray[itemnum][3]);
len = Number(_root.LDItemArray[itemnum][4]);
dep = Number(_root.LDItemArray[itemnum][5]);
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
   _root.LDItemArray[itemnum][3] = rot;
   _root.Scene2["Item" + itemnum]._rotation = _root.LDItemArray[itemnum][3];
   clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
};
ApplyF = function()
{
   _root.LDItemArray[itemnum][4] = Number(len);
   _root.LDItemArray[itemnum][5] = Number(dep);
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
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
