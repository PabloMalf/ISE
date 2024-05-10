t <html><head><title>RTC</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("rtc.cgx", 500);
t function plotADGraph() {
t  hourVal = document.getElementById("hour").value;
t  dateVal = document.getElementById("date").value;
t  document.getElementById("hour").value = (hourVal);
t  document.getElementById("date").value = (dateVal);
t }
t function periodicUpdateAd() {
t  if(document.getElementById("TiChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   Ti_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(Ti_elTime);
t }
#t function formatfunction() {
#t  if(document.getElementById("FormatBox").checked == true) {
#t   hourValue = parseInt(document.getElementById("hour").value);
#t   if(hourValue > 12)
#t     hourValue=hourValue-12;
#t   document.getElementById("hour").value = (hourValue);
#t  }
#t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>RTC</h2>
t <p><font size="2">This page show the <b>hour</b> and <b>date</b>. </font></p>
t <form action="rtc.cgi" method="post" name="rtc">
t <input type="hidden" value="rtc" name="rtc">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Value</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Hour:</td>
c h 1  <td><input type=text name=hour size="10" id="hour" value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Date:</td>
c h 2  <td><input type=text name=date size="10" id="date" value="%s"></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="TiChkBox" onclick="periodicUpdateAd()">
#t Format:<input type=checkbox id="FormatBox" onClick="formatfunction()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
