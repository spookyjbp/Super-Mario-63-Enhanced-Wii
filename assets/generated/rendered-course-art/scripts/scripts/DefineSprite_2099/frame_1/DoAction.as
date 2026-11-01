function startAd(ngad_url)
{
   trace("opening " + ngad_url);
   System.security.allowDomain("70.87.128.99");
   System.security.allowInsecureDomain("70.87.128.99");
   System.security.allowDomain("ads.shizmoo.com");
   System.security.allowInsecureDomain("ads.shizmoo.com");
   System.security.allowDomain("www.cpmstar.com");
   System.security.allowInsecureDomain("www.cpmstar.com");
   System.security.allowDomain("server.cpmstar.com");
   System.security.allowInsecureDomain("server.cpmstar.com");
   var ngads_redirect = new XML();
   ngads_redirect.ignoreWhite = true;
   ngads_redirect.onLoad = function(success)
   {
      trace("[NEWGROUNDS FLASH ADS] :: You may get a \'Security Sandbox Violation\' ... this is normal, do not freak out!");
      if(success)
      {
         ng_ad.loadMovie(ngads_redirect.toString(),"GET");
      }
   };
   ngads_redirect.load(ngad_url);
}
if(NewgroundsAPI.getAdURL())
{
   startAd(NewgroundsAPI.getAdURL());
}
NewgroundsAPI.onAdsApproved = function(ad_url)
{
   startAd(ad_url);
};
stop();
