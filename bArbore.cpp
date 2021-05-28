#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
#define t 500           //gradul arborelui 
ifstream in ("abce.in");
ofstream out ("abce.out");
 
struct nod
{
    int chei[2*t];    //array de chei
    nod* ptr_copii[2*t+1];  //array de copii
    int nr_chei;    //numarul de chei 
    bool frunza; // determina dca nodul e sau nu frunza
};

// declarari
nod *radacina=nullptr;

//LISTA DE FUNCTII FOLOSITE
//INSERARE
void split(nod * parinte, nod *de_divizat, int poz);
void inserare_non_full(nod* nod_curent, int val);
void inserare(int val);
//stergere
void stergere(int val);
void eliminare(nod* nod_curent, int val);
void unire (nod* parinte, int poz_ptr_copil); 
void imprumut_stg(nod * parinte, int poz_ptr_copil);
void imprumut_dr(nod * parinte, int poz_ptr_copil);
void completare(nod * parinte, int poz_ptr_copil);
void stergere_frunza(nod* nod_curent, int poz);
void stergere_nod_int(nod * nod_curent, int poz);
int find_key(nod* nod_curent, int val);
// CAUTARE
nod* Cauta(nod* nod_curent, int val);
 
int main()
{
     int nrOp; //nr de operatii (q in cerinte)
    int Op;
    int x, y;
   
    in>>nrOp;
    for(int i=0; i<nrOp; i++)
    {
        in>>Op;
        switch (Op)
        {
        case 1: {in>>x; inserare(x); break;}
        case 2: {in>>x; stergere(x); break;}
        case 3: {in>>x; 
                    if(radacina ==nullptr)
                         {
                            out<<"0\n";
                            continue;
                         }
 
                     nod* rez=Cauta(radacina, x);
                     if(rez==nullptr)
                          out<<"0\n";
                     else
                          out<<"1\n"; break;
                }
        
        }
    }
    in.close();
    out.close();
    return 0;
}



nod* Cauta(nod* nod_curent, int val)
{
    //cautare binara in nodul curent
    int* up=upper_bound(nod_curent->chei, nod_curent->chei+ nod_curent->nr_chei, val);  //pointer catre primul element >= val
    int poz_up=up-nod_curent->chei; //pozitia in vector unde e upper bound
    
    if(poz_up-1>=0 && val == nod_curent->chei[poz_up-1]) //am gasit valoarea
        return nod_curent;
 
    if (nod_curent->frunza)
        return nullptr;
    return Cauta(nod_curent->ptr_copii[poz_up], val);
}

void split(nod * parinte, nod *de_divizat, int poz) //parinte, copil de divizat care va da o cheie parintelui, pozitia cheii noi
{
    nod * fiu_dreapta= new nod;
    fiu_dreapta->frunza=de_divizat->frunza; //se mosteneste proprietatea de frunza
    fiu_dreapta->nr_chei=t-1;
 
 
    //mut ultimele t-1 chei in fiul din dreapta
    for(int i=t; i<2*t-1; ++i)
        fiu_dreapta->chei[i-t]=de_divizat->chei[i];
 
    //mut ultimii t poineri spre copii in fiul din dreapta
    if(de_divizat->frunza==0)
        for(int i=t; i<2*t; ++i)
            fiu_dreapta->ptr_copii[i-t]=de_divizat->ptr_copii[i];
 
    de_divizat->nr_chei=t-1; //devine nodul din stanga
 
    //mut pointerii catre fii de la mijloc spre dreapta ca sa fac loc
    for(int i=parinte->nr_chei; i>=poz+1; --i)
        parinte->ptr_copii[i+1]=parinte->ptr_copii[i];
 
    //fac legatura cu fiul drept
    parinte->ptr_copii[poz+1]=fiu_dreapta;
 
    //mut cheile spre dreapta ca sa fac loc
    for(int i=parinte->nr_chei-1; i>=poz; --i)
        parinte->chei[i+1]=parinte->chei[i];
 
    parinte->chei[poz]=de_divizat->chei[t-1]; //urc cheia din mijloc
 
    parinte->nr_chei++; //maresc nr de chei ale parintelui
}
 
