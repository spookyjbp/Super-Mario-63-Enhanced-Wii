stop();
var result_lv = new LoadVars();
result_lv.onLoad = function(success)
{
   if(success)
   {
      play();
      _parent._parent.scorestuff.nextFrame();
      _parent._parent.Score = result_lv.score;
      _parent._parent.score.text = "Score: " + _parent._parent.Score + "/5";
      _parent._parent.Votes1 = result_lv.votes;
      if(_parent._parent.Votes1 != "1")
      {
         _parent._parent.votes1.text = "Voted on " + _parent._parent.Votes1 + " times";
      }
      else
      {
         _parent._parent.votes1.text = "Voted on 1 time";
      }
   }
};
var send_lv = new LoadVars();
send_lv.id = _root.LDSelectedLevel;
send_lv.score = num;
send_lv.sendAndLoad("http://runouw.com/levels/leveldesigner/votescore.php",result_lv,"GET");
