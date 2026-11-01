counter = 0;
onEnterFrame = function()
{
   if(invis == false && (_root.Course.Char.attack == false || _root.Course.Char.attackFrame == "Dive" || _root.Course.Char.attackFrame == "Run" || _root.Course.Char.attackFrame == "hurt1" || _root.Course.Char.attackFrame == "hurt2" || _root.Course.Char.attackFrame == "hurt3" || _root.Course.Char.attackFrame == "Spin" || _root.Course.Char.attackFrame == "Ground" || _root.Course.Char.attackFrame == "Land"))
   {
      star.play();
   }
   if(LevelDesigner == true)
   {
      counter++;
      trace("invis = " + invis);
      trace(_root.SilverStars + " > " + _root.LDSilverStar);
      if(_root.RedCoins >= _root.LDRedCoin && counter > 12 && _root.RedCoins !== 0)
      {
         invis = false;
      }
      if(_root.SilverStars >= _root.LDSilverStar && counter > 12 && _root.SilverStars !== 0)
      {
         invis = false;
      }
   }
};
