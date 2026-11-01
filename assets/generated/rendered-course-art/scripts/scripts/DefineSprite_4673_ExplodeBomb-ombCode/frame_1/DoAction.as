if(yspeed == undefined)
{
   yspeed = 0;
}
if(xspeed == undefined)
{
   xspeed = 0;
}
if(scale == undefined)
{
   scale = 1;
}
_xscale = scale * 100;
_yscale = scale * 100;
if(rotation == undefined)
{
   rotation = random(360);
}
sounded = false;
explode = false;
explodetimer = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(explode == false)
      {
         tumblespeed = xspeed / 2;
         rotation += tumblespeed;
         clip.frame._rotation = rotation;
         clip.gotoAndStop(1);
         if(yspeed > 0 && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            explode = true;
         }
         if(xspeed < 0 && _root.Course.Platforms.hitTest((_X - 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            explode = true;
         }
         if(xspeed > 0 && _root.Course.Platforms.hitTest((_X + 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            explode = true;
         }
         if(yspeed < 0 && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            explode = true;
         }
         if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            yspeed -= 1.4 / scale;
            xspeed += (Math.random() * 4 - 2 - xspeed / 1.5) / scale;
            _Y = _Y - 3;
         }
         else
         {
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.max((12 - xspeed / 1.5) / scale,0);
               _Y = _Y - 3;
            }
            if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.min((-12 - xspeed / 1.5) / scale,0);
               _Y = _Y - 3;
            }
         }
         if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water) || this.head.hitTest(_root.Course.Char.charClip.frame.water))
         {
            yspeed -= 1.5 / scale;
            if(_root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true)
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  xspeed *= 0.7;
                  xspeed -= 20 / scale;
               }
               else
               {
                  xspeed *= 0.7;
                  xspeed += 20 / scale;
               }
            }
            else if(_root.Course.Char._xscale > 0)
            {
               if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
               {
                  xspeed *= 0.9;
                  xspeed += 6 / scale;
               }
               else
               {
                  xspeed *= 0.9;
                  xspeed -= 3 / scale;
               }
            }
            else if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
            {
               xspeed *= 0.9;
               xspeed -= 6 / scale;
            }
            else
            {
               xspeed *= 0.9;
               xspeed += 3 / scale;
            }
            _Y = _Y - 3;
         }
         if(this.body.hitTest(_root.Course.Char.charClip.body) && (Math.abs(_root.Course.Char.xspeed) >= 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
         {
            yspeed -= Math.abs((_root.Course.Char.xspeed + 1) / 5) / scale;
            xspeed += (_root.Course.Char.xspeed - xspeed / 1.5) / scale * 0.8;
            _Y = _Y - 3;
         }
         explodetimer++;
         myColor = new Color(this);
         colorTrans = new Object();
         powerblink = Math.sin(_root.angleToRadians(Math.pow(explodetimer,3) / 700)) * 25 + 25 + explodetimer / 6;
         colorTrans = {ra:100,rb:Math.pow(explodetimer,2) / 75 + powerblink,ga:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,gb:0,ba:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,bb:0,aa:100,ab:0};
         myColor.setTransform(colorTrans);
         if(explodetimer >= 96)
         {
            explode = true;
         }
         yspeed += _root.gravity;
         if(yspeed > 7)
         {
            yspeed *= 0.9;
         }
         repeats = Math.round((Math.abs(xspeed) + Math.abs(yspeed)) / 5) + 1;
         repeats = Math.min(repeats,15);
         i = 0;
         while(i < repeats)
         {
            if(i > 5)
            {
               break;
            }
            _X = _X + xspeed / repeats;
            _Y = _Y + yspeed / repeats;
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = 0;
               _Y--;
            }
            while(_root.Course.Platforms.hitTest((_X - 12) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = 90;
               _X++;
            }
            while(_root.Course.Platforms.hitTest((_X + 12) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = -90;
               _X--;
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 10) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = -180;
               _Y++;
            }
            while(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = 0;
               _Y--;
            }
            while(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               explode = true;
               rotation = 0;
               _Y--;
            }
            i++;
         }
      }
      else
      {
         if(this.clip.frame.explode.hitTest(_root.Course.Char.charClip.body))
         {
            if(_root.Course.Char._x > _X + _parent._x)
            {
               _root.MarioHit(2,90,-7,5,10,"Mario-Squish",true);
            }
            else
            {
               _root.MarioHit(2,90,-7,-5,10,"Mario-Squish",true);
            }
         }
         if(sounded == false)
         {
            explodetimer = 0;
            myColor = new Color(this);
            colorTrans = new Object();
            powerblink = Math.sin(_root.angleToRadians(Math.pow(explodetimer,3) / 700)) * 25 + 25 + explodetimer / 6;
            colorTrans = {ra:100,rb:Math.pow(explodetimer,2) / 75 + powerblink,ga:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,gb:0,ba:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,bb:0,aa:100,ab:0};
            myColor.setTransform(colorTrans);
            sounded = true;
            distancevolume = Math.sqrt(Math.pow(_root.currentCamX - (_X + _parent._x),2) + Math.pow(_root.currentCamY - (_Y + _parent._y),2));
            distancevolume /= 100;
            distancevolume *= 15;
            distancevolume = Math.min(100,distancevolume);
            distancevolume = Math.max(0,distancevolume);
            distancevolume -= 100;
            distancevolume *= -1;
            soundclip = new Sound(this);
            soundclip.attachSound("Explosion");
            soundclip.start(0,1);
            soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
            soundclip.setPan(panX);
            _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 15,_yscale:30,_xscale:30,time:6});
         }
         clip.frame._rotation = rotation;
         clip.gotoAndStop(2);
      }
   }
};
