widt = 150;
heigh = 140;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
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