void inserare_non_full(nod* nod_curent, int val) 
{
    if(nod_curent->frunza==true) 
    {
        //pornesc din dreapta, mut cheile mai mari cu o pozitie spre dr, si inserez valoarea la locul ei, pastrand array-ul sortat
        int i=nod_curent->nr_chei-1; 
        while (i>=0 && nod_curent->chei[i]>val)
        {
            nod_curent->chei[i+1]=nod_curent->chei[i];
            i--;
        }
        nod_curent->chei[i+1]=val;
        nod_curent->nr_chei++;
    }
    else // daca nodul curent nu e frunza
    {
        // pornesc din dr si caut fiul
        int i=nod_curent->nr_chei-1; 
        while(i>=0 && nod_curent->chei[i]>val)
            i--;
        //daca fiul gasit e plin 
        if(nod_curent->ptr_copii[i+1]->nr_chei==2*t-1)
        {
            split(nod_curent, nod_curent->ptr_copii[i+1],i+1);
            if(nod_curent->chei[i+1] < val) //daca cheia urcata < val, merg mai departe
                i++;
        }
        inserare_non_full(nod_curent->ptr_copii[i+1], val); //inserarea pe fiu
    }
}
void inserare(int val)
{
    if(radacina!=nullptr)
    {
        if(radacina->nr_chei==2*t-1) //radacina plina
        {
            nod * temp= new nod; //va deveni noua radacina
            temp->frunza=0; //0 adica false
            //atasez radacina (nodul ce urmeaza sa fie split-uit) in stanga nodului temp
            temp->ptr_copii[0]=radacina;
            temp->nr_chei=0;
 
            split(temp, radacina, 0);
 
            //radacina are 2 fii; pun cheia noua in unul dintre ei
            int nr_fiu;
            if(val <= temp->chei[0]) //valoarea mea e mai mica decat prima cheie
                nr_fiu=0;
            else
                nr_fiu=1;
            inserare_non_full(temp->ptr_copii[nr_fiu],val);
            radacina=temp; //actualizez radacina
        }
        else 
            inserare_non_full(radacina, val);
    }
    else  //daca nu avem radacina, creez una si inserez valoarea
    {
        
        nod* temp=new nod;
        temp->frunza=1;
        temp->nr_chei=1;
        temp->chei[0]=val;
 
 
        radacina=temp;
    }
}
 
void unire (nod* parinte, int poz_ptr_copil)
{
    //unesc copilul cu fratele din dreapta in nodul copil
 
    nod* copil=parinte->ptr_copii[poz_ptr_copil];
    nod* frate=parinte->ptr_copii[poz_ptr_copil+1];
 
    //parintele coboara la copil
    // mut cheile parintelui  si pointerii o pozitie spre stanga
    copil->chei[++copil->nr_chei]=parinte->chei[poz_ptr_copil];
 
    //mut cheile fratelui din dreapta in copil si pointerii spre copii
    for(int i=0; i<frate->nr_chei; ++i)
        copil->chei[i+t]= frate->chei[i];
    if(copil->frunza==0)
        for(int i=0; i<= frate->nr_chei; ++i)
            copil->ptr_copii[i+t]=frate->ptr_copii[i];
 
 
    for(int i=poz_ptr_copil+1; i < parinte->nr_chei; ++i)
        parinte->chei[i-1]=parinte->chei[i];
 
    for(int i=poz_ptr_copil+2; i<=parinte->nr_chei; ++i)
        parinte->ptr_copii[i-1] = parinte->ptr_copii[i];
 
    //actualizez nr chei
    parinte->nr_chei--;
    copil->nr_chei += frate->nr_chei +1;
 
    delete frate;
    return;
}

