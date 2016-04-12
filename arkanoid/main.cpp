extern "C" {
#ifdef BIT64
#include"./sdl64/include/SDL.h"
#include"./sdl64/include/SDL_main.h"
#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
#endif
}
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<cstdlib>
#include<fstream>
#include<conio.h>
#include<iostream>
#include<sstream>
#include <ctime>
#define LEFT (screen->w /11)
#define RIGHT (screen->w - screen->w / 10)
#define PREDKOSC_PALETKI 200	// [pixel/s]
#define PREDKOSC_PILKI 50	// [pixel/s]
#define SZEROKOSC_PALETKI 100
#define WYSOKOSC_PALETKI 15
#define SZEROKOSC_PILKI 25
#define WYSOKOSC_PILKI 25
#define ILOSC_BLOKOW 75
#define PREDKOSC_BONUSU 100
#define WYSOKOSC_EKRANU 480
#define SZEROKOSC_EKRANU 640
#define SZEROKOSC_BONUS1 20
#define WYSOKOSC_BONUS1 18
#define SZEROKOSC_BONUS2 20
#define WYSOKOSC_BONUS2 8


using namespace std;

enum orientation
{
	horizontal,
	vertical
};

struct Fpoint
{
	double x;
	double y;
};

class Bloki
{
public:
	int x;
	int y;
	int kolor;
	int szerokosc;
	int wysokosc;
	bool istnieje;
};

class Paletka
{
public:
	int x;
	int y;
	int szerokosc;
	int wysokosc;
	double predkosc;
};

class Pileczka
{
public:
	int x;
	int y;
	int szerokosc;
	int wysokosc;
	double predkosc;
	SDL_Surface *obrazek;
	int kierunek_pilki_x;
	int kierunek_pilki_y;
};

class Bonus
{
public:
	int x;
	int y;
	int szerokosc;
	int wysokosc;
	SDL_Surface *obrazek;

};


// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	}
}


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
}


// rysowanie linii o d³ugoœci l w pionie b¹dŸ poziomie
void DrawLine(SDL_Surface *screen, int x, int y, int l, orientation o,
              Uint32 color)
{
	for(int i = 0; i < l; i++)
	{
		DrawPixel(screen, x, y, color);
		if(o == horizontal) x++;
		else y++;
	}
}


// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor)
{
	int i, j;
	DrawLine(screen, x, y, k, vertical, outlineColor);
	DrawLine(screen, x + l, y, k, vertical, outlineColor);
	DrawLine(screen, x, y, l, horizontal, outlineColor);
	DrawLine(screen, x, y + k, l, horizontal, outlineColor);
	for(i = y + 1; i < y + k; i++)
		for(j = x + 1; j < x + l; j++)
			DrawPixel(screen, j, i, fillColor);
}


void wczytaj_mape(string **mapa,string nazwa)
{
	int x=0;
	int y=0;

	fstream plik;
	plik.open( nazwa , ios::in );
	if( plik.good() )
	{
		while( !plik.eof() )
		{
			for(x=0;x<5;x++)
			{
				for(y=0;y<15;y++)
				{
					plik >> mapa[x][y];
				}
			}
		}
		plik.close();
	}

}

void stworz_bloki(SDL_Surface *screen,string **mapa,Bloki blok[100])
{

	int wiersz=0,kolumna=0,x=20,y=80,ilosc=0,i=0;

	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);


	for(wiersz=0;wiersz<5;wiersz++)
	{
		for(kolumna=0;kolumna<15;kolumna++)
		{
			if(mapa[wiersz][kolumna]=="G") 
			{
				blok[ilosc].kolor=zielony;
				blok[ilosc].x=x;
				blok[ilosc].y=y;
				blok[ilosc].szerokosc=40;
				blok[ilosc].wysokosc=20;
				blok[ilosc].istnieje=true;
			}

			if(mapa[wiersz][kolumna]=="C") 
			{
				blok[ilosc].kolor=czerwony;
				blok[ilosc].x=x;
				blok[ilosc].y=y;
				blok[ilosc].szerokosc=40;
				blok[ilosc].wysokosc=20;
				blok[ilosc].istnieje=true;
			}

			if(mapa[wiersz][kolumna]=="N") 
			{
				blok[ilosc].kolor=niebieski;
				blok[ilosc].x=x;
				blok[ilosc].y=y;
				blok[ilosc].szerokosc=40;
				blok[ilosc].wysokosc=20;
				blok[ilosc].istnieje=true;
			}

			ilosc++;
			x=x+40;
		}
		y=y+20;
		x=20;
	}
	


}


