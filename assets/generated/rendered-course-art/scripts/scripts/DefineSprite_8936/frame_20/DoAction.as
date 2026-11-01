widt = 150;
heigh = 150;
rot = 0;
xs = 100;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
timer = Number(_root.LDItemArray[itemnum][3]);
if(isNaN(timer))
{
   timer = 30;
}
OEF = function()
{
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
   clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
   _root.LDItemArray[itemnum][3] = Number(timer);
};
DeleteF = function()
{
   _root.DeleteItem(itemnum);
   frame = undefined;
   _Y = 1000;
   trace("Deleted");
   gotoAndStop("blank");
};
