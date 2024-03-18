t <html><head><title>Servidor Control</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de la Alarma</h2>
t <p><font size="2">Esta página sirve para configurar el <b>periodo de la alarma</b> y <b>la hora de la alarma</b>.
t <form action="PWM.cgi" method="post" name="PWM">
t <input type="hidden" value="Alarma" name="pg">
t <table border=0 width=99%><font size="3">
#
# Verde: d0f37b     Por defecto: aaccff
#
t <tr bgcolor=#d0f37b>
#
# Se ha añadido antes el </tr>
#
t  <th width=100%>Configuracion del PWM </th> </tr>
#t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
t </font></table>
t <p align=left>
t <form>
t <label for="horaAlarma">Elige las Horas (Entre 0 y 23):</label>
t <input type="number" id="horaAlarma" name="horaAlarma" min="0" max="23">
t <br><br><label for="minutosAlarma">Elige los Minutos (Entre 0 y 59):</label>
t <input type="number" id="minutosAlarma" name="minutosAlarma" min="0" max="59">
t <br><br><label for="segundosAlarma">Elige los Segundos (Entre 0 y 59):</label>
t <input type="number" id="segundosAlarma" name="segundosAlarma" min="0" max="59">
t <br><br><label for="quantity">Elige el periodo de la Alarma (Entre 100 ms y 1000 ms):</label>
t <input type="number" id="quantity" name="quantity" min="100" max="1000">
t </form>
t <p align=center>
t <input type=submit name=set value="Send" id="valor">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.