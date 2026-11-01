function deepObjectCopy(obj)
{
   if(typeof obj != "object" || object instanceof Button || object instanceof TextField || object instanceof MovieClip)
   {
      return obj;
   }
   var _loc3_;
   if(obj instanceof Boolean)
   {
      _loc3_ = new Boolean(Boolean.prototype.valueOf.call(obj));
   }
   else if(obj instanceof Number)
   {
      _loc3_ = new Number(Number.prototype.valueOf.call(obj));
   }
   else if(obj instanceof String)
   {
      _loc3_ = new String(String.prototype.valueOf.call(obj));
   }
   else if(obj.__constructor__)
   {
      if(typeof obj.clone == "function")
      {
         _loc3_ = obj.clone();
         if(_loc3_.__proto__ == obj.__proto__)
         {
            return _loc3_;
         }
      }
      _loc3_ = new obj.__constructor__();
   }
   else if(obj instanceof Array)
   {
      _loc3_ = [];
   }
   else
   {
      _loc3_ = {};
   }
   for(var _loc4_ in obj)
   {
      if(obj.hasOwnProperty(_loc4_))
      {
         _loc3_[_loc4_] = arguments.callee(obj[_loc4_]);
      }
   }
   return _loc3_;
}
