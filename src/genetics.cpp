/**
 * @file genetics.cpp
 * hledani reseni pomoci genetickeho algoritmu;
 * populace je tvorena retezci, kde kazdy koduje urcity obsah hraci plochy;
 * retezce se reprodukuji, krizi a mutuji
 */

#include <stdlib.h>
#include <stdio.h>

#include "genetics.h"
#include "area.h"

using namespace Area;
using namespace Cards;


int Genetics::position_bits;
int Genetics::rotation_bits;
int Genetics::symbol_bits;
int Genetics::string_length;
int Genetics::population_size = MAX_POPULATION_SIZE;
double Genetics::best_quality;
cards_count_t Genetics::orig_cards_count;
cards_count_t Genetics::cards_count;
double Genetics::card_quality[MAX_Y][MAX_X];
double Genetics::origin_importance = 0.75;
int Genetics::crossover_likehood = 400;
int Genetics::mutation_likehood = 3;
area_t Genetics::orig_area;


void Genetics::copy_string(t_string string1, t_string string2)
{
  // kopirujeme po bitech
  for (int bit = 0; bit < string_length; bit++)
    string2[bit] = string1[bit];
}


void Genetics::area_decode_symbols(t_string string)
{
  // kolik bitu jsme zatim prosli - at nemusime vzdy index v poli presne pocitat
  int before_bits = 0;

  // pro kazdy radek plochy
  for (int y = 1; y <= area_max_y; y++)
    // pro kazdy sloupec plochy
    for (int x = 1; x <= area_max_x; x++)
      // pro kazdou stranu
      for (int side = 0; side < 4; side++)
      {
        // musime vynulovat hodnotu symbolu
        area[y][x][side] = 0;
        // budeme pricitat odpovidajici hodnotu k symbolu
        // (a prirozene zvysovat rad prozatimni hodnoty symbolu)
        for (int bit = 0; bit < symbol_bits; bit++)
          area[y][x][side] = area[y][x][side] * 2 + string[before_bits + bit];
        // prepocteme hodnotu symbolu na platnou hodnotu
        area[y][x][side] %= area_max_symbols + 1;
        // pricteme prosle bity
        before_bits += symbol_bits;
      }
}


void Genetics::area_decode_positions(t_string string)
{
  // kolik bitu jsme zatim prosli - at nemusime vzdy index v poli presne pocitat
  int before_bits = 0;
  
  // pro kazdy radek plochy
  for (int y = 1; y <= area_max_y; y++)
    // pro kazdy sloupec plochy
    for (int x = 1; x <= area_max_x; x++)
    {
      // puvodni pozice (suma 1D, rozklad x a y) a rotace pro kopirovani
      int orig_pos = 0, orig_x, orig_y, rot = 0;

      // dekodujeme hodnotu puvodni pozice
      for (int bit = 0; bit < position_bits; bit++)
        orig_pos = orig_pos * 2 + string[before_bits + bit];
      // pricteme prosle bity
      before_bits += position_bits;
      
      // dekodujeme hodnotu rotace pro kopirovani
      for (int bit = 0; bit < rotation_bits; bit++)
        rot = rot * 2 + string[before_bits + bit];
      before_bits += rotation_bits;
      
      // prepocteme pozici z 1D na 2D
      orig_y = orig_pos / area_max_x + 1;
      orig_x = orig_pos % area_max_x + 1;
      orig_y = (orig_y - 1) % area_max_y + 1; // korekce na pripustnou hodnotu
      rot %= 4;

      // zkopirujeme karticku
      card_copy(orig_area[orig_y][orig_x], area[y][x], rot);
    }
}


/**
 * dekoduje dany retezec na aktualni hraci plochu
 * @param string retezec obsahujici stav plochy
 */
void (*Genetics::area_decode) (t_string string) = &area_decode_symbols;