// zmiana wspó³rzêdnej x punktu p o inc; funkcja zwraca zmodyfikowany punkt
Fpoint NextPoint(Fpoint p, double inc)
{
	Fpoint res;
	res.y = p.y;
	res.x = p.x + inc;
	return res;
}


// zmiana wspó³rzêdnej x punktu p o inc i y o inc2; funkcja zwraca zmodyfikowany punkt
Fpoint NextPoint2(Fpoint p, double inc, double inc2)
{
	Fpoint res;
	res.y = p.y + inc2;
	res.x = p.x + inc;
	return res;
}



// rysowanie zawartosci ekranu
// screen to ekran, na którym rysujemy
// charset to tablica znaków
// worldTime to czas w sekundach od rozpoczêcia programu
// tickTime to czas w sekundach od poprzedniego narysowania ekranu
void DrawScreen(SDL_Surface *screen, SDL_Surface *charset,Paletka *paletka,SDL_Surface *obwod, SDL_Surface *obwod2, SDL_Surface *serce, int kierunek_paletki, double tickTime,
	string **mapa, int *liczba_punktow,Bloki blok[100],Pileczka *pilka,bool *start,int *liczba_zyc,bool *nowa_gra,Bonus *serduszko,bool *rysuj_bonus, bool losowanie,Bonus *sz_paletka,int bonus)
{
	static Fpoint p = { screen->w/2-35, screen->h-2*screen->h/10+30}, pi = { screen->w/2, screen->h-2*screen->h/10+30}, bonus_p={ screen->w/2, screen->h-2*screen->h/10+30};
	char text[128];
	int width = screen->w;
	int height = screen->h;
	static double poruszanie = 0;
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int wiersz=0,kolumna=0,x=20,i=0;

	// czarne t³o
	SDL_FillRect(screen, NULL, czarny);


	//ramka
	DrawSurface(screen, obwod, 0, 80);
	DrawSurface(screen, obwod, 620, 80);
	DrawSurface(screen, obwod2, 0,60);

	//jezeli dosz³o do wylosowania bonusu ustal jego wspo³rzedne
	if(losowanie==true)
	{
		bonus_p.x=pilka->x;
		bonus_p.y=pilka->y;
	}

	//rysowanie bonusu-serce
	if(*rysuj_bonus==true)
	{
		bonus_p= NextPoint2(bonus_p, 0, PREDKOSC_BONUSU * tickTime);
		if(bonus==1)
		{
			serduszko->x=bonus_p.x;
			serduszko->y=bonus_p.y;
			DrawSurface(screen, serduszko->obrazek, serduszko->x, serduszko->y);
		}
		if(bonus==2)
		{
			sz_paletka->x=bonus_p.x;
			sz_paletka->y=bonus_p.y;
			DrawSurface(screen, sz_paletka->obrazek, sz_paletka->x, sz_paletka->y);
		}
	}

	// tekst informacyjny
	DrawRectangle(screen, LEFT, 4, RIGHT-LEFT, 36, czerwony, czarny);
	sprintf(text, "liczba punktow = %d", *liczba_punktow);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
	sprintf(text, "Esc - wyjscie, \032 - w lewo, \033 - w prawo, spacja-start");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

	if(p.x < LEFT-35)		// je¿eli paletka jest za daleko w lewo
		kierunek_paletki = 1;		// zmienia kierunek na "w prawo"
	if(p.x + paletka->szerokosc > RIGHT+42)	// je¿eli paletka jest za daleko w prawo
		kierunek_paletki = -1;		// zmienia kierunek na "w lewo"


	//rysowanie paletki
	p = NextPoint(p, kierunek_paletki * paletka->predkosc * tickTime);

	if(*nowa_gra==true)
	{
		p.x=paletka->x;
		p.y=paletka->y;
		*nowa_gra=false;
	}
	paletka->x=p.x;
	paletka->y=p.y;

	DrawRectangle(screen,paletka->x,paletka->y,paletka->szerokosc,paletka->wysokosc,czerwony,zielony);

	//kolizje z bokami
	if(pi.x<20)
		pilka->kierunek_pilki_x=1;
	if(pi.y<85)
		pilka->kierunek_pilki_y=1;
	if(pi.x>595)
		pilka->kierunek_pilki_x=-1;

	if(pilka->y>WYSOKOSC_EKRANU)
	{
		*liczba_zyc-=1;
		*start=0;
		pilka->predkosc=PREDKOSC_PILKI;
	}

	//¿ycia
	for (i=0;i<*liczba_zyc;i++)
	{
		DrawSurface(screen, serce, x, 440);
		x+=40;
	}
	
	//rysowanie pileczki przed startem
	if (*start==false)
	{
		pilka->x=paletka->x+(paletka->szerokosc/2-pilka->szerokosc/2);
		pilka->y=paletka->y-pilka->wysokosc;
		DrawSurface(screen,pilka->obrazek,pilka->x,pilka->y);
		pi.x=pilka->x;
		pi.y=pilka->y;
	}

	//rysowanie pileczki po starcie(pi³eczka zaczyna siê poruszaæ)
	else 
	{
		pi=NextPoint2(pi,pilka->predkosc * pilka->kierunek_pilki_x * tickTime,pilka->predkosc * pilka->kierunek_pilki_y * tickTime);
		pilka->x=pi.x;
		pilka->y=pi.y;
		DrawSurface(screen,pilka->obrazek,pilka->x,pilka->y);
		
	}

	//rysowanie bloków
	for(i=0;i<ILOSC_BLOKOW;i++)
	{
		if (blok[i].istnieje==true)
			DrawRectangle(screen,blok[i].x,blok[i].y,blok[i].szerokosc,blok[i].wysokosc,czarny,blok[i].kolor);
	}


	
}

