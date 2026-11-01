stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
   {
      if(mariowait == true)
      {
         Xdiff = _root.Course.Char._x - (_X + _parent._x);
         Ydiff = _root.Course.Char._y - (_Y + _parent._y);
         if(Math.abs(Xdiff) < Xrange && Ydiff > 0)
         {
            startcounting = true;
         }
         if(startcounting == true && frame == "idle" || frame == "ground")
         {
            count++;
         }
      }
      else if(frame == "idle" || frame == "ground")
      {
         count++;
      }
      if(frame == "idle" && count > waittime)
      {
         startcounting = false;
         yspeed += fallaccel;
         yspeed = Math.min(fallspeed,yspeed);
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            count = 0;
            frame = "ground";
            yspeed = 0;
            distancevolume = Math.sqrt(Math.pow(_root.currentCamX - (_X + _parent._x),2) + Math.pow(_root.currentCamY - (_Y + _parent._y),2));
            distancevolume /= 100;
            distancevolume *= 15;
            distancevolume = Math.min(100,distancevolume);
            distancevolume = Math.max(0,distancevolume);
            distancevolume -= 100;
            distancevolume *= -1;
            if(distancevolume >= 1)
            {
               randomnum = random(2) + 1;
               soundclip = new Sound(this);
               soundclip.attachSound("thwomp-" + randomnum);
               soundclip.start(0,1);
               soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
               panX = _root.currentCamX - (_X + _parent._x);
               panX /= -2;
               panX = Math.min(100,panX);
               panX = Math.max(-100,panX);
               soundclip.setPan(panX);
            }
         }
      }
      if(frame == "ground" && count > groundwaittime)
      {
         if(_Y - ycutoff >= startY)
         {
            yspeed -= riseaccel;
            yspeed = Math.max(- risespeed,yspeed);
         }
         else
         {
            yspeed += riseaccel;
            if(yspeed >= 0)
            {
               count = 0;
               yspeed = 0;
               frame = "idle";
            }
         }
      }
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
      {
         while(_root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
            _root.Course.Platforms["ThwompGround" + depth]._y = this._y;
            _root.Course.Platforms["ThwompGround" + depth]._x = this._x;
            _root.Course.BPlatforms["ThwompPlatform" + depth2]._y = this._y;
            _root.Course.BPlatforms["ThwompPlatform" + depth2]._x = this._x;
         }
         whilecount = 0;
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(whilecount++ > 5)
            {
               break;
            }
            _Y--;
            _root.Course.Platforms["ThwompGround" + depth]._y = this._y;
            _root.Course.Platforms["ThwompGround" + depth]._x = this._x;
            _root.Course.BPlatforms["ThwompPlatform" + depth2]._y = this._y;
            _root.Course.BPlatforms["ThwompPlatform" + depth2]._x = this._x;
         }
      }
      repeats = Math.round(Math.abs(yspeed) / 5) + 1;
      i = 0;
      while(i < repeats)
      {
         _Y = _Y + yspeed / repeats;
         _root.Course.Platforms["ThwompGround" + depth]._y = this._y;
         _root.Course.Platforms["ThwompGround" + depth]._x = this._x;
         _root.Course.BPlatforms["ThwompPlatform" + depth2]._y = this._y;
         _root.Course.BPlatforms["ThwompPlatform" + depth2]._x = this._x;
         if(yspeed > 0 && (_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
         {
            break;
         }
         i++;
      }
      _root.Course.Platforms["ThwompGround" + depth]._y = this._y;
      _root.Course.Platforms["ThwompGround" + depth]._x = this._x;
      _root.Course.BPlatforms["ThwompPlatform" + depth2]._y = this._y;
      _root.Course.BPlatforms["ThwompPlatform" + depth2]._x = this._x;
      tellTarget(_root.Course.BPlatforms["ThwompPlatform" + depth2])
      {
         if(yspeed < 0)
         {
            _Y = _Y - this.yspeed;
         }
         if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(this.yspeed < 0)
            {
               _root.Course.Char._y += this.yspeed;
            }
            if(landedon == false)
            {
            }
            landedon = true;
         }
         else
         {
            if(landedon == true)
            {
               _root.Course.Char.yspeed += this.yspeed;
            }
            landedon = false;
         }
         if(yspeed < 0)
         {
            _Y = _Y + this.yspeed;
         }
         _Y = _Y + this.yspeed;
      }
   }
};
