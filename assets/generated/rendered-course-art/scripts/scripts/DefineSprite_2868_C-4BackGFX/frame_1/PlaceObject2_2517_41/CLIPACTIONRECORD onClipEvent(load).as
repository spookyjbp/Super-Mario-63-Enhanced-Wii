onClipEvent(load){
   if(_root.TotalStars <= 0 && _root.Star[39] == false)
   {
      staramount = 0;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "<b><i>Eddie: </i></b>OK, Give me the Shine Sprite;";
      M1Sta = function()
      {
         if(count2++ > 5)
         {
            _root.Course.Char._xscale = -100;
            return true;
         }
         return false;
      };
      standnext = false;
      M1End = function()
      {
         return BP();
      };
      count2 = 0;
      M2Moo = "Happy";
      M2Txt = "<b><i>Eddie: </i></b> I\'ll keep watch over the Shine Sprite Orb as you go retrieve more of the Shine Sprites....";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M2F = function()
      {
         M1Sta = function()
         {
            return BP();
         };
         M1Moo = "Talk";
         M1Txt = "<b><i>Eddie: </i></b> I\'ll keep watch over the Shine Sprite Orb as you go retrieve more of the Shine Sprites....";
      };
      M3Moo = "Sad";
      M3Txt = "<b><i>Eddie: </i></b>....  ...... ...   .... ";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M3F = function()
      {
         M2Moo = undefined;
         M2Txt = undefined;
         M2Sta = undefined;
         M2End = undefined;
         M2F = undefined;
         _root.Course.Char.charClip.frame.gotoAndPlay("returnstar");
      };
      M4Moo = "Happy";
      M4Txt = "<b><i>Eddie: </i></b>There! Whenever you collect a Shine Sprite, it should automatically place itself here!";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Talk";
      M5Txt = "<b><i>Eddie: </i></b>Now we just need enough to overrule the spells Bowser just placed on the Castle...";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Talk";
      M6Txt = "<b><i>Eddie: </i></b>For our next Shine Sprite, I thought I saw a door on the Left Side of the Main Room...";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M7Moo = "Talk";
      M7Txt = "<b><i>Eddie: </i></b>Go over there; I\'ll stay here and make sure Kamek or Bowser doesn\'t do anything to the Shine Sprite we have...";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         if(BP())
         {
            _parent.SaveGame = true;
         }
         return BP();
      };
   }
   else
   {
      staramount = 0;
      _xscale = -100;
      Mood = "Smile";
      M1Moo = "Happy";
      M1Txt = "<b><i>Eddie: </i></b> Hi Mario!";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talk";
      M2Txt = "<b><i>Eddie: </i></b> There must be lots of Shine Sprites hidden in the Castle...";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      if(_root.TotalStars > 10 && _root.TotalStars < 22)
      {
         M2Moo = "Talk";
         M2Txt = "<b><i>Eddie: </i></b> Wow!  We\'ve collected quite a few Shine Sprites...";
         M2Sta = function()
         {
            return true;
         };
         M2End = function()
         {
            return BP();
         };
      }
      else if(_root.TotalStars >= 22 && _root.TotalStars < 30)
      {
         M2Moo = "Talk";
         M2Txt = "<b><i>Eddie: </i></b>Amazing!  We\'ve collected a lot of Shine Sprites...";
         M2Sta = function()
         {
            return true;
         };
         M2End = function()
         {
            return BP();
         };
         M3Moo = "Talk";
         M3Txt = "<b><i>Eddie: </i></b>But there is still more to go before we can stop Bowser\'s evil plan.";
         M3Sta = function()
         {
            return true;
         };
         M3End = function()
         {
            return BP();
         };
      }
      else if(_root.Star[36] == true)
      {
         M2Moo = "Talk";
         M2Txt = "<b><i>Eddie: </i></b>Now that you\'ve defeated Bowser, there are still many stuff still to do in the Castle...";
         M2Sta = function()
         {
            return true;
         };
         M2End = function()
         {
            return BP();
         };
      }
      else if(_root.Star[36] == false && _root.TotalStars >= 30)
      {
         M2Moo = "Talk";
         M2Txt = "<b><i>Eddie: </i></b>We\'re so close to beating Bowser!";
         M2Sta = function()
         {
            return true;
         };
         M2End = function()
         {
            return BP();
         };
      }
   }
}
