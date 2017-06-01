# traceroute
## **NAME** ##
trace - program, ktery umozni zobrazit pruchod paketu siti od zdroje k cili.

## **SYNOPSIS** ##
**trace** [**-f** *first_ttl*] [**-m** *max_ttl*] <ip-adress>

## **DESCRIPTION** ##
Program umozni zobrazit pruchod paketu siti od zdroje k cili. K objeveni jednotlivych smerovacu,
pres ktere paket prochazi, je vyuzito pole TTL/Hop limit protokolu IPv4/IPv6 a zpravy ICMP TIME_EXCEEDED.
Program zobrazuje IPv4/IPv6 adresu smerovace a dobu odezvy.
Pro zaslani testovaci zpravy s ruzne nastavenym TTL/HL vyuziva se protokol UDP.
## **OPTIONS** ##
**-f** *first_ttl* - parametr specifikuje, jake TTL se pouzije pro prvni paket. Implicitne 1.
**-m** *max_ttl* - parametr specifikuje maximalni TTL. Implicitne 30.
**<ip-adress>** - IPv4/IPv6 adresa cile
## **AUTHOR** ##
Ermak Aleksei
