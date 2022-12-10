#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // dla fork
#include <sys/wait.h> // dla wait

int main(int argc, char **argv){
    if(argc != 2){ // nazwa (która jest zawsze pierwszym argumentem programu) + jeden argument
        fprintf(stderr, "Nieprawidłowa ilość argumentów\n"); // błąd na strumień stderr
        return 1;
    }

    char *endptr;
    int argument = strtol(argv[1], &endptr, 10); // jeśli argument1 (argv[1]) nie jest liczbą to do wskaźnika endptr zostanie przekazane miejsce, w którym pojawił się znak argumentu inny niż cyfra
    // w przeciwnym wypadku endptr = koniec tablicy znaków (argv[1] to tablica znaków) czyli '\0'
    
    if(*endptr != '\0'){ // jeśli funkcja strtol napotkała problem przy próbie konwersji
        fprintf(stderr, "Podan argument nie jest liczbą\n");
        return 2;
    }
    
    if(argument < 1 || argument > 13){
        fprintf(stderr, "Argument nie jest liczbą z przedziału <1,13>\n");
        return 3;
    }

    if(argument == 1 || argument == 2){
        return 1; // wynika z zasady działania ciągu fibonacciego
    }else{
        // utworzenie procesu dziecka
        int pid1 = fork(); // pid1 = -1, gdy nie udało się stworzyć procesu, pid1 = 0, jeśli jesteśmy dzieckiem, pid1 > 0 (faktyczny pid uruchomionego dziecka) który zostaje do dyspozycji rodzica

        if(pid1 == 0) // jeśli jesteśmy dzieckiem 1
        {
            // Zamiana argumentu -1 (który jest liczbą ponieważ wcześniej trzeba było to zrobić dla sprawdzenia warunków) na char (wystarczy tablica dwóch charów bo z treści zadania wynika
            // że maksymalna liczba jaką można podać to 13) ponieważ argument dla execl musi byc w formie ttablicy znaków
            char str[2];
            sprintf(str, "%d", argument - 1);

            // Zastąpienie kodu dziecka kodem programu, przez co proces dziecka uruchomi program od początku a nie będzie przechodził do kolejnych linijek
            // Pierwszy argument funkcji execl to program który ma zostać uruchomiony (w tym przypadku ten sam), drugi to pierwszy argument programu(w c i cpp pierwszy argument to zawsze nazwa programu)
            // Trzeci argument to liczba zmniejszona o 1 (wymóg zadania)
            // NULL oznacza koniec argumentów, które mają zostać przekazane do procesu dziecka (execl - argumenty w formie listy, execv - argumenty w jednej tablicy charów)
            //execl("./a.out", "./a.out", str, NULL);
            // w tym przypadku uruchamiamy ten sam program więc jako jego nazwe wpisujemy argument 1 aktualnego procesu wtedy nie ma znaczeniu jak przy kompliacji nazwiemy program
            execl(argv[0], argv[0], str, NULL);

        }else{ // jeśli nie jesteśmy dzieckiem 1

            int pid2 = fork(); // to tworzymy dziecko 2

            if(pid2 == 0){ // jeśli jesteśmy dzieckiem 2 to wykonujemy to samo co wykonywaliśmy dla dziecka 1, jedynie z argumentem zmniejszonym o 2 nie o 1, jeśli nie jesteśmy dzieckiem 2
                            // to znaczy że jesteśmy jego rodzicem i nie robimy w tym miejscu nic 
                char str[2];
                sprintf(str, "%d", argument - 2);
                //execl("./a.out", "./a.out", str, NULL);
                execl(argv[0], argv[0], str, NULL);
            }
        }
        
        // to dzieje się po przejściu do końca rekurencyjnych wywołań kolejnych procesów (dzieci) czyli w momencie w którym ostatnie dziecko wywoła proces który nie utworzy już kolejnego
        // ponieważ jego argumentem będzie 1 lub 2 (z jednej instrukcji warunkowej wynika, że program w takim przypadku zakończy się z kodem 1)

        // dziecko 2 uruchomi sie na koncu wiec najpierw czekamy az ono sie zakonczy a potem dopiero na dziecko 1
        int status_dziecka_2, dziecko_2 = wait(&status_dziecka_2); // czekamy na zakończenie się procesu dziecka nr 2, bierzemy pid dziecka 2 oraz jego status wyjścia
        int status_dziecka_1, dziecko_1 = wait(&status_dziecka_1); // czekamy na zakończenie się procesu dziecka nr 1, bierzemy pid dziecka 1oraz jego status wyjścia

        // funkcją WEXITSTATUS bierzemy kod wyjścia / powrotu z całego statusu wyjścia
        int kod_powrotu_dziecka_1 = WEXITSTATUS(status_dziecka_1);
        int kod_powrotu_dziecka_2 = WEXITSTATUS(status_dziecka_2);

        printf("%d  %d   %d  %d\n", getpid(), dziecko_1, argument - 1, kod_powrotu_dziecka_1);
        printf("%d  %d   %d  %d\n", getpid(), dziecko_2, argument - 2, kod_powrotu_dziecka_2);

        // jako ostatni argument suma kodów powrotów dzieci aktualnego procesu, czyli rodzica
        printf("%d            %d\n\n", getpid(), kod_powrotu_dziecka_1 + kod_powrotu_dziecka_2);

        return kod_powrotu_dziecka_1 + kod_powrotu_dziecka_2; // zwracamy dla kolejnych printów, w tym przypadku gdy bedziemy sie cofac na góre drabinki
        // wywołań po dojściu na sam dół wywołań dzieci czyli aż któreś będzie równe 1 lub 2 tym samym zwracając kod powrotu 1 (return 1)
    }
}