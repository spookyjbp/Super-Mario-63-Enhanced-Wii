widt = 230;
heigh = 140;
rot = 0;
xs = 100;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
xtarget = Number(_root.LDItemArray[itemnum][3]);
ytarget = Number(_root.LDItemArray[itemnum][4]);
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
   _root.LDItemArray[itemnum][3] = xs;
   _root.LDItemArray[itemnum][4] = rot;
   clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
   _root.LDItemArray[itemnum][3] = Number(xtarget);
   _root.LDItemArray[itemnum][4] = Number(ytarget);
};
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
