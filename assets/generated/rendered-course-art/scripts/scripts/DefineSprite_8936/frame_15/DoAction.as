widt = 150;
heigh = 140;
rot = 0;
xs = 100;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
xs = _root.LDItemArray[itemnum][3];
rot = _root.LDItemArray[itemnum][4];
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
   _root.LDItemArray[itemnum][3] = xs;
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
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