void imprumut_stg(nod * parinte, int poz_ptr_copil)
{
    nod * copil=parinte->ptr_copii[poz_ptr_copil];
    nod * frate=parinte->ptr_copii[poz_ptr_copil-1];
 
    //ultima cheie din frate se duca in parinte si parintele se duce in copil pe prima pozitie
 
    //mut cheile si pointerii (daca nu e frunza) din copil la dreapta cu o pozitie
    for(int i=copil->nr_chei-1; i>=0; --i)
        copil->chei[i+1]=copil->chei[i];
 
    if(copil->frunza==0)
        for(int i=copil->nr_chei;   i>=0; --i)
            copil->ptr_copii[i+1]=copil->ptr_copii[i];
 
    //mut parintele in copil
    copil->chei[0]=parinte->chei[poz_ptr_copil-1];
 
    //mut ultimul copil al fratelui la copil, daca nu e frunza
    if(copil->frunza==0)
        copil->ptr_copii[0]=frate->ptr_copii[frate->nr_chei];
 
    //mut fratele drept in parinte (ultima cheie din frate)
    parinte->chei[poz_ptr_copil-1] = frate->chei[(frate->nr_chei)-1];
 
    copil->nr_chei++;
    frate->nr_chei--;
    return;
}
 
void imprumut_dr(nod * parinte, int poz_ptr_copil)
{
    nod * copil=parinte->ptr_copii[poz_ptr_copil];
    nod * frate=parinte->ptr_copii[poz_ptr_copil+1];
 
    //cheia parintelui vine pe ultima pozitie a cheilor copilului
    copil->chei[copil->nr_chei]=parinte->chei[poz_ptr_copil];
    //mut si pointerul, daca nu e frunza
    if(copil->frunza==0)
        copil->ptr_copii[(copil->nr_chei)+1]= frate->ptr_copii[0];
 
    //mut cheia fratelui in parinte
    parinte->chei[poz_ptr_copil]=frate->chei[0];
 
    //mut cheile fratelui cu 1 la dreapta
    for(int i=1; i<frate->nr_chei; ++i)
        frate->chei[i-1]=frate->chei[i];
    //mut si pointerii fratelui un pas la dreapta, daca nu e frunza
    if(frate->frunza==0)
        for(int i=1; i<= frate->nr_chei; ++i)
            frate->ptr_copii[i-1]=frate->ptr_copii[i];
 
    copil->nr_chei++;
    frate->nr_chei--;
    return;
}
 
void completare(nod * parinte, int poz_ptr_copil)
{
    //nodul parinte si pozitia pointeruui pentru copilul ce trebuie completat
    //daca fratele din stanga exista si  are mai mult de t-1 noduri ma imprumut de la el
 
    if(poz_ptr_copil!=0 && parinte->ptr_copii[poz_ptr_copil-1]->nr_chei>t-1)
        imprumut_stg(parinte, poz_ptr_copil);
 
    //incerc cu fratele drept
    else if (poz_ptr_copil!=parinte->nr_chei && parinte->ptr_copii[poz_ptr_copil+1]->nr_chei>t-1)
        imprumut_dr(parinte, poz_ptr_copil);
 
    else  // daca nu pot face imprumut, trebuie sa cobor parintele si sa unesc fratii
        if(poz_ptr_copil == parinte->nr_chei)
            unire(parinte, poz_ptr_copil-1); //daca nodul problematic e ultimul-> unesc cu fratele din stanga
        else
            unire(parinte, poz_ptr_copil);  //altfel unesc cu fratele din dreapta
    return;
}

void stergere_frunza(nod* nod_curent, int poz)
{
    //sterg cheia de pe pozitia poz din nodul curent
    for(int i=poz+1; i<nod_curent->nr_chei; ++i)
        nod_curent->chei[i-1]=nod_curent->chei[i];
 
    nod_curent->nr_chei--;
    return;
}

int GasestePredecesor(nod * nod_curent, int poz_copil)
{
    nod* temp=nod_curent->ptr_copii[poz_copil];
 
    while(temp->frunza==0) //cat timp nu e frunza, merg pe dreapta
        temp=temp->ptr_copii[temp->nr_chei];
 
    return temp->chei[(temp->nr_chei)-1]; //cea mai din stanga cheie
}
int GasesteSuccesor(nod* nod_curent, int poz_copil)
{
    nod* temp=nod_curent->ptr_copii[poz_copil+1];
 
    while(temp->frunza==0) //cat timp nu e frunza, merg pe stanga
        temp=temp->ptr_copii[0];
 
    return temp->chei[0]; //cea mai din stanga cheie
}
 
