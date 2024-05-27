//
// Created by Maciej Szymański on 27.05.2024.
//

#include <stdio.h>
#include <unistd.h>

#define g 1.6

double get_optimal_ignition_height(double* H_0, double* V_0, double a) {
    return ((*V_0) * (*V_0) + 2 * g * (*H_0)) / (2 * (a + g)); // wzór wywiedziony z prawa zachowania energii (w tym przypadku z czystym sumieniem możemy założyć brak oporów)
}

int main(void) {
    double m; // masa lądownika [kg]
    double H_0; // wysokość lądownika [m]
    double V_0; // prędkość lądownika [m/s] (zwrot w dół)
    double F_c; // siła ciągu silnika [N]
    double a; // przyspieszenie wypadkowe [m/s^2] (zwrot w dół)
    bool ENGINE_STATE = false; // stan silnika, domyślnie wyłączony;
    int counter = 0; // licznik wykorzystywany do ograniczenia spamu w konsoli
    const double T_STEP = 0.00001; // zmiana czasu pomiędzy kolejnymi etapami symulacji [s]
    const int T_WAIT = (int)(T_STEP * 1000000); // zmiana czasu między kolejnymi etapami symulacji [μs]

    printf("Podaj m, H_0, V_0, F_c:\nNp.: 2000 600 10 4500\n");
    scanf("%lf %lf %lf %lf", &m, &H_0, &V_0, &F_c);

    const double LANDER_a = F_c / m; // przyspieszenie pochodzące z silnika [m/s^2]
    const double H_OPTIMAL = get_optimal_ignition_height(&H_0, &V_0, (LANDER_a - g)); // optymalna wysokość włączenia silnika [m]

    while(H_0 > 0){
        if(H_0 <= H_OPTIMAL){
            ENGINE_STATE = true;
            a = g - LANDER_a;
        }
        else{
            a = g;
        }

        H_0 -= (V_0 * T_STEP + (0.5 * a * T_STEP * T_STEP)); // height update
        /*
         * Szacowanie rzędów wielkości w celu możliwie najdokładniejszego wykorzystania typu double (zazwyczaj do 15 cyfr znaczących bez utraty precyzji) -> szacowanie minimalnego rzędu wartości T_STEP
         * 1. Zadanie mówi o ostatniej fazie lądowania, więc zakładamy, że początkowa wysokość będzie mniejsza niż 1000m (3 cyfry znaczące na lewo od części ułamkowej)
         * 2. Zakładamy, że pilot nie może doświadczyć przeciążania większego niż 10g (ziemskich),
         * więc przyspieszenie będzie zawsze mniejsze niż 100m/s^2 (2 cyfry znaczące na lewo od części ułamkowej)
         * ~ chociaż przy 10g raczej nie zdołałby on kliknąć już guzika, aby wyłączyć silnik ~
         * 3. Z powyższych ograniczeń możemy obliczyć, że prędkość nie może przekroczyć ok. 450m/s (3 cyfry znaczące na lewo od części ułamkowej)
         */
        V_0 += (a * T_STEP); // velocity update

        counter++;
        if(counter == 20000){ // żeby nie było za dużego spamu w konsoli
            printf("H=%.2lfm, V=%.2lfm/s, %s\n", H_0, V_0, ENGINE_STATE ? "ON" : "OFF");
            counter = 0;
        }

        usleep(T_WAIT); // czas rzeczywisty działa tutaj tylko w teorii ze względu na bardzo małą wartość T_STEP
    }

    printf("----------------------\nVELOCITY AT TOUCHDOWN: %lf\nLANDING STATUS: %s\n\n", V_0, V_0 <= 1.0 ? "SUCCESS" : "FAILURE");

    getchar(); // zbierze '\n' z bufora po wejściu
    printf("Press ENTER to exit\n");
    getchar();

    return 0;
}