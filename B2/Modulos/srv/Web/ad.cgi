t <html><head><title>AD Input</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("ad.cgx", 500);
t function plotADGraph() {
t  adVal = document.getElementById("ad_value").value;
t  numVal = parseInt(adVal, 16);
t  voltsVal = (3.3*numVal*1000)/4096;
t  tableSize = (numVal*100/4096);
t  document.getElementById("ad_table").style.width = (tableSize + '%');
t  document.getElementById("ad_volts").value = (voltsVal.toFixed(3) + ' mA');
t }
t function periodicUpdateAd() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
#i pg_header.inc
t <body background="background3.png" style="background-repeat: no-repeat;">
t <div style="width: 1060;">
t <h2 align="center" ><br>Consumo</h2>
t <td style="border-bottom: 1px solid #000080" align=center vAlign=center noWrap width="70"> 
t <a href="index.htm"><img border="0" src="home.png" width="31" height="29"></a>
t <form action="ad.cgi" method="post" name="ad">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #94C8A5">
#t  <th width=15%>Item</th>
#t  <th width=15%>Value</th>
t  <th width=15%>Consume</th></tr>
#t  <th width=55%>Bargraph</th>
#t <td><img src="pabb.gif">POT1:</td>
t <tr><td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c x 1  size="10" id="ad_value" value="0x%03X"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c x 2  size="10" id="ad_volts" value="%5.0f mA"></td>
t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
c x 3 <td><table id="ad_table" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
t <tr><td bgcolor="#0000FF">&nbsp;</td></tr></table></td></tr></table></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
#i pg_footer.inc
. End of script must be closed with period