//kolizja pileczki z blokami
void kolizja(Bloki blok[100],double tickTime,Pileczka *pilka,int *zbite_klocki,int *liczba_punktow,int *zbite_klocki_losowanie)
{
	int i=0;

	for(i=0;i<ILOSC_BLOKOW;i++)
		if((pilka->x+pilka->szerokosc+(pilka->kierunek_pilki_x *pilka->predkosc * tickTime)>=blok[i].x) && (pilka->x+(pilka->kierunek_pilki_x * pilka->predkosc * tickTime)<=(blok[i].x+blok[i].szerokosc)) && (pilka->y+pilka->wysokosc+(pilka->kierunek_pilki_y * pilka->predkosc * tickTime)>=blok[i].y) && (pilka->y+(pilka->kierunek_pilki_y * pilka->predkosc * tickTime)<=(blok[i].y+blok[i].wysokosc)) && blok[i].istnieje==true)
		{
			blok[i].istnieje=false;
			*zbite_klocki+=1;
			*liczba_punktow+=100;
			*zbite_klocki_losowanie+=1;

			//zderzenie z doln¹ œciank¹
			if( (pilka->y + (pilka->kierunek_pilki_y *pilka->predkosc * tickTime)<=blok[i].y+blok[i].wysokosc) && (pilka->y>=blok[i].y+blok[i].wysokosc) )
				pilka->kierunek_pilki_y=1;
			
			//zderzenie z górn¹ œciank¹
			if( (pilka->y + (pilka->kierunek_pilki_y *pilka->predkosc * tickTime) + pilka->wysokosc>=blok[i].y) && (pilka->y + pilka->wysokosc <= blok[i].y) )
				pilka->kierunek_pilki_y=-1;

			//zderzenie z lew¹ œciank¹
			if( (pilka->x + (pilka->kierunek_pilki_x *pilka->predkosc * tickTime) + pilka->szerokosc>=blok[i].x) && (pilka->x + pilka->szerokosc<=blok[i].x) )
				pilka->kierunek_pilki_x=-1;

			//zderzenie z praw¹ œciank¹ 
			if( (pilka->x + (pilka->kierunek_pilki_x *pilka->predkosc * tickTime) <= blok[i].x+blok[i].szerokosc) && (pilka->x >=blok[i].x+blok[i].szerokosc) )
				pilka->kierunek_pilki_x=1;
			
		}

}

