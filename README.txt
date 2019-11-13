Implementując bazę danych, przetworzyłem wektor transakcji przekazywany w funkcji 
'setTransactions' na kolekcję kont ('AccountTransactions'), ponieważ wszystkie 
funkcje dostępowe interfejsu 'Database' wymagają podania numeru konta. 

Dzięki użyciu std::unordered_map dodanie nowego elementu do kolekcji jak i średni 
dostęp do szukanego elementu uzyskuje się w czasie stałym (O(1)). Ponieważ funkcje 
'findTransactions' jak i 'findTransaction' z założeń odrzucają duplikaty, to już 
przy wczytywaniu wszystkich transakcji ('setTransactions') odrzucane są wszystkie 
kolejne transakcje z tymi samymi numerami (txNo). 

Po stworzeniu kolekcji kont transakcji i wczytaniu do nich danych, dla każdego 
konta wektor transakcji jest od razu sortowany rosnąco po numerze transakcji (txNo), 
oraz obliczana jest średnia wartość wszystkich transakcji. Akcje te wykonywane są w 
tym miejscu, ponieważ funkcja 'setTransactions' jest jedyną funkcją ładującą wszystkie 
transakcje do bazy, więc nie ma możliwości aby w trakcie wykonywania operacji na 
aktualnej bazie dane zostały zmienione. 

Dzięki wykonaniu tych obliczeń zwiększana jest złożoność funkcji ładującej dane do bazy
('setTransactions'), ale za to znacznie zmniejszona zostaje złożoność funkcji 
pobierających dane, jak 'calculateAverageAmount' oraz 'findTransactions' (O(1)). 
Ponadto posortowana tablica transakcji pozwala wykorzystać przeszukiwanie binarne 
(std::lower_bound) w funkcji 'findTransaction', dzięki czemu jej złożoność wynosi O(log(n)).