void stergere_nod_int(nod * nod_curent, int poz)
{
    //stergere cheia de pe pozitia poz din nodul curent
    int val=nod_curent->chei[poz];
 
    //verfic daca fiul din stanga are mai mult de t-1 chei
    if(nod_curent->ptr_copii[poz]->nr_chei>t-1)
    {
        int predecesor=GasestePredecesor(nod_curent,poz);
        nod_curent->chei[poz]=predecesor;
        eliminare(nod_curent->ptr_copii[poz], predecesor); //eliminare predecesorul care se afla in subarborele cu radacina in fiul stang al nodului curent
    }
    //verific daca fiul din dreapta are mai mult de t-1 chei
    else if(nod_curent->ptr_copii[poz+1]->nr_chei>t-1)
    {
        int succesor=GasesteSuccesor(nod_curent,poz);
        nod_curent->chei[poz]=succesor;
        eliminare(nod_curent->ptr_copii[poz+1], succesor); //eliminare succesorul care se afla in subarborele cu radacina in fiul drept al nodului curent
    }
    else // daca nici unul dintre fii nu are mai mult decat numarul minim de chei trebuie sa fac merge
    {
        unire(nod_curent, poz);
        eliminare(nod_curent->ptr_copii[poz], val); //Se aplica mai departe procesul de stergerere recursiva a lui val din nodul curent
    }
    return;
}
 
int find_key(nod* nod_curent, int val)
{
    //returneaza pozitia pe care se afla valoarea sau unde se afla prima valoare cea mai mare
    int poz=0;
    while(poz<nod_curent->nr_chei && nod_curent->chei[poz]<val)
        poz++;
    return poz;
}

void eliminare(nod* nod_curent, int val)
{

    int poz=find_key(nod_curent, val);
 
    if(poz < nod_curent->nr_chei && nod_curent->chei[poz]==val)  //daca valoarea e in nod *******
    {
        if(nod_curent->frunza)
            stergere_frunza(nod_curent, poz);
        else
            stergere_nod_int(nod_curent, poz);
    }
    else //daca valoarea nu e in nod *********
    {
        if(nod_curent->frunza)   //daca nodul e frunza
        {
            //nu am ce sa sterg
            return;
        }
        else // daca nodul nu e frunza
        {
            bool ultimul; //caz particular, retin daca trebuia sa merg pe extremitatea dreapta sau nu
            if(poz==nod_curent->nr_chei) //ar veni pe ultimul copil al nodului curent
                ultimul=1;
 
            else ultimul=0;
            //cheia pe care o caut se afla in subarborele cu radacina in acest nod
            if(nod_curent->ptr_copii[poz]->nr_chei< t ) //daca nodul unde ar trebui sa se afle cheia are mai putin de t chei
                completare(nod_curent, poz);
 
            if(ultimul && poz > nod_curent->nr_chei)
                //s-a facut un merge pe ultimul copil deci numarul de chei a scazut, deci
                //poz_up > nr de chei curent, si nu egal ca inainte
                //in plus, era ultimul nod
                eliminare(nod_curent->ptr_copii[poz-1], val); //ma mut cu unul spre stanga pentru ca am ramas in aer datorita merge-ului
            else
                //nu s-a produs niciun merge
                eliminare(nod_curent->ptr_copii[poz], val);
        }
    }
    return;
}
 
void stergere(int val)
{
    if(radacina==nullptr)
    {
        //daca nu am radacina, nu am ce sa sterg
        return;
    }
 
    eliminare(radacina, val);
 
    if(radacina->nr_chei==0) 
    {
        //il fac radacina pe fiu daca exista
        nod* temp=radacina; 
 
        if(radacina->frunza==1) //daca nu exista fiu
            radacina=nullptr;
        else                     //exista 
            radacina=radacina->ptr_copii[0];
 
        delete temp;       //sterg fosta radacina
    }
    return;
}