//kolizje paletki z innym obiektami
void kolizja_paletki(double tickTime,Pileczka *pilka, Paletka *paletka,Bonus *serduszko,Bonus *sz_paletka,int kierunek_paletki,int *liczba_zyc,bool *rysuj_bonus,int *liczba_punktow,bool *startuj_czas,double *czas)
{
	
	//kolizja z pilka
	if ((pilka->x + pilka->szerokosc + (pilka->kierunek_pilki_x *pilka->predkosc * tickTime) >= paletka->x + (kierunek_paletki *paletka->predkosc * tickTime) + 5) && (pilka->x + (pilka->kierunek_pilki_x * pilka->predkosc * tickTime) <= (paletka->x + paletka->szerokosc + (kierunek_paletki *paletka->predkosc * tickTime) - 5)) && (pilka->y + pilka->wysokosc + (pilka->kierunek_pilki_y * pilka->predkosc * tickTime) >= paletka->y) && (pilka->y + (pilka->kierunek_pilki_y * pilka->predkosc * tickTime) <= (paletka->y + paletka->wysokosc)))
	{

		//zderzenie z górn¹ œciank¹
		if ((pilka->y + (pilka->kierunek_pilki_y *pilka->predkosc * tickTime) + pilka->wysokosc >= paletka->y) && (pilka->y + pilka->wysokosc <= paletka->y))
			pilka->kierunek_pilki_y = -1;

		//zderzenie z lew¹ œciank¹
		if ((pilka->x + (pilka->kierunek_pilki_x *pilka->predkosc * tickTime) + pilka->szerokosc >= paletka->x + 5) && (pilka->x + pilka->szerokosc <= paletka->x + 5))
			pilka->kierunek_pilki_x = -1;

		//zderzenie z praw¹ œciank¹ 
		if ((pilka->x + (pilka->kierunek_pilki_x *pilka->predkosc * tickTime) <= paletka->x + paletka->szerokosc - 5) && (pilka->x >= paletka->x + paletka->szerokosc - 5))
			pilka->kierunek_pilki_x = 1;
	}



	//kolizja z serduszkiem
	if( (serduszko->x+serduszko->szerokosc>=paletka->x+5) && (serduszko->x<=(paletka->x+paletka->szerokosc-5)) && (serduszko->y+serduszko->wysokosc+(PREDKOSC_BONUSU * tickTime)>=paletka->y) && (serduszko->y+(PREDKOSC_BONUSU * tickTime)<=(paletka->y+paletka->wysokosc)) )
	{
		*liczba_zyc+=1;
		*rysuj_bonus=false;
		*liczba_punktow+=50;
		serduszko->x=800;
		serduszko->y=800;
	}

	//kolizja z bonusem paletki(poszerzenie)
	if( (sz_paletka->x+sz_paletka->szerokosc>=paletka->x+5) && (sz_paletka->x<=(paletka->x+paletka->szerokosc-5)) && (sz_paletka->y+sz_paletka->wysokosc+(PREDKOSC_BONUSU * tickTime)>=paletka->y) && (sz_paletka->y+(PREDKOSC_BONUSU * tickTime)<=(paletka->y+paletka->wysokosc)) )
	{
		*rysuj_bonus=false;
		*liczba_punktow+=50;
		sz_paletka->x=1000;
		sz_paletka->y=1000;
		paletka->szerokosc+=20;
		*startuj_czas=true;
		czas=0;
	}
}

//obsuguje zdarzenia po koncu gry
void koniec_gry(SDL_Surface *screen, SDL_Surface *charset,bool wygrana,bool przegrana,bool *pauza)
{
	char text[128];
	if(przegrana==true)
	{
		sprintf(text, "Przegrales, aby rozpoczac nowa gre kliknij spacje lub ESC aby wyjsc");
		DrawString(screen, 60, 300, text, charset);
		*pauza=true;

	}

	if(wygrana==true)
	{
		sprintf(text, "Brawo wygrales !!");
		DrawString(screen, 240, 300, text, charset);
		sprintf(text, "Aby rozpoczac nowa gre kliknij spacje lub ESC aby wyjsc");
		DrawString(screen, 100, 330, text, charset);
		*pauza=true;

	}

}



// funkcja odczutuje plik graficzny z pliku o nazwie nazwa_pliku
// i zwraca odpowiedni¹ strukturê (zgodnie z bibliotek¹ SDL
// Funkcja tak¿e ustawia przezroczyty kolor, zgodnie z drugim argumentem
// W przypadku niepowodzenia funkcja zwraca NULL
SDL_Surface *LoadBitmap( const char *nazwa_pliku, Uint32 col ) {
	SDL_Surface *bitmap = SDL_LoadBMP(nazwa_pliku);
	if(bitmap == NULL)
	{
		SDL_Quit();
		printf("Error loading %s\n", nazwa_pliku);
		return NULL;
	}
	// ustawienie w powierzchni charset koloru przezroczystego
	// od tej chwili pixele koloru col na tej powierzchni
	// bêd¹ przezroczyste
	SDL_SetColorKey(bitmap, SDL_SRCCOLORKEY, col);
	return bitmap;
}