double Genetics::evaluate()
{
  // spocita karticky pro kazdou moznou kombinaci vzoru
  count_cards_safe(cards_count);

  double quality = 0; // dosavadni kvalita (budeme pricitat)

  // maximalni hodnota bodu pro jednu karticku
  double one_point = (double)1 / area_max_x / area_max_y;

  // pro kazdou karticku
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      // jsou vsechny symboly dane karticky v poradku?
      bool ok = true;
      for (int side = 0; side < 4; side++)
        if (area[y][x][side] > area_max_symbols)
          ok = false;
      if (ok) // ano, muzeme hodnotit standardnim zpusobem
      {
        // originalni pocet karticek s danym vzorem
        int orig_count = orig_cards_count[area[y][x][NORTH]]
          [area[y][x][EAST]][area[y][x][SOUTH]][area[y][x][WEST]];

        // aktualni pocet karticek s danym vzorem
        int count = cards_count[area[y][x][NORTH]][area[y][x][EAST]]
          [area[y][x][SOUTH]][area[y][x][WEST]];

        // kvalita karticky podle poctu:
        // je-li dany typ karticky v originale obsazen vicekrat, je kvalita
        // karticky maximalni; neni-li dany typ karticky obsazen vubec,
        // je kvalita karticky nulova; jinak je kvalita karticky rovna
        // podilu originalniho poctu a aktualniho poctu
        double origin_quality = (orig_count >= count) ? 1 :
          (orig_count == 0) ? 0 : (double)orig_count / count;

        // kvalita podle vyhovovani okoli
        double neighbour_quality = 0;
        neighbour_quality +=
          (area[y][x][NORTH] == area[y - 1][x][SOUTH]) ? 1 : 0;
        neighbour_quality +=
          (area[y][x][EAST] == area[y][x + 1][WEST]) ? 1 : 0;
        neighbour_quality +=
          (area[y][x][SOUTH] == area[y + 1][x][NORTH]) ? 1 : 0;
        neighbour_quality +=
          (area[y][x][WEST] == area[y][x - 1][EAST]) ? 1 : 0;
        neighbour_quality /= 4;

        // celkova kvalita karticky
        card_quality[y][x] = origin_quality * origin_importance
          + neighbour_quality * (1 - origin_importance);

        // celkova kvalita populace
        quality += card_quality[y][x] * one_point;
      }
      else // v pripade, ze karticka ma nektery ze symbolu neplatny,
        card_quality[y][x] = 0; // je jeji kvalita 0
    }

  // prace se zafixovanymi kartickami - v pripade nedodrzeni jejich pozic
  // dojde k penalizaci
  for (int fixed_nr = 0; fixed_nr < MAX_FIXED; fixed_nr++)
    if (!cards_equal(area_fixed_card[fixed_nr], area[area_fixed_y[fixed_nr]]
        [area_fixed_x[fixed_nr]]))
    {
      card_quality[area_fixed_y[fixed_nr]][area_fixed_x[fixed_nr]] = 0;
      quality /= 2;
    }

  return quality; // vrat kvalitu cele populace (hraci plochy)
}


double Genetics::find_best_string(t_population population, int *string_pos)
{
  double max, actual; // dosud maximalni kvalita a aktualni kvalita
  area_decode(population[0]); // dekoduj nulty retezec populace na hraci plochu
  max = evaluate(); // ohodnot ji => dosud maximalni kvalita
  //write_string(-1, population[0], max);
  *string_pos = 0;  // pozice nejlepsiho retezce

  // pro kazdy zbyvajici retezec populace
  for (int n_string = 1; n_string < population_size; n_string++)
  {
    area_decode(population[n_string]);
    actual = evaluate();
    //write_string(-1, population[n_string], actual);
    if (actual > max) // je kvalita daneho retezce vetsi nez zatim maximalni?
    {
      max = actual;
      *string_pos = n_string;
    }
  }
  
  return max; // vrat maximalni kvalitu
}


int Genetics::bin_search(double value, double roulette[])
{
  // krajni a prostredni bod prohledavane oblasti
  int left = 1, right = population_size, mid;

  if (value <= roulette[0]) // krajni pripad
    return 0;

  // dokud je kde prohledavat (= dokud nebyla nalezena hodnota presne)
  while (right - left > 1)
  {
    mid = (right + left) / 2; // prostredni bod
    if (roulette[mid] >= value)
      right = mid; // hodnota je nalevo
    else
      left = mid; // hodnota je napravo
  }

  return right; // umisteni dane hodnoty na rulete
}


