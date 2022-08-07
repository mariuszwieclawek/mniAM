Aby uruchomić projekt docelowy potrzebujemy jeszcze narzędzia, które będzie "pomostem" pomiędzy socketem TCP a 
portem USART. W tym celu powstała aplikacja tcp2com.

Można ją uruchomić z linii komend w taki sposób:

tcp2com \\.\COM19 2001

gdzie COM19 to numer portu COM przez który komunikujemy się z płytką a 2001 to numer portu TCP.
Uruchomienie tego programu spowoduje, że wszystkie dane, które trafią na port 2001 komputera na którym go 
uruchomiliśmy zostaną przesłane do portu COM19 i odwrotnie - wszystkie dane odebrane z portu COM19 zostaną wysłane przez port 2001. 
W praktyce więc w ten sposób uruchomimy serwer gracza na komputerze, a klient gry będzie się do niego podłączał zdalnie.