// Funkcja inicjalizuje tryb graficzny i zwraca wskaŸnik do "ekranu"
// Jeœli inicjalizacja siê nie powiedzie - funkcja zwróci NULL
SDL_Surface *Init() {
	SDL_Surface *screen;
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("error: %s\n", SDL_GetError());
		return NULL;
	}

	screen = SDL_SetVideoMode(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	// tryb pelnoekranowy
	//screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

	if(screen == NULL)
	{
		printf("SetVideoMode error: %s\n", SDL_GetError());
		return NULL;
	}

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);
	return screen;
}



// main
int main(int argc, char **argv)
{
	int t1,
		t2,
		quit,
		kierunek_paletki,
		liczba_punktow = 0, //liczba punktów zdobytych w grze
		liczba_zyc = 3, //liczba ¿yæ w grze 
		zbite_klocki = 0, //zlicza iloœæ klocków zbitych w grze, aby wygraæ
		los = 0, //zmienna do wylosowania wymaganej liczby zbitych klocków, aby otrzymaæ bonus 
		zbite_klocki_losowanie = 0, // zlicza iloœæ zbitych klocków potrzebnych do zdobycia bonusu
		i = 0;  // zmienna pomocnicza

	int bonus=0;//okresla jaki bonus siê wylosowa³, 1-serce,2-szeroka paletka
	double delta, czas;
	bool wygrana = false, //wygrana rozgrywka
		przegrana = false, //przegrana rozgrywka
		pauza = false, //pauza po przegranej lub wygranej grze
		nowa_gra = false, //zmienna pomagaj¹ca wykryæ now¹ gre i ustawiæ paletke wraz pi³eczk¹ na œrodku
		losowanie = true, //losuje bonus
		rysuj_bonus = false, //okreœla 
		startuj_czas = false, //startuje czasu bonusu szerokoœæ paletki
		start = false; // startuje gre i poruszanie siê pi³eczki

	SDL_Event event;
	SDL_Surface *screen;
	SDL_Surface *charset, *obwod, *obwod2, *serce, *pileczka, *serducho, *sz_paletkao;
	string **mapa;

	mapa=new string * [100];
	for (int i=0;i<100;i++)
		mapa[i]=new string[100];

	Bloki blok[100];
	Paletka* paletka= new Paletka;
	Pileczka* pilka= new Pileczka;
	Bonus *serduszko= new Bonus; // bonus serca
	Bonus *sz_paletka= new Bonus; // bonus poszerzenia paletki

	screen = Init();
	charset = LoadBitmap("cs8x8.bmp", 0x000000);
	if(charset == NULL)
		return 1;
	obwod = LoadBitmap("obwod.bmp", 0xFFFFFF); 
	if (obwod == NULL )
		return 1;
	obwod2 = LoadBitmap("obwod2.bmp", 0xFFFFFF); 
	if (obwod == NULL )
		return 1;
	serce = LoadBitmap("serce.bmp", 0xFFFFFF); 
	if (serce == NULL )
		return 1;
	pileczka = LoadBitmap("pileczka.bmp", 0xFFFFFF);
	if (pileczka == NULL )
		return 1;
	serducho = LoadBitmap("serduszko.bmp", 0xFFFFFF); 
	if (serducho == NULL )
		return 1;
	sz_paletkao = LoadBitmap("sz_paletka.bmp", 0xFFFFFF); 
	if (sz_paletkao == NULL )
		return 1;


	//wartoœci paletki
	paletka->szerokosc=SZEROKOSC_PALETKI;
	paletka->wysokosc=WYSOKOSC_PALETKI;
	paletka->predkosc=PREDKOSC_PALETKI;

	//wartoœci pileczki
	pilka->obrazek=pileczka;
	pilka->szerokosc=SZEROKOSC_PILKI;
	pilka->wysokosc=WYSOKOSC_PILKI;
	pilka->kierunek_pilki_x=1;
	pilka->kierunek_pilki_y=-1;
	pilka->predkosc=PREDKOSC_PILKI;

	//wartoœci bonusu serca;
	serduszko->obrazek=serducho;
	serduszko->szerokosc=SZEROKOSC_BONUS1;
	serduszko->wysokosc=WYSOKOSC_BONUS1;
	serduszko->x=800;
	serduszko->y=800;

	//wartoœci bonusu poszerzenia paletki
	sz_paletka->obrazek=sz_paletkao;
	sz_paletka->szerokosc=SZEROKOSC_BONUS2;
	sz_paletka->wysokosc=WYSOKOSC_BONUS2;
	sz_paletka->x=1000;
	sz_paletka->y=1000;

	wczytaj_mape(mapa,"mapa.txt");
	stworz_bloki(screen,mapa,blok);

	quit = 0;
	kierunek_paletki = 0; // kierunek paletki (+1 prawo, -1 lewo, 0 brak)
	czas=0;
	t1 = SDL_GetTicks();
	while(!quit)
	{
		//puaza je¿eli koniec gry
		if(pauza==false)
		{
			t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna³ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			//zwiekszanie predkosci pileczki
			if(pilka->predkosc<175 && start==1)
				pilka->predkosc+=0.1;


			//losowanie wartoœci zbitych klocków, po których dostaniemy bonus
			if(losowanie==true)
			{
				srand( time( NULL ) );
				los=(rand()%5)+6;
				bonus=(rand()%2)+1;
				losowanie=false;
			}


			// je¿eli wylosowana liczba równa siê zbitym klockom wytwórz bonus i wylosuj nowy
			if (los == zbite_klocki_losowanie)
			{
				losowanie = true;
				rysuj_bonus = true;
				zbite_klocki_losowanie = 0;
			}


			//mierzymy czas dzia³ania bonusu poszerzania paletki
			if(startuj_czas==true)
				czas+=delta;

			//Po 20 sekundach paletka wraca do swojego rozmiaru pierwotnego
			if(czas>=20)
			{
				paletka->szerokosc=SZEROKOSC_PALETKI;
				startuj_czas=false;
				czas=0;
			}

			kolizja(blok,delta,pilka,&zbite_klocki,&liczba_punktow,&zbite_klocki_losowanie);
			kolizja_paletki(delta,pilka,paletka,serduszko,sz_paletka,kierunek_paletki,&liczba_zyc,&rysuj_bonus,&liczba_punktow,&startuj_czas,&czas);
			DrawScreen(screen, charset, paletka, obwod, obwod2, serce, kierunek_paletki, delta,mapa,&liczba_punktow,blok,pilka,&start,&liczba_zyc,&nowa_gra,serduszko,&rysuj_bonus,losowanie,sz_paletka,bonus);

			if(liczba_zyc==0)
			{
				przegrana=true;
				zbite_klocki=0;
				liczba_zyc=3;
				for(i=0;i<ILOSC_BLOKOW;i++)
					blok[i].istnieje=true;
			}

			if(zbite_klocki==ILOSC_BLOKOW)
			{
				wygrana=true;
				liczba_zyc=3;
				zbite_klocki=0;
				for(i=0;i<ILOSC_BLOKOW;i++)
					blok[i].istnieje=true;
			}

			koniec_gry(screen, charset,wygrana,przegrana,&pauza);

			// naniesienie wyniku rysowania na rzeczywisty ekran
			SDL_Flip(screen);

			// obs³uga zdarzeñ (o ile jakieœ zasz³y)
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_KEYDOWN:
						if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
						else if(event.key.keysym.sym == SDLK_LEFT) kierunek_paletki=-1;
						else if(event.key.keysym.sym == SDLK_RIGHT) kierunek_paletki=1;
						else if(event.key.keysym.sym == SDLK_SPACE) start=true;
						break;
					case SDL_KEYUP:
						kierunek_paletki=0;
						break;
					case SDL_QUIT:
						quit = 1;
						break;
				}
			}
		}
		else
		{
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if(event.key.keysym.sym == SDLK_SPACE) //wznowienie rozgrywki
					{	
						start=false;
						pauza=false;
						przegrana=false;
						wygrana=false;
						paletka->x=screen->w/2-35;
						paletka->y=screen->h-2*screen->h/10+30;
						pilka->predkosc=PREDKOSC_PILKI;
						nowa_gra=true;
						liczba_punktow=0;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				}
		}




		}
	}

	// zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(obwod);
	SDL_FreeSurface(obwod2);
	SDL_FreeSurface(serce);
	SDL_FreeSurface(serducho);
	SDL_FreeSurface(pileczka);
	SDL_FreeSurface(sz_paletkao);

	SDL_Quit();
	return 0;
}
