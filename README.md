![VUT FIT logo](https://wis.fit.vutbr.cz/images/fitnewb.png)

# Projekt č. 2 - Práce s datovými strukturami
**Předmět:** IZP - Základy programování

**Autor:** Denis Karev (xkarev00)

**Ten projekt nemá být použit pro jiné než výukové účely.**

## Popis 
Cílem projektu je vytvořit program, který bude implementovat základní operace tabulkových procesorů. 
Vstupem programu budou textová data tabulky, zadání operací bude prostřednictvím argumentů příkazové řádky a výsledek zpracování tabulky bude program ukládat do zadaného souboru.

## Příkazy ##

### Příkazy pro změnu výběru ###

* `[R,C]` - výběr buňky na řádku R a sloupci C.
* `[R,_]` - výběr celého řádku R.
* `[_,C]` - výběr celého sloupce C.
* `[R1,C1,R2,C2]` - výběr okna, tj. všech buněk na řádku R a sloupci C, pro které platí R1 <= R <= R2, C1 <= C <= C2. Pokud namísto čísla R2 resp. C2 bude pomlčka, nahrazuje tak maximální řádek resp. sloupec v tabulce.
* `[_,_]` - výběr celé tabulky.
* `[min]` - v již existujícím výběru buněk najde buňku s minimální numerickou hodnotou a výběr nastaví na ni.
* `[max]` - obdobně jako předchozí příkaz, ale najde buňku s maximální hodnotou.
* `[find STR]` - v již existujícím výběru buněk vybere první buňku, jejíž hodnota obsahuje podřetězec STR.
* `[_]` - obnoví výběr z dočasné proměnné.