void Genetics::uprise(t_population population)
{
  // pro kazdy retezec populace
  for (int n_string = 0; n_string < population_size; n_string++)
    // pro kazdy bit retezce
    for (int n_bit = 0; n_bit < string_length; n_bit++)
    {
      // nahodna hodnota (0/1)
      if (rand() % 2)
        population[n_string][n_bit] = true;
      else
        population[n_string][n_bit] = false;
    }
}


void Genetics::reproduce(t_population old_gen, t_population new_gen)
{
  double roulette[MAX_POPULATION_SIZE]; // ruleta
  double sum = 0; // soucet vsech kvalit
  // pro kazdy retezec v populaci:
  for (int n_string = 0; n_string < population_size; n_string++)
  {
    area_decode(old_gen[n_string]);  // dekoduj retezec na hraci plochu
    // ohodnot dany retezec a pricti k sume
    sum += roulette[n_string] = evaluate();
  }

  if (sum == 0)
    sum = 1; // pro krajni pripad

  // prevod hodnot rulety do intervalu <0, 1>
  roulette[0] /= sum;
  for (int n_string = 0; n_string < population_size - 1; n_string++)
    roulette[n_string] = roulette[n_string - 1] + roulette[n_string] / sum;
  roulette[population_size - 1] = 1; // 1 - pro jistotu

  // do nove generace pujde stejny pocet retezcu
  for (int n_string = 0; n_string < population_size - 1; n_string++)
  {
    double rnd = (double)(rand() % 1001) / 1000.0; // nahodne cislo na rulete
    // kopie retezce, ktery je na danem miste rulety, do nove generace
    copy_string(old_gen[bin_search(rnd, roulette)], new_gen[n_string]);
  }
}


void Genetics::crossover_strings_halfs(t_string first, t_string second)
{
  t_string tmp;
  int border = rand() % (string_length - 1); // delici misto v retezci
  // presuny prvni "poloviny": (druhe se nemusime dotykat)
  for (int bit = 0; bit <= border; bit++)
    tmp[bit] = first[bit];
  for (int bit = 0; bit <= border; bit++)
    first[bit] = second[bit];
  for (int bit = 0; bit <= border; bit++)
    second[bit] = tmp[bit];
}


void Genetics::crossover_strings_bits(t_string first, t_string second)
{
  // pro kazdy bit
  for (int bit = 0; bit < string_length; bit++)
  {
    // podle rovnomerne nahody prohodime dane bity retezcu nebo ne
    if (rand() % 2)
    {
      bool tmp = first[bit];
      first[bit] = second[bit];
      second[bit] = tmp;
    }
  }
}


/**
 * zkrizi dva retezce
 * @param first prvni retezec
 * @param second druhy retezec
 */
void (*Genetics::crossover_strings) (t_string first, t_string second)
  = &crossover_strings_halfs;


void Genetics::crossover(t_population old_gen, t_population new_gen)
{
  int remaining = population_size; // zbyvajicich retezcu
  t_string first, second; // pomocne retezce

  // pro polovinu retezcu v populaci (krizi se vzdy dva do dvou)
  for (int n_string = 0; n_string < population_size / 2; n_string++)
  {
    int choosen = rand() % remaining; // vybereme retezec ze zbyvajicich
    copy_string(old_gen[choosen], first); // zkopirujeme do pomocneho first
    // presunuti posledniho dosud nepouziteho retezce
    copy_string(old_gen[remaining], old_gen[choosen]);
    remaining--;  // zbyva jiz o jeden retezec mene
    // to same pro druhy vybrany retezec:
    choosen = rand() % remaining;
    copy_string(old_gen[choosen], second);
    copy_string(old_gen[remaining], old_gen[choosen]);
    remaining--;

    // budeme krizit nebo nechame projit jen tak?
    if (rand() % 1000 <= crossover_likehood)
    {
      crossover_strings(first, second);
    }

    // kopie pomocnych retezcu do nove generace populace
    copy_string(first, new_gen[2 * n_string]);
    copy_string(second, new_gen[2 * n_string + 1]);
  }

  // je lichy pocet retezcu v populaci? (zbyl jeden?)
  if (population_size / 2 == 1)
    copy_string(old_gen[0], new_gen[population_size]); // presuneme i jej
}


