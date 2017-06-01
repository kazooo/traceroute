.TH CORRUPT 1
.SH NAME
trace - program, ktery umozni zobrazit pruchod paketu siti od zdroje k cili.
.SH SYNOPSIS
.B trace
[\fB\-f\fR \fIfirst_ttl\fR]
[\fB\-m\fR \fImax_ttl\fR]
<ip-adress>
.SH DESCRIPTION
Program umozni zobrazit pruchod paketu siti od zdroje k cili. K objeveni jednotlivych smerovacu,
pres ktere paket prochazi, je vyuzito pole TTL/Hop limit protokolu IPv4/IPv6 a zpravy ICMP TIME_EXCEEDED.
Program zobrazuje IPv4/IPv6 adresu smerovace a dobu odezvy.
Pro zaslani testovaci zpravy s ruzne nastavenym TTL/HL vyuziva se protokol UDP.
.SH OPTIONS
.TP
\fB\-f\fR \fIfirst_ttl\fR
Parametr specifikuje, jake TTL se pouzije pro prvni paket. Implicitne 1.
.TP
\fB\-m\fR \fImax_ttl\fR
Parametr specifikuje maximalni TTL. Implicitne 30.
.TP
<ip-adress>
IPv4/IPv6 adresa cile
.SH AUTHOR
Ermak Aleksei (xermak00@stud.fit.vutbr.cz)
