#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Objects.h"

#define ScreenWidth 400
#define ScreenHeight 600

int cima, baixo, esquerda, direita, i, j;

//------------------------------------------Função Colisão-----------------------------------------------
bool Collision(ALLEGRO_BITMAP *player, ALLEGRO_BITMAP *player2, float x, float y, float ex, float ey, int width1, int height1, int width2, int height2) {
 if (x+width1<ex || x>ex+width2 || y+height1<ey || y>ey+height2) {
    return false;
 }
 else {
    cima=max(y,ey);
    baixo=min(y+height1,ey+height2);
    esquerda=max(x,ex);
    direita=min(x+width1,ex+width2);
    for (i=cima;i<baixo;i++) {
        for (j=esquerda;j<direita;j++) {
            al_lock_bitmap(player, al_get_bitmap_format(player), ALLEGRO_LOCK_READONLY);
            al_lock_bitmap(player2, al_get_bitmap_format(player2), ALLEGRO_LOCK_READONLY);
            ALLEGRO_COLOR color = al_get_pixel(player, j-x, i-y);
            ALLEGRO_COLOR color2 = al_get_pixel(player2, j-ex, i-ey);
            if (color.a!=0 && color2.a!=0) {
                return true;
            }
        }
    }
 }
 return false;
}
//________________________________________________________________________________________________


