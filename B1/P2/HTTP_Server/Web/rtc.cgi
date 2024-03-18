t <html><head><title>RTC Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var rtcUpdate = new periodicObj("rtc.cgx", 1000);
t function updateValueRTC() {
t  rtcVal = document.getElementById("rtc_value").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(rtcUpdate,updateValueRTC);
t   rtc_elTime = setTimeout(periodicUpdateRTC, rtcUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Control del RTC</h2>
t <p><font size="2">Esta página muestra el valor del tiempo y la fecha del <b>Modulo RTC</b>.
#t  Simply enter the text which you want to display and click <b>Send</b>.<br><br>
#t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action="rtc.cgi" method="post" name="rtc">
t <input type="hidden" value="rtc" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=100%>Tiempo y Fecha</th>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 1 size="60" id="rtc_time_value" value="%s"></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(rtcUpdate,updateValueRTC)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateRTC()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
