t <html><head><title>Registro de entradas</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("checking.cgx", 300);
t function plotADGraph() {
# entreda 0
t  id0 = document.getElementById("identidad0").value;
t  time0 = document.getElementById("fecha_hora0").value;
t  tipo_acceso0 = document.getElementById("acceso0").value;
t  dir_imagen0 = document.getElementById("dirImagen0").value;
t  document.getElementById("id_targeta0").value = id0;
t  document.getElementById("hora_fecha0").value = time0;
t  document.getElementById("tipoacceso0").value = tipo_acceso0;
t }
t function periodicUpdate() {
t  if(document.getElementById("refreshChkBox").checked == true) {
t   updateMultiple(formUpdate);
t   periodicFormTime = setTimeout("periodicUpdate()", formUpdate.period); 
t  }
t  else
t   clearTimeout(periodicFormTime);
t }
t </script></head>
t <body bgcolor=#D7EFCB style="background-repeat: no-repeat;">
t <div style="width: 1060;">
t <h2 align="center" ><br>Lista de acceso</h2>
t <td style="border-bottom: 1px solid #000080" align=center vAlign=center noWrap width="70"> 
t <a href="index.htm"><img border="0" src="home.png" width="31" height="29"></a>
t <form action="checking.cgi" method="post" id="form1" name="form1">
t <table border="0" width=99%><font size="3">
t <tr bgcolor=#94C8A5>
t <th width=30%>Hora y Fecha</th>
t <th width=20%>Nombre</th>
t <th width=30%>Identificacion</th>
t <th width=20%>Acceso</th>
t </tr>
# COMIENZO DE ENTRADAS
t <tr>
c a 1 <td><span align="center"  type=text size="10" id="hora_fecha0"> %s </span></td>
c a 2 <td><span align="center"  type=text size="10" id="nombre0"> %s </span></td>
c a 3 <td><span align="center"  type=text size="10" id="id_targeta0"> %s </span></td>
c a 4 <td><span align="center"  type=text size="10" id="tipoacceso0"> %s </span></td>
t </tr>
t <tr>
c b 1 <td><span align="center"  type=text size="10"> %s </span></td>
c b 2 <td><span align="center"  type=text size="10"> %s </span></td>
c b 3 <td><span align="center"  type=text size="10"> %s </span></td>
c b 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c c 1 <td><span align="center"  type=text size="10"> %s </span></td>
c c 2 <td><span align="center"  type=text size="10"> %s </span></td>
c c 3 <td><span align="center"  type=text size="10"> %s </span></td>
c c 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c d 1 <td><span align="center"  type=text size="10"> %s </span></td>
c d 2 <td><span align="center"  type=text size="10"> %s </span></td>
c d 3 <td><span align="center"  type=text size="10"> %s </span></td>
c d 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c e 1 <td><span align="center"  type=text size="10"> %s </span></td>
c e 2 <td><span align="center"  type=text size="10"> %s </span></td>
c e 3 <td><span align="center"  type=text size="10"> %s </span></td>
c e 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c f 1 <td><span align="center"  type=text size="10"> %s </span></td>
c f 2 <td><span align="center"  type=text size="10"> %s </span></td>
c f 3 <td><span align="center"  type=text size="10"> %s </span></td>
c f 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c g 1 <td><span align="center"  type=text size="10"> %s </span></td>
c g 2 <td><span align="center"  type=text size="10"> %s </span></td>
c g 3 <td><span align="center"  type=text size="10"> %s </span></td>
c g 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c h 1 <td><span align="center"  type=text size="10"> %s </span></td>
c h 2 <td><span align="center"  type=text size="10"> %s </span></td>
c h 3 <td><span align="center"  type=text size="10"> %s </span></td>
c h 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t <tr>
c i 1 <td><span align="center"  type=text size="10"> %s </span></td>
c i 2 <td><span align="center"  type=text size="10"> %s </span></td>
c i 3 <td><span align="center"  type=text size="10"> %s </span></td>
c i 4 <td><span align="center"  type=text size="10"> %s </span></td>
t </tr>
t </font></table>
t </form>
# MENSAJE DE INFOMRACION DEL STANDBY
c z 1 <h4> %s </h4>
t </body>
. End of script must be closed with period.