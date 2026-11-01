_root.FallSquish = function()
{
   tellTarget(_root.Course.Char)
   {
      nohurt = false;
      if((attack == false || attackFrame == "land") && fallingyspeed >= 14.5 && yspeed > 10)
      {
         hurtfall = Math.round((fallingyspeed - 14.5) * 3);
      }
      else if(fallingyspeed >= 21 && yspeed > 10 && attackFrame == "Ground")
      {
         hurtfall = Math.round((fallingyspeed - 23) / 2);
      }
      else
      {
         nohurt = true;
      }
      if(fallingyspeed >= 15 && yspeed > 10 && hurtfallcount > 3 && nohurt !== true)
      {
         hurtfall = Math.max(hurtfall,1);
         hurtfall = Math.min(hurtfall,4);
         _root.CharHP -= hurtfall;
         attack = true;
         attackFrame = "Squish";
      }
      hurtfall = 0;
      fallingyspeed = 0;
   }
};
