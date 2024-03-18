t <html><head><title>Servidor Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de los Servidores NTP</h2>
t <p><font size="2">Esta página muestra los <b>Servidores NTP</b> que se puede elegir.
t <form action="sel_server.cgi" method="post" name="servidorSNTP">
t <input type="hidden" value="rtc" name="pg">
t <table border=0 width=99%><font size="3">
#
# Verde: d0f37b     Por defecto: aaccff
#
t <tr bgcolor=#d0f37b>
#
# Se ha añadido antes el </tr>
#
t  <th width=100%>Selección del servidor</th> </tr>
#t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
t </font></table>
t <p align=left>
t <form>
t <input type="radio" id="servidor1" name=serverSNTP value = "1">
t <label for="servidor 1">Servidor 1: Instituto IMDEA Software</label><br>
t <input type="radio" id="servidor2" name=serverSNTP value = "2">
t <label for="servidor 1">Servidor 2: RedIris </label><br>
t </form>
t <p align=center>
t <input type=submit name=set value="Send" id="valor">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.