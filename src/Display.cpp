/*
Definition of the specific methods for the Display Class.
*/


#include "Display.h"
#include <Fonts/FreeMonoBold12pt7b.h>
#include "Screen_Config.h"
#include "Bitmaps.h"
#include "Timer.h"

    //Constructor - Call the constructor of parent class for my specific Display
 Display::Display() : GxEPD2_BW<GxEPD2_150_BN, MAX_HEIGHT(GxEPD2_150_BN)>
 #if (BOARD == NANO_EVERY)
   (GxEPD2_150_BN(/*CS=10*/ 10, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7)), 
 #elif (BOARD == UNO_R3)
   (GxEPD2_150_BN(/*CS=10*/ 10, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7)), 
 #endif

 m_lastTemp(0), 
 m_LastGaugeState(0)
 {       
 }

 Display::~Display()
 {
 }

 void Display::begin(uint32_t _bitrate)
{
   Display::init(_bitrate);
}

 void Display::setUpHud()
 {

    Display::setFullWindow();
    Display::setRotation(1);                  // Set orientation. Goes from 0, 1, 2 or 3

    Display::setTextWrap(false); 
    Display::setTextColor(GxEPD_BLACK);

    Display::firstPage();
    do
    {
         //UI Element : Speparator segments.
      Display::drawFastHLine(0, 100, 200, GxEPD_BLACK);
      Display::drawFastVLine(100, 100, -100, GxEPD_BLACK);
       
         //Temp indication     
      Display::drawBitmap(2, 2, epd_bitmap_Logo_Temp_35px, 35, 35, GxEPD_BLACK);

       // Fuel gauge Background
      Display::drawRect(1, 150, 198, 50, GxEPD_BLACK);      // Gauge frame
      Display::drawFastVLine(100, 150, -10, GxEPD_BLACK);   // Middle indication
      Display::drawBitmap(2, 105, epd_bitmap_LOGO_GAS, 36, 36, GxEPD_BLACK);
      
    } while (Display::nextPage());
   
    Display::setFont(FONT_DYN);
 }




 void Display::dispTemp(int16_t* _temp)
 {
   if (*_temp != m_lastTemp)
   {
      m_lastTemp = *_temp;

      Display::setPartialWindow(10, 50, 50, 30);
      Display::firstPage();
      do
      {
         Display::setCursor(10, 75);
         Display::print(*_temp);

      } while (Display::nextPage());
   }
 }

 void Display::dispGasLevel(uint8_t _gradToDisplay)
 {
   if (_gradToDisplay != m_LastGaugeState)
   {
      m_LastGaugeState = _gradToDisplay;
      
      if (_gradToDisplay == 1)
      {
       Display::dispReserve();
      }
      
      else
      {
            // Optimisation : Etat d'affichage de la jauge repr??sent??e avec un tableau de pointeurs
            // L'??tat (Blanc ou Noir) est stock?? dans deux variables allou?? dynamiquement ?? l'appel ?? de la fonction.
            // Oui, l'allocation dynamique c'est juste pour se la p??ter, en vrai ca sert ?? rien dans ce cas.
         
         static uint16_t colorBlack = GxEPD_BLACK;
         static uint16_t colorWhite = GxEPD_WHITE;
         
         uint16_t* graduationGauge[6];

         for (uint8_t i = 0; i < _gradToDisplay; i++)
         {
            graduationGauge[i] = &colorBlack;
         }

         for (uint8_t i = _gradToDisplay; i < 6; i++)
         {
            graduationGauge[i] = &colorWhite;
         }

         
         
         Display::setPartialWindow(2, 152, 196, 40);
         Display::firstPage();
         do
         {
            Display::fillScreen(GxEPD_WHITE);

            Display::fillRoundRect(Px1, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[0]);
            Display::fillRoundRect(Px2, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[1]);
            Display::fillRoundRect(Px3, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[2]);

            Display::fillRoundRect(Px4, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[3]);
            Display::fillRoundRect(Px5, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[4]);
            Display::fillRoundRect(Px6, Py1, G_UNIT_W, 39, G_RAD, *graduationGauge[5]);
            
               // Du coup c'est cette partie l?? qui rame...

               /* 
               Pour dessiner un rectangle, la lib appelle r??cursivement des fonctions primitives de dessin.
               Pour dessiner un rectangle : elle appelle r??cursivement grace ?? une boucle "for" la fonction "drawFastVLine"
               Et ?? chaque appel de fonction, la valeur de couleur est COPIEE !

               Je pense qu'ils ont fait ??a pour que la lib soit "Noob Friendly"... La couleur est d??finie dans une macro,
               et pour que n'importe laquel de ces fonctions primitives puisse ??tre appel??e dans le main, la fonction prend en argumant la VALEUR (sur 16bits).
               La valeur est donc COPIEE des milliers de fois pour dessiner les jauge...

               Il aurait sans dout?? ??t?? plus pertinant de d??clarer les differentes couleurs dans une variable (comme j'ai fais plus haut),
               et d'y acceder avec un pointeur. ainsi, les appel multiples de fonctions ne ferait que se 
               passer des pointeurs et on gagnerais beaucoup de temp de traitement.

               Mais peut-??tre que je me trompe ?


                  J'essairai de r??-??crire cette partie avec un startWrite + DrawPixel dans des for
               */
         } while (Display::nextPage());

      }
   
   }

 }
  

void Display::dispReserve()
{
   Display::setPartialWindow(2, 152, 196, 40);
   Display::firstPage();
   do
   {
      Display::fillScreen(GxEPD_WHITE);

      Display::drawRoundRect(Px1, Py1, G_UNIT_W, 39, G_RAD, GxEPD_BLACK);

      Display::setCursor(5, 190);
      Display::print("R");

   } while (Display::nextPage());
     

}
 
 
 
 
 

 
 
 
 
 
 
    
    // Test method
 void Display::dispTime(uint32_t* _time)
 {
    
 /* Display::setFont(&FreeMonoBold12pt7b);
    Display::setTextColor(GxEPD_BLACK);
    
    Display::setRotation(1);   */
    Display::setPartialWindow(110, 25, 80, 50);
   
    Display::firstPage();
    do
    {
       
        Display::fillScreen(GxEPD_WHITE);
        Display::setCursor(115, 50);
        Display::print(*_time);
        
        
    } while (Display::nextPage());
    

 }
   
