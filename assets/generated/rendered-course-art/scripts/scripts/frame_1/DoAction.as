function menuHandler(obj, menuObj)
{
   if(_root.AutoQuality == true)
   {
      menuObj.customItems[1].caption = "Turn Auto Quality Off";
   }
   else
   {
      menuObj.customItems[1].caption = "Turn Auto Quality On";
   }
   if(_root.MuteBGMusic == true)
   {
      menuObj.customItems[2].caption = "Unmute Background Music";
   }
   else
   {
      menuObj.customItems[2].caption = "Mute Background Music";
   }
   menuObj.customItems[1].separatorBefore = true;
   menuObj.customItems[3].separatorBefore = true;
   menuObj.customItems[4].enabled = true;
   menuObj.customItems[5].enabled = true;
   menuObj.customItems[6].enabled = true;
   menuObj.customItems[7].enabled = true;
}
function ContextHomepage(obj, item)
{
   getURL("http://www.runouw.com", "_blank");
}
function ContextEmail(obj, item)
{
}
function ContextAutoQuality(obj, item)
{
   _root.AutoQuality = !_root.AutoQuality;
}
function ContextMuteBGMusic(obj, item)
{
   _root.MuteBGMusic = !_root.MuteBGMusic;
   if(_root.MuteBGMusic == true)
   {
      _root.StopBGsong();
   }
   else
   {
      _root.PlayMusicAndIntro();
   }
}
function ContextQualityLow(obj, item)
{
   _root._quality = "LOW";
   _root.BGquality = "LOW";
}
function ContextQualityMedium(obj, item)
{
   _root._quality = "MEDIUM";
   _root.BGquality = "MEDIUM";
}
function ContextQualityHigh(obj, item)
{
   _root._quality = "HIGH";
   _root.BGquality = "HIGH";
}
function ContextQualityBest(obj, item)
{
   _root._quality = "BEST";
   _root.BGquality = "HIGH";
}
stop();
System.security.allowDomain("*");
System.security.loadPolicyFile("http://runouw.awardspace.com/crossdomain.xml");
var newMenu = new ContextMenu(menuHandler);
newMenu.hideBuiltInItems();
var showItem = true;
newMenu.customItems.push(new ContextMenuItem("Runouw\'s Website",ContextHomepage));
newMenu.customItems.push(new ContextMenuItem("Turn Auto Quality On",ContextAutoQuality));
newMenu.customItems.push(new ContextMenuItem("Mute Background Music",ContextMuteBGMusic));
newMenu.customItems.push(new ContextMenuItem("Low Quality",ContextQualityLow));
newMenu.customItems.push(new ContextMenuItem("Medium Quality",ContextQualityMedium));
newMenu.customItems.push(new ContextMenuItem("High Quality",ContextQualityHigh));
newMenu.customItems.push(new ContextMenuItem("Best Quality",ContextQualityBest));
this.menu = newMenu;
block1 = false;
block2 = false;
block3 = false;
block3count = 0;
_root.TotalCoins = 0;
onEnterFrame = function()
{
   percentloaded = Math.round(_root.getBytesLoaded() / _root.getBytesTotal() * 100);
   percenttext = percentloaded;
};