void Genetics::mutate(t_population population)
{
  // pro vsechny retezce v populaci
  for (int n_string = 0; n_string < population_size; n_string++)
    // pro kazdy bit retezce
    for (int bit = 0; bit < string_length; bit++)
      // budeme mutovat?
      if (rand() % 1000 <= mutation_likehood)
        population[n_string][bit] = !population[n_string][bit];
}


void Genetics::write_string(long generation_count, t_string string,
  double quality)
{
  printf("*****************\n%ld. generace:\n", generation_count);
  printf("Kvalita: %f\n", quality);
  printf("String: ");
  for (int bit = 0; bit < string_length; bit++)
    printf("%s", string[bit] ? "1" : "0");
  printf("\n");
  area_decode(string);
  area_write();
  /*
  for (int bit = 0; bit < string_length; bit++)
    printf("%s", string[bit] ? "1" : "0");
  printf("\n");
  */
}


void Genetics::genetics()
{
  printf("genetics...\n");
  area_load("area.area"); // nacte plochu
  // spocita karticky pro kazdou moznou kombinaci vzoru
  count_cards(orig_cards_count);
  
  // spocte delku retezce (jedince v populaci)
  if (area_decode == &area_decode_positions) // kodujeme retezce podle pozic?
  {
    for (position_bits = 1; // kolik bitu nam staci na zakodovani pozice?
      // dokud je/jsou bity nejvetsi pozice mimo dosah kodovani,
      (area_max_x * area_max_y - 1) >> position_bits != 0;
      // zvetsi kodovani
      position_bits++);
    rotation_bits = 2;
    string_length = (position_bits + rotation_bits) * area_max_x * area_max_y;
  }
  // kodujeme retezce podle symbolu?
  else if (area_decode == &area_decode_symbols)
  {
    for (symbol_bits = 1; // kolik bitu nam staci na zakodovani symbolu?
      // dokud je/jsou bity nejvetsiho symbolu mimo dosah kodovani,
      area_max_symbols >> symbol_bits != 0;
      // zvetsi kodovani
      symbol_bits++);
    string_length = 4 * symbol_bits * area_max_x * area_max_y;
  }
  
  // pro praci s pozicemi:
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      card_copy(area[y][x], orig_area[y][x]);
  
  t_population population, new_population; // populace a nasledujici populace

  uprise(population); // nahodne vytvori prvni (nultou) generaci populace
  int string_pos;  // cislo nejlepsiho retezce v populaci
  // najde nejlepsi retezec a urci jeho kvalitu
  best_quality = find_best_string(population, &string_pos);
  
  //getc(stdin);
  //printf("--------------------------------------------------------\n");
  
  long generation_count = 0; // cislo generace

  // vypise nejlepsi retezec
  write_string(generation_count, population[string_pos], best_quality);
  // ulozeni docasne nejlepsiho vysledku
  area_save("area_genetics_temp.area");

  while (best_quality < 1.0) // dokud jsme nenasli optimalni retezec
  {
    reproduce(population, new_population); // reprodukce populace
    crossover(new_population, population); // krizeni populace
    mutate(population); // mutace populace
    generation_count++; // dalsi cislo generace

    // nenalezli jsme lepsi retezec?
    double quality = find_best_string(population, &string_pos);
    if (quality > best_quality)
    {
      best_quality = quality;
      write_string(generation_count, population[string_pos], best_quality);
      // ulozeni docasne nejlepsiho vysledku
      area_save("area_genetics_temp.area");
    }
    
    //getc(stdin);
    //printf("--------------------------------------------------------\n");
  }

  area_save("area_genetics_result.area");  // ulozeni vysledku

  printf("OK\n");
}