int main() {
 //----------------------------------------Inicialização------------------------------------------
 ALLEGRO_DISPLAY *display;
 al_init();
 al_set_new_display_flags(ALLEGRO_WINDOWED);
 display = al_create_display(ScreenWidth,ScreenHeight);

 al_install_keyboard();
 al_install_audio();
 al_init_acodec_addon();
 al_init_image_addon();
 al_init_font_addon();
 al_init_ttf_addon();
 al_reserve_samples(12);
 //_______________________________________________________________________________________________


 //---------------------------------------Variáveis Allegro---------------------------------------
 ALLEGRO_SAMPLE *song = al_load_sample("nico.ogg");
 ALLEGRO_SAMPLE_INSTANCE *songinstance = al_create_sample_instance(song);
 ALLEGRO_SAMPLE *colisao = al_load_sample("explosao.wav");
 ALLEGRO_SAMPLE *tiros = al_load_sample("pew.wav");
 ALLEGRO_FONT *font = al_load_font("airstrike.ttf", 30, NULL);
 ALLEGRO_BITMAP *player = al_load_bitmap("nave.png");
 ALLEGRO_BITMAP *boss = al_load_bitmap("boss.png");
 ALLEGRO_BITMAP *asteroid = al_load_bitmap("asteroids.png");
 ALLEGRO_BITMAP *background = al_load_bitmap("background.jpg");
 ALLEGRO_BITMAP *tiro = al_load_bitmap("tiro.png");
 ALLEGRO_BITMAP *explode = al_load_bitmap("explode.png");
 ALLEGRO_BITMAP *vida = al_load_bitmap("heart.png");
 ALLEGRO_BITMAP *backgroundm = al_load_bitmap("menu_final.png");
 ALLEGRO_BITMAP *dificul = al_load_bitmap("menu_dificuldade.png");
 ALLEGRO_BITMAP *b_jogar = al_load_bitmap("BOTAO_JOGAR.png");
 ALLEGRO_BITMAP *b_regras = al_load_bitmap("BOTAO_REGRAS.png");
 ALLEGRO_BITMAP *b_sair = al_load_bitmap("BOTAO_SAIR.png");
 ALLEGRO_BITMAP *seta = al_load_bitmap("seta.png");
 ALLEGRO_BITMAP *regras = al_load_bitmap("regras.png");
 ALLEGRO_BITMAP *over = al_load_bitmap("game_over.png");
 ALLEGRO_KEYBOARD_STATE keystate;
 ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
 ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);
 ALLEGRO_TIMER *frtimer = al_create_timer(1.0/10.0);
 ALLEGRO_EVENT event;
 ALLEGRO_EVENT event2;
 //_______________________________________________________________________________________________


 //-------------------------------------Funções Allegro-------------------------------------------
 al_register_event_source(queue, al_get_keyboard_event_source());
 al_register_event_source(queue, al_get_timer_event_source(timer));
 al_register_event_source(queue, al_get_timer_event_source(frtimer));
 al_register_event_source(queue, al_get_display_event_source(display));
 al_set_sample_instance_playmode(songinstance, ALLEGRO_PLAYMODE_LOOP);
 al_attach_sample_instance_to_mixer(songinstance, al_get_default_mixer());
 //_______________________________________________________________________________________________


 //---------------------------------------Variaveis Jogo------------------------------------------
 nave nave;
 explosao xp;
 tirost tir;
 asteroide aster[3];
 bool done = true, draw = true, active = false, menu = false, game = true;
 float by1=0, by2=-600, vx[3];
 float bspeed=5, sy=190, rx=1000, ry=1000, rx2=600, ry2=800;
 int score=0, score2=0, vi=0;
 nave.x=170;
 nave.y=526;
 nave.speed=6;
 nave.an_x=0;
 xp.x=-75;
 xp.y=-75;
 xp.an_x=0;
 xp.an_y=0;
 xp.an_t=1;
 tir.x=-50;
 tir.y=-50;
 tir.speed=18;
 aster[0].x=0;
 aster[0].y=-64;
 aster[0].speed=9;
 aster[0].mov=102;
 aster[0].t=73;
 aster[1].x=336;
 aster[1].y=-124;
 aster[1].mov=49;
 aster[1].t=336;
 aster[2].x=210;
 aster[2].y=-200;
 aster[2].mov=177;
 aster[2].t=210;
 aster[0].an_x=0;
 aster[0].an_y=0;
 aster[0].an_t=1;
 vx[0]=310;
 vx[1]=340;
 vx[2]=370;
 //______________________________________________________________________________________________

 al_play_sample_instance(songinstance);
 al_start_timer(timer);
 al_start_timer(frtimer);
 while (!menu) {

    //-----------------------------------Desenho e lógica do menu--------------------------------
    al_draw_bitmap(backgroundm, 0, 0, 0);
    al_draw_bitmap(b_jogar, 20, 160, 0);
    al_draw_bitmap(b_regras, 20, 310, 0);
    al_draw_bitmap(b_sair, 20, 460, 0);
    al_draw_bitmap(dificul, rx2, ry2, NULL);
    al_draw_bitmap(seta, 320, sy, NULL);
    al_draw_bitmap(regras, rx, ry,0);
    al_wait_for_event(queue, &event2);
    al_get_keyboard_state(&keystate);
    if (event2.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        done=true;
        menu=true;
    }
    if (event2.type == ALLEGRO_EVENT_KEY_UP) {
      switch (event2.keyboard.keycode) {
      case ALLEGRO_KEY_DOWN:
       if (rx2!=0) {
         sy+=150;
       }
       else {
         sy+=85;
       }
       if (sy>=500) {
         sy=190;
       }
       break;
      case ALLEGRO_KEY_UP:
       if (rx2!=0) {
         sy-=150;
         if (sy<150) {
           sy=490;
         }
       }
       else {
         sy-=85;
         if (sy<150) {
           sy=445;
         }
       }
       break;
      case ALLEGRO_KEY_ENTER:
       if (rx2==0) {
         done=false;
         score=0;
         score2=0;
         vx[0]=310;
         vx[1]=340;
         vx[2]=370;
         vi=0;
         aster[0].x=0;
         aster[0].y=-64;
         aster[1].x=336;
         aster[1].y=-124;
         aster[2].x=210;
         aster[2].y=-200;
         nave.x=170;
         nave.y=526;
         xp.x=-75;
         xp.y=-75;
         bspeed=5;
         by1=0;
         by2=-600;
         if (sy==190) {
           aster[0].speed=9;
         }
         if (sy==275) {
           score2=2000;
           aster[0].speed=9;
         }
         if (sy==360) {
           score2=5000;
           aster[0].speed=9;
         }
         if (sy==445) {
           score2=5000;
           aster[0].speed=15;
           bspeed=8;
         }
       }
       if (sy==190) {
         rx2=0;
         ry2=0;
         sy=190;
       }
       if (sy==340) {
         rx=0;
         ry=0;
         sy=10000;
       }
       if (sy==490) {
         menu=true;
       }
       break;
      case ALLEGRO_KEY_ESCAPE:
        rx=1000;
        ry=1000;
        rx2=600;
        ry2=800;
        sy=190;
      }
    }
    //_____________________________________________________________________________________

 while (!done) {
    al_wait_for_event(queue, &event);

    //------------------------------Condição para game over--------------------------------
    if (vx[2]==410) {
      done=true;
      game=false;
    }
    //_____________________________________________________________________________________


    //--------------------------------Dificuldade: Insano---------------------------------
    if (score>=20000) {
      aster[0].speed=15;
      bspeed=8;
    }
    //_____________________________________________________________________________________


    //-------------------------------------Sair do jogo------------------------------------
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_ESCAPE:
          done=true;
          game=false;
          rx2=600;
          ry2=800;
          sy=190;
        }
    }
    else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        done=true;
        menu=true;
    }
    //____________________________________________________________________________________

    if (event.type == ALLEGRO_EVENT_TIMER) {
        if (event.timer.source == timer) {
            active = true;
            tir.y-=tir.speed;

            //---------------------------Lógica animação explosao-------------------------
            xp.an_x+=64;
            if (xp.an_x==512) {
              xp.an_x=0;
              xp.an_y=xp.an_t*64;
              xp.an_t++;
              if (xp.an_y==384) {
                xp.x=-75;
                xp.y=-75;
                xp.an_y=0;
                xp.an_t=1;
              }
            }
            //____________________________________________________________________________


            //--------------------------Movimentação background---------------------------
            by1+=bspeed;
            if (by1>=600)
               by1=-600;
            by2+=bspeed;
            if (by2>=600)
               by2=-600;
            //____________________________________________________________________________


            //-------------------------Lógica movimentação asteróides---------------------
            aster[0].y+=aster[0].speed;
            if (aster[0].y>=ScreenHeight) {
              score+=100;
              score2+=100;
              aster[0].y=-64;
              aster[0].x+=aster[0].mov;
              if (aster[0].x+64>=ScreenWidth) {
                aster[0].x=aster[0].t;
                aster[0].t+=73;
                if (aster[0].t+64>=400) {
                  aster[0].t=0;
                }
              }
            }
            if (score2>=2000) {
              aster[1].y+=aster[0].speed;
              if (aster[1].y>=ScreenHeight) {
                score+=100;
                score2+=100;
                aster[1].y=-64;
                aster[1].x-=aster[1].mov;
                if (aster[1].x<0) {
                  aster[1].x=aster[1].t;
                  aster[1].t-=100;
                  if (aster[1].t<0) {
                    aster[1].t=336;
                  }
                }
              }
            }
            if (score2>=5000) {
              aster[2].y+=aster[0].speed;
              if (aster[2].y>=ScreenHeight) {
                score+=100;
                score2+=100;
                aster[2].y=-64;
                aster[2].x+=aster[2].mov;
                if (aster[2].x+64>=ScreenWidth) {
                  aster[2].x=aster[2].t;
                  aster[2].t+=73;
                  if (aster[2].t+64>=400) {
                    aster[2].t=0;
                  }
                }
              }
            }
            //_______________________________________________________________________________


            //-------------------------------Lógica teclado----------------------------------
            al_get_keyboard_state(&keystate);
            if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT)) {
             if (nave.x+64<ScreenWidth) {
              nave.x+=nave.speed;
             }
            }
            else if (al_key_down(&keystate, ALLEGRO_KEY_LEFT)) {
             if (nave.x>0) {
              nave.x-=nave.speed;
             }
            }
            else
                active = false;
            if (al_key_down(&keystate, ALLEGRO_KEY_SPACE)) {
              tir.y=nave.y+32;
              tir.x=nave.x+24;
              al_play_sample(tiros, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
            }
            else
              active = false;
            //______________________________________________________________________________

        }
        if (event.timer.source == frtimer) {

            //---------------------------Animação asteróides--------------------------------
            aster[0].an_x+=64;
            if (aster[0].an_x==512) {
              aster[0].an_x=0;
              aster[0].an_y=aster[0].an_t*64;
              aster[0].an_t++;
              if (aster[0].an_y==512) {
                aster[0].an_y=0;
                aster[0].an_t=1;
              }
            }
            //______________________________________________________________________________


            //-------------------------------Animação nave----------------------------------
            nave.an_x+=64;
            if (nave.an_x==256)
              nave.an_x=0;
            //______________________________________________________________________________
        }
        draw=true;
    }

    //-----------------------------------------Colisões-------------------------------------
    if (Collision(player, asteroid, nave.x, nave.y, aster[0].x, aster[0].y, al_get_bitmap_width(player)/4, al_get_bitmap_height(player), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
       al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
       xp.an_x=0;
       xp.an_y=0;
       xp.x=aster[0].x;
       xp.y=aster[0].y;
       aster[0].y=600;
       score-=100;
       score2-=100;
       vx[vi]=410;
       vi++;
       if (vi>=4)
         vi=0;
    }
    if (Collision(player, asteroid, nave.x, nave.y, aster[1].x, aster[1].y, al_get_bitmap_width(player)/4, al_get_bitmap_height(player), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
       al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
       xp.an_x=0;
       xp.an_y=0;
       xp.x=aster[1].x;
       xp.y=aster[1].y;
       aster[1].y=600;
       score-=100;
       score2-=100;
       vx[vi]=410;
       vi++;
       if (vi>=4)
         vi=0;
    }
    if (Collision(player, asteroid, nave.x, nave.y, aster[2].x, aster[2].y, al_get_bitmap_width(player)/4, al_get_bitmap_height(player), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
      al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
      xp.an_x=0;
      xp.an_y=0;
      xp.x=aster[2].x;
      xp.y=aster[2].y;
      aster[2].y=600;
      score-=100;
      score2-=100;
      vx[vi]=410;
      vi++;
      if (vi>=4)
        vi=0;
    }
    if (Collision(tiro, asteroid, tir.x, tir.y, aster[0].x, aster[0].y, al_get_bitmap_width(tiro), al_get_bitmap_height(tiro), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
      al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
      xp.x=aster[0].x;
      xp.y=aster[0].y;
      xp.an_x=0;
      xp.an_y=0;
      tir.x=-50;
      tir.y=-50;
      aster[0].y=600;
      score+=50;
      score2+=50;
    }
    if (Collision(tiro, asteroid, tir.x, tir.y, aster[1].x, aster[1].y, al_get_bitmap_width(tiro), al_get_bitmap_height(tiro), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
      al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
      xp.x=aster[1].x;
      xp.y=aster[1].y;
      xp.an_x=0;
      xp.an_y=0;
      tir.x=-50;
      tir.y=-50;
      aster[1].y=600;
      score+=50;
      score2+=50;
    }
    if (Collision(tiro, asteroid, tir.x, tir.y, aster[2].x, aster[2].y, al_get_bitmap_width(tiro), al_get_bitmap_height(tiro), al_get_bitmap_width(asteroid)/8, al_get_bitmap_height(asteroid)/8)) {
      al_play_sample(colisao, 0.5, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
      xp.x=aster[2].x;
      xp.y=aster[2].y;
      xp.an_x=0;
      xp.an_y=0;
      tir.x=-50;
      tir.y=-50;
      aster[2].y=600;
      score+=50;
      score2+=50;
    }
    //________________________________________________________________________________________


    //-------------------------------------Imagens do jogo------------------------------------
    if (draw) {
        draw=false;
        al_draw_bitmap(background, 0, by1, NULL);
        al_draw_bitmap(background, 0, by2, NULL);
        al_draw_bitmap(tiro, tir.x, tir.y, NULL);
        al_draw_bitmap_region(player, nave.an_x, 0, 64, 64, nave.x, nave.y, NULL);
        al_draw_bitmap_region(asteroid, aster[0].an_x, aster[0].an_y, 64, 64, aster[0].x, aster[0].y, NULL);
        if (score2>=2000) {
          al_draw_bitmap_region(asteroid, aster[0].an_x, aster[0].an_y, 64, 64, aster[1].x, aster[1].y, NULL);
        }
        if (score2>=5000) {
          al_draw_bitmap_region(asteroid, aster[0].an_x, aster[0].an_y, 64, 64, aster[2].x, aster[2].y, NULL);
        }
        al_draw_bitmap_region(explode, xp.an_x, xp.an_y, 64, 64, xp.x, xp.y, NULL);
        al_draw_textf(font, al_map_rgb(255,255,255), 0, 0, NULL, "%d", score);
        al_draw_bitmap(vida, vx[0], 0, NULL);
        al_draw_bitmap(vida, vx[1], 0, NULL);
        al_draw_bitmap(vida, vx[2], 0, NULL);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));
    }
    //________________________________________________________________________________________

 }

 //-----------------------------------Tela game over------------------------------------------
 while (!game) {
  al_draw_bitmap(over, 0, 0, NULL);
  al_draw_textf(font, al_map_rgb(255,255,255), 105, 320, NULL, "SCORE: %d", score);
  al_wait_for_event(queue, &event2);
  if (event2.type == ALLEGRO_EVENT_KEY_UP) {
    switch (event2.keyboard.keycode)
    case ALLEGRO_KEY_ESCAPE:
      game=true;
      rx2=600;
      ry2=800;
      sy=190;
      break;
  }
  //__________________________________________________________________________________________

  al_flip_display();
  al_clear_to_color(al_map_rgb(0,0,0));
 }
  al_flip_display();
  al_clear_to_color(al_map_rgb(0,0,0));
 }
 al_destroy_timer(timer);
 al_destroy_timer(frtimer);
 al_destroy_font(font);
 al_destroy_bitmap(player);
 al_destroy_bitmap(asteroid);
 al_destroy_bitmap(tiro);
 al_destroy_bitmap(explode);
 al_destroy_bitmap(background);
 al_destroy_bitmap(vida);
 al_destroy_bitmap(backgroundm);
 al_destroy_bitmap(b_jogar);
 al_destroy_bitmap(b_regras);
 al_destroy_bitmap(b_sair);
 al_destroy_bitmap(regras);
 al_destroy_bitmap(seta);
 al_destroy_bitmap(boss);
 al_destroy_bitmap(dificul);
 al_destroy_bitmap(over);
 al_destroy_event_queue(queue);
 al_destroy_sample(colisao);
 al_destroy_sample(tiro);
 al_destroy_sample(song);
 al_destroy_sample_instance(songinstance);
 al_destroy_display(display);
 return 0;
 }
